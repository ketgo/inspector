/**
 * Copyright 2023 Ketan Goyal
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

#include <cstdint>
#include <cstddef>

#include <inspector/details/serializer.hpp>

#define PACKED __attribute__((packed))

namespace inspector {
namespace details {
namespace Event {

/**
 * @brief The data structure `Header` contains all the header information
 * of a single captured trace or metric event using the inspector library.
 *
 */
struct PACKED Header {
  uint8_t type;       //<- Type of event. This field is used to distinguish the
                      // different events.
  uint8_t category;   //<- Category of event.
  uint64_t counter;   //<- Counter used to track for lost events.
  int64_t timestamp;  //<- Timestamp in nano seconds when the event occurred.
  int32_t pid;  //<- Identifier of the process in which the event occurred or
                // is observed.
  int32_t tid;  //<- Identifier of the thread in which the event occurred or is
                // observed.
};

/**
 * @brief Get the size in bytes required to storage an event.
 *
 * @tparam Args Argument types to store as part of the event.
 * @param args Constant reference to the arguments to storage as part of the
 * event.
 * @returns Size in bytes.
 */
template <class... Args>
size_t storageSize(const Args&... args) {
  return sizeof(Header) + sizeof(uint8_t) + storageSize(args...);
}

/**
 * @brief The class `MutableView` exposes a mutable view of an event on a
 * given memory buffer.
 *
 */
class MutableView {
 public:
  /**
   * @brief Construct a new MutableEventView object.
   *
   * @param address Pointer to starting address of memory buffer.
   */
  explicit MutableView(void* const address) : address_(address) {}

  /**
   * @brief Get header of the event.
   *
   * @returns Reference to the event header.
   */
  Header& header() const { return *static_cast<Header*>(address_); }

  /**
   * @brief Add the given arguments to the event.
   *
   * @tparam Args Argument types to store as part of the event.
   * @param args Constant reference to the arguments to storage as part of the
   * event.
   */
  template <class... Args>
  void addArgs(const Args&... args) {
    *static_cast<uint8_t*>(address_ + sizeof(Header)) = sizeof...(Args);
    dump(address_ + sizeof(Header) + sizeof(uint8_t), args...);
  }

 private:
  void* const address_;
};

class ConstantView {
 public:
  explicit ConstantView(void* const address) : address_(address) {}

  /**
   * @brief Get header of the event.
   *
   * @returns Constant reference to the event header.
   */
  const Header& header() const { return *static_cast<Header*>(address_); }

  size_t argsCount() const {
    return *static_cast<uint8_t*>(address_ + sizeof(Header));
  }

  class ArgIterator {
    friend class ConstantView;

   public:
    ArgIterator() : address_(nullptr), count_(0) {}

    DataType dataType() const { return details::dataType(address_); }

    template <class T>
    const T& get() const {
      return details::load<T>(address_);
    }

    ArgIterator& operator++() {
      if (count_ == 0) {
        return *this;
      }

      return *this;
    }

    ArgIterator operator++(int) const {
      ArgIterator it(address_, count_);
      return ++it;
    }

    bool operator==(const ArgIterator& other) const {
      return address_ == other.address_ && count_ == other.count_;
    }

    bool operator!=(const ArgIterator& other) const {
      return !(*this == other);
    }

   private:
    ArgIterator(void* const address, const size_t count)
        : address_(address), count_(count) {}

    void* address_;
    size_t count_;
  };

 private:
  void* const address_;
};

}  // namespace Event
}  // namespace details
}  // namespace inspector