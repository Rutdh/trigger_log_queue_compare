#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

/**
 * @brief 简单的基准测试多线程运行器
 *
 * 目的: 避免在 nanobench 循环中反复创建/销毁线程带来的噪音。使用 epoch 代际驱动每一轮任务，规避 start/reset 竞态。
 */
class BenchmarkRunner {
public:
    BenchmarkRunner(int producer_count, int consumer_count)
        : running_(true),
          producer_count_(producer_count),
          consumer_count_(consumer_count) {
        for (int i = 0; i < producer_count_; ++i) {
            workers_.emplace_back([this] { WorkerLoop(true); });
        }
        for (int i = 0; i < consumer_count_; ++i) {
            workers_.emplace_back([this] { WorkerLoop(false); });
        }
    }

    ~BenchmarkRunner() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_ = false;
            producer_task_ = nullptr;
            consumer_task_ = nullptr;
        }
        cv_start_.notify_all();
        cv_done_.notify_all();

        for (auto& t : workers_) {
            if (t.joinable()) t.join();
        }
    }

    void Run(std::function<void()> producer_func, std::function<void()> consumer_func) {
        std::unique_lock<std::mutex> lock(mutex_);
        producer_task_ = producer_func;
        consumer_task_ = consumer_func;
        pending_producers_ = producer_count_;
        pending_consumers_ = consumer_count_;
        ++epoch_;

        cv_start_.notify_all();

        cv_done_.wait(lock, [this] {
            return pending_producers_ == 0 && pending_consumers_ == 0;
        });
    }

private:
    void WorkerLoop(bool is_producer) {
        int local_epoch = 0;
        while (true) {
            std::function<void()> task;
            int current_epoch = 0;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                cv_start_.wait(lock, [this, local_epoch] {
                    return !running_ || epoch_ > local_epoch;
                });
                if (!running_) return;
                task = is_producer ? producer_task_ : consumer_task_;
                current_epoch = epoch_;
            }

            if (task) task();

            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (is_producer) {
                    --pending_producers_;
                } else {
                    --pending_consumers_;
                }
                if (pending_producers_ == 0 && pending_consumers_ == 0) {
                    cv_done_.notify_one();
                }
            }

            local_epoch = current_epoch;
        }
    }

    std::vector<std::thread> workers_;
    std::mutex mutex_;
    std::condition_variable cv_start_;
    std::condition_variable cv_done_;

    bool running_;
    int producer_count_;
    int consumer_count_;
    int pending_producers_ = 0;
    int pending_consumers_ = 0;
    int epoch_ = 0;

    std::function<void()> producer_task_;
    std::function<void()> consumer_task_;
};
