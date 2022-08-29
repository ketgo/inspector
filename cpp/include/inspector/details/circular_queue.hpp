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

#include <inspector/details/span.hpp>
#include <inspector/details/circular_queue/allocator.hpp>

// TODO: Change the consumer API by introducing a DynamicSpan class.

namespace inspector {
namespace details {

/**
 * @brief The class `CircularQueue` is a lock-free and wait-free circular queue
 * supporting multiple concurrent consumers and producers.
 *
 * The circular queue is designed to be lock-free and wait-free supporting
 * multiple consumers and producers. It exposes two operations, `Publish` for
 * writing and `Consume` for reading. Details on the two are discussed in the
 * following sections.
 *
 *  *
 * <-------llllll0000000XXXXXXXXXXXXXXXXXXXaaaaaaaaaaabbbbbbbcccccccccc-------->
 *         ^     ^      ^                  ^          ^      ^         ^
 *         |     |      |                  |          |      |         |
 *     read[l] read[0]  read_head         write[a] write[b] write[c]  write_head
 *
 * MemoryBlock
 * -----------
 *
 * Data is written on a circular queue in the form of `MemoryBlock` data
 * structure. The data structure contains two parts: header and body. The header
 * contains the size of the body and possibly a checksum. The body contains the
 * data published by a producer.
 *
 * Write
 * -----
 *
 * In order to support multiple producers, the circular queue contains multiple
 * write cursors and a write head. When writing a message of size s, a producer
 * is required to reserves a memory block of size S bytes on the buffer. The
 * following steps are performed for allocation:
 *
 * 1. Try to get a free cursor from the pool of write cursors. If no cursor is
 *    available then return an error. This can also be a blocking call where we
 *    wait till the cursor is available.
 * 2. Check if the buffer has available size greater than S. If not then
 *    release the free cursor and return an error. We can also wait till the
 *    desired size is available. Note that S should never be greater than the
 *    buffer capacity.
 * 3. Attempt to reserve a memory block by storing the write head value in the
 *    obtained cursor and moving the head by S. Moving the head should be
 *    performed by CAS (compare and swap). Here we compare the current value
 *    with the original value of head before we attempted write. If the CAS
 *    operation fails then goto step 2.
 *
 * Once the desired size is allocated, the given message is written on the
 * buffer.
 *
 * Read
 * ----
 *
 * In order to support multiple consumers, the circular queue contains multiple
 * read cursors and a read head. When reading a message, a consumer is required
 * to request for a read cursor. The following steps are performed for
 * allotment:
 *
 * 1. Try to get a free cursor from the pool of read cursors. If no cursor is
 *    available then return an error. This can also be a blocking call where we
 *    wait till the cursor is available.
 * 2. Check that the read head is behind all the write cursors. If not then
 *    release the free cursor and return an error. We can also wait till the
 *    read head becomes behind all the write cursors.
 * 3. Attempt to reserve a memory block for reading by storing the read head
 *    value in the obtained cursor and moving the head by block size. The block
 *    size is stored in the first 8 bytes. Moving the head should be performed
 *    by CAS (compare and swap). Here we compare the current head value with the
 *    original value before we attempted write. If the CAS operation fails then
 *    goto step 2.
 *
 * Once the block is reserved, the message contained is loaded into a desired
 * variable from the buffer.
 *
 * @tparam T The type of object stored in the circular queue.
 * @tparam BUFFER_SIZE The size of memory buffer in bytes.
 * @tparam MAX_PRODUCERS The maximum number of producers.
 * @tparam MAX_CONSUMERS The maximum number of consumers.
 */
template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
class CircularQueue {
 public:
  /**
   * @brief Enumerated set of possible status.
   *
   */
  enum class Status {
    OK = 0,     // Queue is ok for publishing and consumption
    FULL = 1,   // Queue is full so can not publish
    EMPTY = 2,  // Queue is empty so can not consume
  };

  /**
   * @brief Span encapsulating a memory block in the circular queue reserved for
   * reading.
   *
   */
  using ReadSpan = circular_queue::MemoryBlockHandle<const T, BUFFER_SIZE>;

  /**
   * @brief Default maximum number of attempts made when publishing or consuming
   * data from the circular queue.
   *
   */
  constexpr static std::size_t defaultMaxAttempt() { return 32; }

  /**
   * @brief Get default cursor timeout in nano seconds.
   *
   */
  static constexpr uint64_t DefaultTimeoutNs() {
    return 2000000000UL;  // 2 seconds
  }

  /**
   * @brief Get default start marker value. A start marker is a unique marker
   * used to indicate start of a memory block and is primarly used in stale
   * block detection and recovery.
   *
   */
  static constexpr uint32_t DefaultStartMarker() { return 924926508; }

  /**
   * @brief Construct a new CircularQueue object.
   *
   * @param timeout_ns Cursor timeout in nano seconds.
   * @param start_marker Start marker used to split memory blocks. It is
   * primarly used to recover memory block size for stale blocks written by
   * abruptly terminated producers. Please use a value which is never going to
   * be a part of the actual message content.
   */
  CircularQueue(const uint64_t timeout_ns = DefaultTimeoutNs(),
                const uint32_t start_marker = DefaultStartMarker());

  /**
   * @brief Publish data to circular queue.
   *
   * The method writes the data in the given span object onto the circular
   * queue.
   *
   * @param span Constant reference to the span.
   * @param max_attempt Maximum number of attempts to perform.
   * @returns Status of the queue.
   */
  Status Publish(const Span<T> &span, size_t max_attempt = defaultMaxAttempt());

  /**
   * @brief Consume data from circular queue.
   *
   * The method sets the passed span object such that it can be used to read
   * data from the circular queue.
   *
   * @param span Reference to the read span.
   * @param max_attempt Maximum number of attempts to perform.
   * @returns Status of the queue.
   */
  Status Consume(ReadSpan &span,
                 size_t max_attempt = defaultMaxAttempt()) const;

  /**
   * @brief Get raw data in the circular queue. Only intended for testing and
   * debugging.
   *
   */
  const unsigned char *Data() const;

 private:
  circular_queue::Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>
      allocator_;
};

// ----------------------------
// CircularQueue Implementation
// ----------------------------

template <class T, size_t BUFFER_SIZE, size_t MAX_PRODUCERS,
          size_t MAX_CONSUMERS>
CircularQueue<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::CircularQueue(
    const uint64_t timeout_ns, const uint32_t start_marker)
    : allocator_(timeout_ns, start_marker) {}

template <class T, size_t BUFFER_SIZE, size_t MAX_PRODUCERS,
          size_t MAX_CONSUMERS>
typename CircularQueue<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Status
CircularQueue<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Publish(
    const Span<T> &span, size_t max_attempt) {
  // Attempt writing of data
  while (max_attempt) {
    // Allocate a write block on the buffer
    auto block_h = std::move(allocator_.Allocate(span.Size(), max_attempt));
    if (block_h) {
      memcpy(block_h.Data(), span.Data(), span.Size() * sizeof(T));
      return Status::OK;
    }
    // Could not allocate chunk so attempt again
    --max_attempt;
  }
  return Status::FULL;
}

template <class T, size_t BUFFER_SIZE, size_t MAX_PRODUCERS,
          size_t MAX_CONSUMERS>
typename CircularQueue<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Status
CircularQueue<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Consume(
    ReadSpan &span, size_t max_attempt) const {
  // Attempt reading of data
  while (max_attempt) {
    // Allocate a read block on the buffer
    span = std::move(allocator_.Allocate(max_attempt));
    if (span) {
      return Status::OK;
    }
    // Could not allocate chunk so attempt again
    --max_attempt;
  }
  return Status::EMPTY;
}

template <class T, size_t BUFFER_SIZE, size_t MAX_PRODUCERS,
          size_t MAX_CONSUMERS>
const unsigned char *
CircularQueue<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Data() const {
  return allocator_.Data();
}

// -------------------------

}  // namespace details
}  // namespace inspector
