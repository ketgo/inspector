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
  static_assert(BUFFER_SIZE > sizeof(MemoryBlock<T>),
                "Invalid buffer size specified.");

 public:
  /**
   * @brief Construct a new Allocator object.
   *
   * @param timeout_ns Cursor timeout in nano seconds.
   * @param start_marker Start marker used to split memory blocks. It is
   * primarly used to recover memory block size for stale blocks written by
   * abruptly terminated producers. Please use a value which is never going to
   * be a part of the actual message content.
   */
  Allocator(const uint64_t timeout_ns, const uint32_t start_marker);

  /**
   * @brief Allocate memory in the circular queue for writing.
   *
   * @param size Number of objects to write.
   * @param max_attempt The maximum number of attempts.
   * @returns Pointer to the memory block.
   */
  MemoryBlockHandle<T, BUFFER_SIZE> Allocate(const std::size_t size,
                                             std::size_t max_attempt);

  /**
   * @brief Allocate memory in the circular queue for reading.
   *
   * @param max_attempt The maximum number of attempts.
   * @returns Pointer to the memory block.
   */
  MemoryBlockHandle<const T, BUFFER_SIZE> Allocate(
      std::size_t max_attempt) const;

  /**
   * @brief Get the raw buffer data.
   *
   */
  const unsigned char *Data() const;

 private:
  /**
   * @brief Get read-write memory block at the given cursor location on the
   * buffer.
   *
   * @param cursor Constant reference to the cursor.
   * @returns Pointer to the memory block.
   */
  MemoryBlock<T> *GetMemoryBlock(const Cursor<BUFFER_SIZE> &cursor);

  /**
   * @brief Get read-only memory block at the given cursor location on the
   * buffer.
   *
   * @param cursor Constant reference to the cursor.
   * @returns Pointer to the memory block.
   */
  MemoryBlock<const T> *GetMemoryBlock(const Cursor<BUFFER_SIZE> &cursor) const;

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
      const Cursor<BUFFER_SIZE> &start, const Cursor<BUFFER_SIZE> &end) const;

  unsigned char data_[BUFFER_SIZE];                    // data buffer
  CursorPool<MAX_PRODUCERS, BUFFER_SIZE> write_pool_;  // write cursor pool
  AtomicCursor<BUFFER_SIZE> write_head_;               // write head cursor
  mutable CursorPool<MAX_CONSUMERS, BUFFER_SIZE>
      read_pool_;                                // read cursor pool
  mutable AtomicCursor<BUFFER_SIZE> read_head_;  // read head cursor
  const uint32_t start_marker_;                  // start marker
};

// ----------------------------
// Allocator Implementation
// ----------------------------

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
MemoryBlock<T>
    *Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::GetMemoryBlock(
        const Cursor<BUFFER_SIZE> &cursor) {
  return reinterpret_cast<MemoryBlock<T> *>(
      &data_[cursor.Location() % BUFFER_SIZE]);
}

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
MemoryBlock<T const>
    *Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::GetMemoryBlock(
        const Cursor<BUFFER_SIZE> &cursor) const {
  return reinterpret_cast<MemoryBlock<const T> *>(
      const_cast<unsigned char *>(&data_[cursor.Location() % BUFFER_SIZE]));
}

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
std::pair<std::size_t, bool>
Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::
    GetOrRecoverMemoryBlockSize(const Cursor<BUFFER_SIZE> &start,
                                const Cursor<BUFFER_SIZE> &end) const {
  auto *block = GetMemoryBlock(start);
  // Check if the memory block header contains a valid size by checking for the
  // existance of the start marker
  if (block->start_marker == start_marker_) {
    // Start marker present so return the memory block size.
    // @note We need to implicitly create a pair and set the size since
    // the memory block is a packed data structure.
    std::pair<std::size_t, bool> size;
    size.first = block->size;
    size.second = false;
    return size;
  }

  // Start marker not present in the header so attempt a recover by searching
  // for the next start marker
  std::size_t size = 0;
  Cursor<BUFFER_SIZE> cursor = start + sizeof(MemoryBlock<T>);
  while (cursor < end) {
    block = GetMemoryBlock(cursor);
    if (block->start_marker == start_marker_) {
      // Found possible header of the next block so validate the header before
      // returning the computed block size
      auto next = cursor + sizeof(T) * block->size;
      if (next == end) {
        return {size, true};
      }
      //@note We dont combine the two if statements into one since the cursor
      //`next` might be pointing to the buffer end and thus would result in
      // invalid memory access if cast to a memory block.
      auto *next_block = GetMemoryBlock(next);
      if (next_block->start_marker == start_marker_) {
        return {size, true};
      }
      // Flase marker found so continue the search
    }
    cursor = cursor + sizeof(T);
    ++size;
  }

  // Failed to recover the block size so return 0
  return {0, true};
}

// --------- public ----------

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
Allocator<T, BUFFER_SIZE, MAX_PRODUCERS, MAX_CONSUMERS>::Allocator(
    const uint64_t timeout_ns, const uint32_t start_marker)
    : write_pool_(timeout_ns),
      read_pool_(timeout_ns),
      start_marker_(start_marker) {
  Cursor<BUFFER_SIZE> cursor(false, 0);
  write_head_.store(cursor, std::memory_order_seq_cst);
  read_head_.store(cursor, std::memory_order_seq_cst);
}

template <class T, std::size_t BUFFER_SIZE, std::size_t MAX_PRODUCERS,
          std::size_t MAX_CONSUMERS>
MemoryBlockHandle<T, BUFFER_SIZE>
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
        auto *block = GetMemoryBlock(write_head);
        block->size = size;
        block->start_marker = start_marker_;
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
MemoryBlockHandle<const T, BUFFER_SIZE>
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
        // If the block size was recovered then move to the next block
        if (size.second) {
          continue;
        }
        auto *block = GetMemoryBlock(read_head);
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
