#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>

namespace utils {

template <typename T>
class SynchronizedQueue {
 public:
  SynchronizedQueue() = default;
  ~SynchronizedQueue() = default;

  void push(const T& value) {
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push(value);
    _condition.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(_mutex);
    _condition.wait(lock, [this]() { return !_queue.empty(); });
    T item = std::move(_queue.front());
    _queue.pop();
    return item;
  }

 private:
  mutable std::mutex _mutex;
  std::queue<T> _queue;
  std::condition_variable _condition;
};

}  // namespace utils