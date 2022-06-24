#include <future>
#include <thread>
#include <deque>

class BlurWorker {
public:
    BlurWorker();

    void addTask(std::packaged_task<void()> task);

	void stop();

	void waitForCompletion();

private:
	std::atomic_bool m_stop { false };
    std::thread m_worker;
    std::deque<std::packaged_task<void()>> m_tasks;

    void consumeTasks();
};
