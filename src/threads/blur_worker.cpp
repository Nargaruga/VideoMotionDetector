#include "blur_worker.h"

BlurWorker::BlurWorker() {
    m_worker = std::thread(&BlurWorker::consumeTasks);
}

void BlurWorker::addTask(std::packaged_task<void()> task) {
    m_tasks.push_back(task);
}

void BlurWorker::consumeTasks() {
    while(!m_stop) {
        auto task = std::move(m_tasks.front());
        m_tasks.pop_front();

        if(task.valid())
            task();
    }
}

void BlurWorker::stop() {
    m_stop = true;
}

void BlurWorker::waitForCompletion() {
    m_worker.join();
}
