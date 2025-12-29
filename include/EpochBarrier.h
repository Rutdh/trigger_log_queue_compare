#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
class EpochBarrier {
  EpochBarrier(int producer_num, int consumer_num);
private:
  void WorkerLoop(bool is_producer);

  std::mutex mtx_;
  std::condition_variable cv_start_;
  std::condition_variable cv_done_;
  int producer_num_;
  int consumer_num_;
  int pending_producer_;
  int pending_consumer_;
  std::function<void()> producer_func_;
  std::function<void()> consumer_func_;
  std::vector<std::thread> workers_;
  bool running_;
  int epoch_;
};