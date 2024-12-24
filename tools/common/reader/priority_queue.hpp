/**
 * Copyright 2022 Ketan Goyal
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace inspector {

using timestamp_t = int64_t;
using duration_t = uint64_t;

namespace details {

/**
 * @brief Functor to compare timestamps of two timestamped values. It is used to
 * chronologically order elements in a priority queue.
 *
 * @tparam T The type of timestamped value.
 */
template <class T>
class TimestampCompare {
 public:
  bool operator()(const std::pair<timestamp_t, T>& lhs,
                  const std::pair<timestamp_t, T>& rhs) const;
};

template <class T>
bool TimestampCompare<T>::operator()(
    const std::pair<timestamp_t, T>& lhs,
    const std::pair<timestamp_t, T>& rhs) const {
  return lhs.first >= rhs.first;
}

/**
 * @brief Priority queue containing timestamped values in chronologically order.
 *
 * @tparam T The type of timestamped value.
 */
template <class T>
using ChronologicalPriorityQueue =
    std::priority_queue<std::pair<timestamp_t, T>,
                        std::vector<std::pair<timestamp_t, T>>,
                        TimestampCompare<T>>;

/**
 * @brief Thread-safe priority queue containing chronologically ordered
 * timestamped values where the latest and oldest values have a timestamp
 * difference fall between specified min and max window size.
 *
 * @tparam T The type of timestamped value. Required to be default
 * constructable.
 */
template <class T>
class SlidingWindowPriorityQueue {
  using queue_t = ChronologicalPriorityQueue<T>;

 public:
  /**
   * @brief Type traits
   *
   */
  using value_type = typename queue_t::value_type;
  using size_type = typename queue_t::size_type;

  /**
   * @brief Enumerates collection of potential results of operation on the
   * queue.
   *
   */
  enum class Result {
    kSuccess = 0,
    kClosed,
    kOutOfOrder,
    kFull,
    kEmpty,
  };

  /**
   * @brief Construct a new SlidingWindowPriorityQueue object.
   *
   * @param min_window_size Minimum slidding window size below which popping of
   * events is blocked.
   * @param max_window_size Maximum slidding window size above which pusshing of
   * events is blocked.
   */
  SlidingWindowPriorityQueue(const duration_t min_window_size,
                             const duration_t max_window_size);

  /**
   * @brief Destroy the Sliding Window Priority Queue object.
   *
   */
  ~SlidingWindowPriorityQueue();

  /**
   * @brief The method opens the queue so that timestamped values can be pushed
   * into the queue.
   *
   */
  void open();

  /**
   * @brief The method closes the queue so that no more timestamped values can
   * be pushed into the queue. Note that values can still be popped until the
   * queue is empty. As part of closing any waiting producers and/or consumers
   * are woken.
   *
   */
  void close();

  /**
   * @brief Check if the queue is closed.
   *
   * @return `true` if closed else `false`.
   */
  bool isClosed() const;

  /**
   * @brief Push a given timestamped value into the queue.
   *
   * The method pushes a given timestamped value into the priority queue. If the
   * queue is full, it blocks until the value is pushed. The queue is considered
   * to be full if pushing the timestamped value increases the slidding window
   * size above max size.
   *
   * @param value Constant reference to the timestamped value.
   * @returns Result of operation.
   */
  Result push(const value_type& value);

  /**
   * @brief Try to push a given timestamped value into the queue.
   *
   * The method tries to push a given timestamped value into the priority queue.
   * It is a non-blocking call.
   *
   * @param value Constant reference to the timestamped value.
   * @returns Result of operation.
   */
  Result tryPush(const value_type& value);

  /**
   * @brief Pop the oldest timestamped value form the queue.
   *
   * The method pops the oldest timestamped value from the queue. If the queue
   * has a window size less then min size, it blocks until a value can be
   * popped.
   *
   * @return Pair with the first element being result of the operation and the
   * second being the popped value.
   */
  std::pair<Result, value_type> pop();

  /**
   * @brief Try to pop the oldest timestamped value form the queue.
   *
   * The method tries to pop the oldest timestamped value from the queue. It is
   * a non-blocking call.
   *
   * @return Pair with the first element being result of the operation and the
   * second being the popped value.
   */
  std::pair<Result, value_type> tryPop();

 private:
  bool canPush(const timestamp_t timestamp) const;
  Result waitTillCanPush(std::unique_lock<std::mutex>& lock,
                         const timestamp_t timestamp) const;
  bool canPop() const;
  Result waitTillCanPop(std::unique_lock<std::mutex>& lock) const;

  const duration_t min_window_size_;
  const duration_t max_window_size_;
  bool closed_;
  timestamp_t lower_;
  timestamp_t upper_;
  queue_t queue_;
  mutable std::mutex mutex_;
  mutable std::condition_variable cv_can_push_, cv_can_pop_;
};

// -------------------------------------------
// SlidingWindowPriorityQueue Implementation
// -------------------------------------------

template <class T>
bool SlidingWindowPriorityQueue<T>::canPush(const timestamp_t timestamp) const {
  duration_t window_size =
      (upper_ >= timestamp) ? upper_ - lower_ : timestamp - lower_;
  return (queue_.empty() || (window_size <= max_window_size_));
}

template <class T>
typename SlidingWindowPriorityQueue<T>::Result
SlidingWindowPriorityQueue<T>::waitTillCanPush(
    std::unique_lock<std::mutex>& lock, const timestamp_t timestamp) const {
  cv_can_push_.wait(lock, [&]() { return !closed_ && canPush(timestamp); });

  if (closed_) {
    return Result::kClosed;
  }

  if (timestamp < lower_) {
    return Result::kOutOfOrder;
  }

  return Result::kSuccess;
}

template <class T>
bool SlidingWindowPriorityQueue<T>::canPop() const {
  duration_t window_size = upper_ - lower_;
  return window_size >= min_window_size_;
}

template <class T>
typename SlidingWindowPriorityQueue<T>::Result
SlidingWindowPriorityQueue<T>::waitTillCanPop(
    std::unique_lock<std::mutex>& lock) const {
  cv_can_pop_.wait(lock, [&]() { return closed_ || canPop(); });

  if (closed_ && queue_.empty()) {
    return Result::kClosed;
  }

  return Result::kSuccess;
}

// --- public ---

template <class T>
SlidingWindowPriorityQueue<T>::SlidingWindowPriorityQueue(
    const duration_t min_window_size, const duration_t max_window_size)
    : min_window_size_(min_window_size),
      max_window_size_(max_window_size),
      closed_(false),
      lower_(0),
      upper_(0) {}

template <class T>
SlidingWindowPriorityQueue<T>::~SlidingWindowPriorityQueue() {
  close();
}

template <class T>
void SlidingWindowPriorityQueue<T>::open() {
  std::unique_lock<std::mutex> lock(mutex_);
  closed_ = false;
}

template <class T>
void SlidingWindowPriorityQueue<T>::close() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    closed_ = true;
  }
  // Notify all the waiting producer and consumer threads.
  cv_can_pop_.notify_all();
  cv_can_push_.notify_all();
}

template <class T>
bool SlidingWindowPriorityQueue<T>::isClosed() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return closed_;
}

template <class T>
typename SlidingWindowPriorityQueue<T>::Result
SlidingWindowPriorityQueue<T>::push(const value_type& value) {
  {
    std::unique_lock<std::mutex> lock(mutex_);

    // The `if` clause only occurs for the very first element pushed into the
    // queue.
    if (lower_ == 0) {
      upper_ = value.first;
      lower_ = value.first;
    }

    auto result = waitTillCanPush(lock, value.first);
    if (result != Result::kSuccess) {
      return result;
    }

    queue_.push(value);
    upper_ = value.first > upper_ ? value.first : upper_;
  }

  cv_can_pop_.notify_all();

  return Result::kSuccess;
}

template <class T>
typename SlidingWindowPriorityQueue<T>::Result
SlidingWindowPriorityQueue<T>::tryPush(const value_type& value) {
  {
    std::unique_lock<std::mutex> lock(mutex_);

    // The `if` clause only occurs for the very first element pushed into the
    // queue.
    if (lower_ == 0) {
      upper_ = value.first;
      lower_ = value.first;
    }

    if (closed_) {
      return Result::kClosed;
    }

    if (value.first < lower_) {
      return Result::kOutOfOrder;
    }

    if (!canPush(value.first)) {
      return Result::kFull;
    }

    queue_.push(value);
    upper_ = value.first > upper_ ? value.first : upper_;
  }

  cv_can_pop_.notify_all();

  return Result::kSuccess;
}

template <class T>
std::pair<typename SlidingWindowPriorityQueue<T>::Result,
          typename SlidingWindowPriorityQueue<T>::value_type>
SlidingWindowPriorityQueue<T>::pop() {
  std::unique_lock<std::mutex> lock(mutex_);

  std::pair<Result, value_type> result;

  result.first = waitTillCanPop(lock);
  if (result.first != Result::kSuccess) {
    return result;
  }

  result.second = std::move(const_cast<value_type&>(queue_.top()));
  queue_.pop();
  lower_ = result.second.first;
  lock.unlock();

  cv_can_push_.notify_all();

  return result;
}

template <class T>
std::pair<typename SlidingWindowPriorityQueue<T>::Result,
          typename SlidingWindowPriorityQueue<T>::value_type>
SlidingWindowPriorityQueue<T>::tryPop() {
  std::unique_lock<std::mutex> lock(mutex_);

  std::pair<Result, value_type> result;

  if (closed_ && queue_.empty()) {
    result.first = Result::kClosed;
    return result;
  }

  if (!canPop()) {
    result.first = Result::kEmpty;
    return result;
  }

  result.second = std::move(const_cast<value_type&>(queue_.top()));
  queue_.pop();
  lower_ = result.second.first;
  lock.unlock();

  cv_can_push_.notify_all();

  return result;
}

// -------------------------------------------

}  // namespace details
}  // namespace inspector