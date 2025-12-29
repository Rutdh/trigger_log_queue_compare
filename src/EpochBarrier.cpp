#include "EpochBarrier.h"
#include <mutex>


EpochBarrier::EpochBarrier(int producer_num, int consumer_num)
  : pending_producer_(producer_num)
  , pending_consumer_(consumer_num)
  , producer_num_(producer_num)
  , consumer_num_(consumer_num) {
  for (int i = 0; i < producer_num_; ++i) {
    workers_.emplace_back([this] {

    });
  }
  
}

void EpochBarrier::WorkerLoop(bool is_producer) {
  int local_epoch = 0;
  while (true) {
    std::function<void()> task;
    int current_epoch = 0;
    {
      std::unique_lock<std::mutex> lock(mtx_);
      cv_start_.wait(lock, [this, local_epoch] {
        return !running_ || epoch_ > local_epoch;
      });
    }
  }
}
