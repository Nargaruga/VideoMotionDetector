#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <deque>
#include <future>
#include <iostream>
#include <mutex>
#include <vector>

/*
 *  Class that implements a thread pool.
 *  A queue handles tasks in a FIFO fashion, with insertion
 *  done externally through a public interface and extraction done
 *	by the threads of the pool.
 */
template <class R> class ThreadPool {
public:
    /*
     *	Creates a thread pool with `nThreads` workers.
     */
    ThreadPool(int nThreads) {
        if(nThreads < 1)
            return;

        m_nWorkers = nThreads;

        for(int i = 0; i < m_nWorkers; i++) {
            m_workers.push_back(std::thread(&ThreadPool::doWork, this, i));
        }
    };

    /*
     *	Closes and destroys the threadpool.
     */
    ~ThreadPool() {
        conclude();
    }

    /*
     *  Inserts `task` in the task queue and wakes a worker.
     */
    void insertTask(std::packaged_task<R()> task) {
        std::scoped_lock lck(m_mtx);
        m_tasks.push_back(std::move(task));
        m_pendingTasks++;
        m_cond.notify_one();
    };

    /*
     *  Thread `tid` keeps extracting and executing tasks
     *  until the thread pool closes.
     */
    void doWork(size_t tid) {
        while(true) {
            std::unique_lock<std::mutex> lck(m_mtx);
            while(m_tasks.empty()) {
                if(m_stop) {
                    // There are no tasks left and the thread pool is trying to close,
                    // we should return.
                    lck.unlock();
                    return;
                }

                m_cond.wait(lck);
            }

            // Get a task and execute it
            auto task = std::move(m_tasks.front());
            m_tasks.pop_front();

            lck.unlock();

            task();

            m_pendingTasks--;
            if(m_pendingTasks == 0)
                m_doneCond.notify_all();
        }
    }

    /*
     *	Waits for the queue to become empty, then returns.
     */
    void awaitCompletion() {
        std::unique_lock<std::mutex> lck(m_mtx);
        while(m_pendingTasks > 0) {
            m_doneCond.wait(lck);
        }
        lck.unlock();
    }

    /*
     *	Prepares to close the thread pool.
     */
    void conclude() {
        m_stop = true;
        m_cond.notify_all();
        for(int i = 0; i < m_nWorkers; i++) {
            m_workers[i].join();
        }
    };

private:
    std::atomic_bool m_stop = false;				// When true, the threadpool will close
    int m_nWorkers;                   				// Number of workers in the pool
    std::vector<std::thread> m_workers; 			// Pool of workers
    std::deque<std::packaged_task<R()>> m_tasks; 	// Task queue
    std::atomic_int m_pendingTasks = 0; 			// Number of pending tasks
    std::mutex m_mtx;               				// Mutex for synchronized access to the task queue
    std::condition_variable m_cond; 				// Needed to avoid active wait when the queue is empty
    std::condition_variable m_doneCond; 			// Used to wait for the queue to empty
};

#endif
