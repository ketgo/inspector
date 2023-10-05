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
#include <utility>

namespace inspector {

/**
 * @brief Enumerated list of data types having serialization support.
 *
 */
enum class DataType {
  TYPE_INT16 = 0,
  TYPE_INT32,
  TYPE_INT64,
  TYPE_UINT8,
  TYPE_UINT16,
  TYPE_UINT32,
  TYPE_UINT64,
  TYPE_FLOAT,
  TYPE_DOUBLE,
  TYPE_CHAR,
  TYPE_CSTRING,
  TYPE_STRING,
};

namespace details {

/**
 * @brief Dummy method to always return 0. The method added to facilitate
 * parameter pack expansion.
 *
 */
inline size_t storageSize() { return 0; }

/**
 * @brief Get the storage size required to store the given object.
 *
 * @tparam T Type of object.
 * @param obj Constant reference to the object to store.
 * @returns Storage size in bytes.
 */
template <class T>
size_t storageSize(const T& obj);

/**
 * @brief Get the storage size required to store the given objects.
 *
 * @tparam T Type of first object.
 * @tparam Args Type of other objects.
 * @param obj Constant reference to the first object.
 * @param args Constant reference to other objects.
 * @return Storage size in bytes.
 */
template <class T, class... Args>
size_t storageSize(const T& obj, const Args&... args) {
  return storageSize(obj) + storageSize(args...);
}

/**
 * @brief Get the size in bytes used to store a given data type.
 *
 * @param data_type Type of data.
 * @returns Size in bytes.
 */
size_t storageSizeOf(const DataType data_type);

/**
 * @brief Dummy method which perform NOP. The method added to facilitate
 * parameter pack expansion.
 *
 */
inline void dump(void* address) {}

/**
 * @brief Dump the given object to the given memory address.
 *
 * @tparam T Type of object to dump.
 * @param address Starting address to dump object.
 * @param obj Constant reference to the object.
 */
template <class T>
void dump(void* address, const T& obj);

/**
 * @brief Dump the given objects to the given memory address.
 *
 * @tparam T Type of first object to dump.
 * @tparam Args Type of other objects to dump.
 * @param address Starting address to dump object.
 * @param obj Constant reference to the first object.
 * @param args Constant reference to the other objects.
 */
template <class T, class... Args>
void dump(void* address, const T& obj, const Args&... args) {
  dump(address, obj);
  dump(static_cast<void*>(static_cast<uint8_t*>(address) + storageSize(obj)),
       args...);
}

/**
 * @brief Get the type of data stored at the given memory address.
 *
 * @param address Constant pointer to the memory address.
 * @returns Type of data stored.
 *
 * @warning This method does not perform any validation on the correctness of
 * the given memory address and hence should be used carefully.
 */
DataType dataType(const void* address);

/**
 * @brief Load an object of type T stored at the given memory address.
 *
 * @tparam T Type of object to load.
 * @param address  Constant pointer to the memory address.
 * @returns Loaded object of type T.
 * @throws Invalid type exception as std::runtime_error.
 *
 * @warning The memory address should be valid otherwise a segfault can occur.
 */
template <class T>
T load(const void* address);

}  // namespace details
}  // namespace inspector