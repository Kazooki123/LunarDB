// Authors: Kazooki123, Starloexoliz

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


#include "concurrency.h"
#include "cache.h"
#include "saved.h"
#include <algorithm>
#include <chrono>

TaskQueue::TaskQueue(size_t num_threads)
    : stop(false), active_threads(0) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back(&TaskQueue::workerThread, this);
    }
}

TaskQueue::~TaskQueue() {
    stopBackgroundTasks();
}

void TaskQueue::workerThread() {
    while (true) {
        Task task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition.wait(lock, [this] {
                return stop || !tasks.empty();
            });

            if (stop && tasks.empty()) {
                return;
            }

            task = std::move(tasks.front());
            tasks.pop();
        }

        {
            std::lock_guard<std::mutex> lock(active_threads_mutex);
            active_threads++;
        }

        task(); // Execute the task

        {
            std::lock_guard<std::mutex> lock(active_threads_mutex);
            active_threads--;
        }
    }
}

template<typename F, typename... Args>
auto TaskQueue::enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop) {
            throw std::runtime_error("Cannot enqueue task on stopped TaskQueue");
        }

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

void TaskQueue::startBackgroundTasks() {
    stop = false;
}

void TaskQueue::stopBackgroundTasks() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (auto& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

size_t TaskQueue::getQueueSize() const {
    std::unique_lock<std::mutex> lock(queue_mutex);
    return tasks.size();
}

size_t TaskQueue::getActiveThreadCount() const {
    std::lock_guard<std::mutex> lock(active_threads_mutex);
    return active_threads;
}

BackgroundProcessor::BackgroundProcessor() : running(false) {}

BackgroundProcessor::~BackgroundProcessor() {
    stop();
}

void BackgroundProcessor::scheduleTask(std::function<void()> task,
                                     std::chrono::milliseconds interval) {
    std::lock_guard<std::mutex> lock(tasks_mutex);
    scheduled_tasks.push_back({
        std::move(task),
        interval,
        std::chrono::steady_clock::now() + interval
    });
}

void BackgroundProcessor::scheduleCleanup() {
    scheduleTask(
        []()
        {
            extern Cache cache;
            cache.cleanup_expired();
        },
        std::chrono::minutes(5));
}

void BackgroundProcessor::schedulePersistence(const std::string& filename) {
    scheduleTask(
        [filename]() {
            extern Cache cache;
            Saved::saveToFile(cache, filename);
        },
        std::chrono::minutes(15)
    );
}

void BackgroundProcessor::start() {
    if (!running.exchange(true)) {
        processor_thread = std::thread(&BackgroundProcessor::processingLoop, this);
    }
}

void BackgroundProcessor::stop() {
    if (running.exchange(false)) {
        if (processor_thread.joinable()) {
            processor_thread.join();
        }
    }
}

void BackgroundProcessor::processingLoop() {
    while (running) {
        auto now = std::chrono::steady_clock::now();

        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            for (auto& task : scheduled_tasks) {
                if (now >= task.next_run) {
                    task.task();
                    task.next_run = now + task.interval;
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
