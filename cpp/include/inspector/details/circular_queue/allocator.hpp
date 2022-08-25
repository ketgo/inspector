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

#include <inspector/details/circular_queue/block_handle.hpp>
#include <inspector/details/circular_queue/cursor_pool.hpp>

namespace inspector {
namespace details {
namespace circular_queue {

/**
 * @brief Allocator to allocate read and write memory blocks in a circular
 * queue.
 *
 * @tparam T The type of object stored in circular queue.
 * @tparam BUFFER_SIZE The size of memory buffer in bytes.
 * @tparam MAX_PRODUCERS The maximum number of producers.
 * @tparam MAX_CONSUMERS The maximum number of consumers.
 */
template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
class Allocator {
 public:
  /**
   * @brief Construct a new Allocator object.
   *
   * @param timeout_ns Cursor timeout in nano seconds.
   */
  Allocator(const uint64_t timeout_ns);

  /**
   * @brief Allocate memory in the circular queue for writing.
   *
   * @param size Number of objects to write.
   * @param max_attempt The maximum number of attempts.
   * @returns Pointer to the memory block.
   */
  MemoryBlockHandle<T> Allocate(const std::size_t size,
                                std::size_t max_attempt);

  /**
   * @brief Allocate memory in the circular queue for reading.
   *
   * @param max_attempt The maximum number of attempts.
   * @returns Pointer to the memory block.
   */
  MemoryBlockHandle<const T> Allocate(std::size_t max_attempt) const;

  /**
   * @brief Get the raw buffer data.
   *
   */
  const unsigned char *Data() const;

 private:
  /**
   * @brief Get or recover the block size of the memory block located at the
   * given cursor.
   *
   * The method checks for the presence of the magic mark in the memory block.
   * If no such mark is present then the block size is recovered by computing
   * the distance to the next magic mark on the buffer. In case no such mark is
   * present then the method returns a size of 0.
   *
   * @param start Constant reference to the cursor pointing to the start of the
   * memory block.
   * @param end Constant reference to the cursor containing the max possible end
   * location of the memory block.
   * @returns Tuple containing the block size and a flag indicating if the size
   * was recovered.
   */
  std::pair<std::size_t, bool> GetOrRecoverMemoryBlockSize(
      const Cursor &start, const Cursor &end) const;

  unsigned char data_[BUFFER_SIZE];              // data buffer
  CursorPool<MAX_PRODUCERS> write_pool_;         // write cursor pool
  AtomicCursor write_head_;                      // write head cursor
  mutable CursorPool<MAX_CONSUMERS> read_pool_;  // read cursor pool
  mutable AtomicCursor read_head_;               // read head cursor
};

// ----------------------------
// Allocator Implementation
// ----------------------------

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
std::pair<std::size_t, bool>
Allocator<T, BUFFER_SIZE, MAX_PRODUCERS,
          MAX_CONSUMERS>::GetOrRecoverMemoryBlockSize(const Cursor &start,
                                                      const Cursor &end) const {
  uint64_t location = start.Location();
  auto *block = reinterpret_cast<MemoryBlock<const T> *>(
      const_cast<unsigned char *>(&data_[location % BUFFER_SIZE]));
  // Check if the memeory block header contains a valid size
  if (block->magic_mark != kMagicMark) {
    // The header does not contain a valid size so attempt recovery
    std::size_t delta = sizeof(MemoryBlock<T>);
    location += delta;
    while (location < end.Location()) {
      location = start.Location() + delta;
      auto *header = reinterpret_cast<MemoryBlock<const T> *>(
          const_cast<unsigned char *>(&data_[location % BUFFER_SIZE]));
      if (header->magic_mark == kMagicMark) {
        // Found the header of the next block so return the computed block size
        return {delta / sizeof(T), true};
      }
      ++delta;
    }
    // Failed to recover the block size so return 0
    return {0, true};
  }
  // Magic mark present in the memory block header so return the valid size.
  // @note We need to specifically create a pair and set the size since the
  // block is a packed data structure.
  std::pair<std::size_t, bool> size;
  size.first = block->size;
  size.second = false;
  return size;
}

// --------- public ----------

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Allocator(
    const uint64_t timeout_ns)
    : write_pool_(timeout_ns), read_pool_(timeout_ns) {
  Cursor cursor(false, 0);
  write_head_.store(cursor, std::memory_order_seq_cst);
  read_head_.store(cursor, std::memory_order_seq_cst);
}

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
MemoryBlockHandle<T>
Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Allocate(
    const std::size_t size, std::size_t max_attempt) {
  // Get the size of memory block to allocate for writing
  auto block_size = sizeof(MemoryBlock<T>) + size * sizeof(T);
  assert(block_size < BUFFER_SIZE);
  // Attempt to get a write cursor from the cursor pool
  auto cursor_h = write_pool_.Allocate(max_attempt);
  if (!cursor_h) {
    return {};
  }
  // Attempt to allocate the requested size of space in the buffer for writing.
  while (max_attempt) {
    auto read_head = read_head_.load(std::memory_order_seq_cst);
    auto write_head = write_head_.load(std::memory_order_seq_cst);
    auto end = write_head + (block_size - 1);
    // Allocate block only if the end location is ahead of all the allocated
    // read cursors and the read head, and the read head is equal or behind the
    // write head.
    if (read_head <= write_head && read_pool_.IsAhead(end) && read_head < end) {
      cursor_h->store(write_head, std::memory_order_seq_cst);
      // Set write head to new value if its original value has not already been
      // changed by another writer.
      if (write_head_.compare_exchange_weak(write_head, end + 1)) {
        auto *block = reinterpret_cast<MemoryBlock<T> *>(
            &data_[write_head.Location() % BUFFER_SIZE]);
        block->size = size;
        block->magic_mark = kMagicMark;
        return {*block, std::move(cursor_h)};
      }
      // Another writer allocated memory before us so try again until success
      // or max attempt is reached.
    }
    // Not enough space to allocate memory so try again until enough space
    // becomes available or the max attempt is reached.
    --max_attempt;
  }
  // Could not allocate the requested space in the specified number of attempts
  return {};
}

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
MemoryBlockHandle<const T>
Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Allocate(
    std::size_t max_attempt) const {
  // Attempt to get a read cursor from the cursor pool
  auto cursor_h = read_pool_.Allocate(max_attempt);
  if (!cursor_h) {
    return {};
  }
  // Attempt to allocate the requested size of space in the buffer for reading.
  while (max_attempt) {
    auto read_head = read_head_.load(std::memory_order_seq_cst);
    auto write_head = write_head_.load(std::memory_order_seq_cst);
    // Check if the block has valid size information
    auto size = GetOrRecoverMemoryBlockSize(read_head, write_head);
    auto block_size = sizeof(MemoryBlock<T>) + size.first * sizeof(T);
    auto end = read_head + (block_size - 1);
    // Allocate block only if the end location is behind all of the allocated
    // write cursors and the write head, the read head is behind the write head,
    // and the block size is not 0.
    if (read_head < write_head && write_pool_.IsBehind(end) &&
        end < write_head && size.first != 0) {
      cursor_h->store(read_head, std::memory_order_seq_cst);
      // Set read head to new value if its original value has not already been
      // changed by another reader.
      if (read_head_.compare_exchange_weak(read_head, end + 1)) {
        auto *block = reinterpret_cast<MemoryBlock<const T> *>(
            const_cast<unsigned char *>(
                &data_[read_head.Location() % BUFFER_SIZE]));
        if (size.second) {
          block->size = size.first;
          block->magic_mark = kMagicMark;
        }
        return {*block, std::move(cursor_h)};
      }
      // Another reader allocated memory before us so try again until success
      // or max attempt is reached.
    }
    // Not enough space to allocate memory so try again until enough space
    // becomes available or the max attempt is reached.
    --max_attempt;
  }
  // Could not allocate the requested space in the specified number of attempts
  return {};
}

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
const unsigned char *
Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Data() const {
  return data_;
}

// -------------------------

}  // namespace circular_queue
}  // namespace details
}  // namespace inspector
