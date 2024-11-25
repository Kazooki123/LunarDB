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
    mutable std::mutex queue_mutex;
    mutable std::mutex active_threads_mutex;
    std::condition_variable condition;
    bool stop;
    size_t active_threads;

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

/*
** Copyright 2024 Kazooki123
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the “Software”), to deal in the Software without restriction, including without
** limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
** of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
** conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial
** portions of the Software.
**
** THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
** LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
** THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**/

#endif // CONCURRENCY_H
