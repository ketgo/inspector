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

// TODO: Correctly handle extreamly out of order events.
// TODO: Better sliding window implementation which in turn results in better
// outlier (extreamly out of order events) detection. The current implementation
// is extreamly sensitive to outliers and thus can adversly effect sliding
// window motion.

namespace inspector {
namespace internal {

/**
 * @brief Functor to compare timestamps of two timestamped values. It is used to
 * chronologically order elements in a priority queue.
 *
 * @tparam T The type of timestamped value.
 */
template <class T>
class TimestampCompare {
 public:
  bool operator()(const std::pair<int64_t, T>& lhs,
                  const std::pair<int64_t, T>& rhs) const;
};

template <class T>
bool TimestampCompare<T>::operator()(const std::pair<int64_t, T>& lhs,
                                     const std::pair<int64_t, T>& rhs) const {
  return lhs.first > rhs.first;
}

}  // namespace internal

/**
 * @brief Priority queue containing timestamped values in chronologically order.
 *
 * @tparam T The type of timestamped value.
 */
template <class T>
using ChronologicalPriorityQueue =
    std::priority_queue<std::pair<int64_t, T>,
                        std::vector<std::pair<int64_t, T>>,
                        internal::TimestampCompare<T>>;

/**
 * @brief Thread-safe priority queue containing chronologically ordered
 * timestamped values with the latest and oldest values have a timestamp
 * difference less than equal to a specified window size.
 *
 * @tparam T The type of timestamped value.
 * @tparam WindowType The type of sliding window.
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
   * @brief Construct a new SlidingWindowPriorityQueue object.
   *
   * @param window_size Size of the time window.
   */
  SlidingWindowPriorityQueue(const int64_t window_size);

  /**
   * @brief Push a given timestamped value into the queue.
   *
   * The method pushes a given timestamped value into the priority queue. If the
   * queue is full, it blocks until the value is pushed. The queue is taken to
   * be full if pushing the timestamped value slides the time window. The window
   * is allowed to be slided only when the oldest value in the queue is popped.
   *
   * @param value Constant reference to the timestamped value.
   */
  void Push(const value_type& value);

  /**
   * @brief Push a given timestamped value into the queue.
   *
   * The method pushes a given timestamped value into the priority queue. If the
   * queue is full, it blocks until the value is pushed. The queue is taken to
   * be full if pushing the timestamped value slides the time window. The window
   * is allowed to be slided only when the oldest value in the queue is popped.
   *
   * @param value Rvalue reference to the timestamped value.
   */
  void Push(value_type&& value);

  /**
   * @brief Try to push a timestamped value into the queue.
   *
   * The method attempts to push a given timestamped value into the priority
   * queue. If the queue is full then no operation is performed and `false` is
   * returned. Otherwise the value is pushed and `true` is returned.
   *
   * @param value Constant reference to the timestamped value.
   * @returns `true` on success else `false`.
   */
  bool TryPush(const value_type& value);

  /**
   * @brief Try to push a timestamped value into the queue.
   *
   * The method attempts to push a given timestamped value into the priority
   * queue. If the queue is full then no operation is performed and `false` is
   * returned. Otherwise the value is pushed and `true` is returned.
   *
   * @param value Rvalue reference to the timestamped value.
   * @returns `true` on success else `false`.
   */
  bool TryPush(value_type&& value);

  /**
   * @brief Pop the oldest timestamped value form the queue.
   *
   * The method pops the oldest timestamped value from the queue. If the queue
   * is empty, it blocks until a value can be popped.
   *
   * @param value Reference to the object to store the popped value.
   */
  void Pop(value_type& value);

  /**
   * @brief Try to pop the oldest timestamped value from the queue.
   *
   * The method attempts to pop the oldest timestampled value from the queue. If
   * the queue is empty, no operation is performed and `false` is returned.
   * Otherwise the oldest value is popped and `true` is returned.
   *
   * @param value Reference to the object to store the popped value.
   * @return `true` on success else `false`.
   */
  bool TryPop(value_type& value);

  /**
   * @brief Get the oldest element in the priority queue.
   *
   * @returns Oldest element in the queue.
   */
  value_type Top() const;

  /**
   * @brief Get the number of elements in the queue.
   *
   * @returns Number of elements in the queue.
   */
  size_type Size() const;

 private:
  const int64_t window_size_;
  int64_t lower_bound_;
  int64_t upper_bound_;
  ChronologicalPriorityQueue<T> queue_;
  mutable std::mutex mutex_;
  std::condition_variable cv_not_full_, cv_not_empty_;
};

// -------------------------------------------
// SlidingWindowPriorityQueue Implementation
// -------------------------------------------

template <class T>
SlidingWindowPriorityQueue<T>::SlidingWindowPriorityQueue(
    const int64_t window_size)
    : window_size_(window_size), lower_bound_(0), upper_bound_(0) {}

template <class T>
void SlidingWindowPriorityQueue<T>::Push(const value_type& value) {
  {
    std::unique_lock<std::mutex> lock(mutex_);

    // Check if the given value has a timestamp less than the current sliding
    // window lower bound. If so then skip operation.
    if (value.first < lower_bound_) {
      return;
    }

    //  Wait till the queue is not full. The queue is considered to be full if
    //  the new lower bound is still less than the timestamp of the oldest
    //  value in the queue.
    cv_not_full_.wait(lock, [&]() {
      auto lower_bound = (value.first > upper_bound_)
                             ? value.first - window_size_
                             : lower_bound_;
      return queue_.empty() || queue_.top().first > lower_bound;
    });

    queue_.push(value);
    lower_bound_ = (value.first > upper_bound_) ? value.first - window_size_
                                                : lower_bound_;
    upper_bound_ = lower_bound_ + window_size_;
  }
  // Notify all consumer threads waiting for the queue to not be empty.
  cv_not_empty_.notify_all();
}

template <class T>
void SlidingWindowPriorityQueue<T>::Push(value_type&& value) {
  {
    std::unique_lock<std::mutex> lock(mutex_);

    // Check if the given value has a timestamp less than the current sliding
    // window lower bound. If so then skip operation.
    if (value.first < lower_bound_) {
      return;
    }

    //  Wait till the queue is not full. The queue is considered to be full if
    //  the new lower bound is still less than the timestamp of the oldest
    //  value in the queue.
    cv_not_full_.wait(lock, [&]() {
      auto lower_bound = (value.first > upper_bound_)
                             ? value.first - window_size_
                             : lower_bound_;

      return queue_.empty() || queue_.top().first > lower_bound;
    });

    queue_.push(std::move(value));
    lower_bound_ = (value.first > upper_bound_) ? value.first - window_size_
                                                : lower_bound_;
    upper_bound_ = lower_bound_ + window_size_;
  }
  // Notify all consumer threads waiting for the queue to not be empty.
  cv_not_empty_.notify_all();
}

template <class T>
bool SlidingWindowPriorityQueue<T>::TryPush(const value_type& value) {
  std::unique_lock<std::mutex> lock(mutex_);

  // Check if the given value has a timestamp less than the current sliding
  // window lower bound. If so then skip operation.
  if (value.first < lower_bound_) {
    return false;
  }

  //  Check if the queue is not full. The queue is considered to be full if
  //  the new lower bound is still less than the timestamp of the oldest
  //  value in the queue.
  auto lower_bound =
      (value.first > upper_bound_) ? value.first - window_size_ : lower_bound_;
  if (queue_.empty() || queue_.top().first > lower_bound) {
    queue_.push(value);
    lower_bound_ = lower_bound;
    upper_bound_ = lower_bound_ + window_size_;
    return true;
  }
  // Queue is full so no operation is performed
  return false;
}

template <class T>
bool SlidingWindowPriorityQueue<T>::TryPush(value_type&& value) {
  std::unique_lock<std::mutex> lock(mutex_);

  // Check if the given value has a timestamp less than the current sliding
  // window lower bound. If so then skip operation.
  if (value.first < lower_bound_) {
    return false;
  }

  //  Check if the queue is not full. The queue is considered to be full if
  //  the new lower bound is still less than the timestamp of the oldest
  //  value in the queue.
  auto lower_bound =
      (value.first > upper_bound_) ? value.first - window_size_ : lower_bound_;
  if (queue_.empty() || queue_.top().first > lower_bound) {
    queue_.push(std::move(value));
    lower_bound_ = lower_bound;
    upper_bound_ = lower_bound_ + window_size_;
    return true;
  }
  // Queue is empty so no operation is performed
  return false;
}

template <class T>
void SlidingWindowPriorityQueue<T>::Pop(value_type& value) {
  {
    std::unique_lock<std::mutex> lock(mutex_);

    // Wait till the queue is not empty.
    cv_not_empty_.wait(lock, [&]() { return !queue_.empty(); });
    value = queue_.top();
    queue_.pop();
  }
  // Notify all producer threads waiting for the queue to not be full.
  cv_not_full_.notify_all();
}

template <class T>
bool SlidingWindowPriorityQueue<T>::TryPop(value_type& value) {
  std::unique_lock<std::mutex> lock(mutex_);

  if (!queue_.empty()) {
    value = queue_.top();
    queue_.pop();
    return true;
  }
  return false;
}

template <class T>
typename SlidingWindowPriorityQueue<T>::value_type
SlidingWindowPriorityQueue<T>::Top() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return queue_.top();
}

template <class T>
typename SlidingWindowPriorityQueue<T>::size_type
SlidingWindowPriorityQueue<T>::Size() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return queue_.size();
}

// -------------------------------------------

}  // namespace inspector