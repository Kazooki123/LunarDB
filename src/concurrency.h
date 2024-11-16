#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <vector>
#include <future>
#include <memory>

namespace lunardb {

class TaskQueue {
public:
    using Task = std::function<void()>;

    TaskQueue(size_t num_threads = std::thread::hardware_concurrency());
    ~TaskQueue();

    // Add a task to the queue
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    // Background task management
    void startBackgroundTasks();
    void stopBackgroundTasks();

    // Stats and monitoring
    size_t getQueueSize() const;
    size_t getActiveThreadCount() const;

private:
    std::vector<std::thread> workers;
    std::queue<Task> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
    std::atomic<size_t> active_threads;

    void workerThread();
};

class BackgroundProcessor {
public:
    BackgroundProcessor();
    ~BackgroundProcessor();

    // Schedule periodic tasks
    void scheduleTask(std::function<void()> task, std::chrono::milliseconds interval);

    // Cache maintenance tasks
    void scheduleCleanup();
    void schedulePersistence(const std::string& filename);

    // Control background processing
    void start();
    void stop();

private:
    struct ScheduledTask {
        std::function<void()> task;
        std::chrono::milliseconds interval;
        std::chrono::steady_clock::time_point next_run;
    };

    std::vector<ScheduledTask> scheduled_tasks;
    std::mutex tasks_mutex;
    std::thread processor_thread;
    std::atomic<bool> running;

    void processingLoop();
};

}

#endif // CONCURRENCY_H
