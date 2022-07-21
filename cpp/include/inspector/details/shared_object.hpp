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

#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */

#include <cerrno>
#include <string>
#include <system_error>
#include <type_traits>

namespace inspector {
namespace details {
namespace shared_object {

/**
 * @brief Create a shared object of type T with given globally unique name and
 * CTOR parameters.
 *
 * @tparam T The type of shared object.
 * @tparam Args Constructor argument types.
 * @param name Constant reference to the system unique name of the shared
 * object.
 * @param args Rvalue reference to the constructor arguments of the shared
 * object.
 * @returns Pointer to the shared object.
 */
template <class T, class... Args>
T* Create(const std::string& name, Args&&... args) {
  static_assert(std::is_trivial<T>::value,
                "The data type used does not have a trivial memory layout.");
  auto size = sizeof(T);

  auto fd =
      shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    throw std::system_error(errno, std::generic_category(), "shm_open");
  }
  if (ftruncate(fd, size) == -1) {
    throw std::system_error(errno, std::generic_category(), "ftruncate");
  }
  auto addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    throw std::system_error(errno, std::generic_category(), "mmap");
  }
  close(fd);
  return new (addr) T(std::forward<Args>(args)...);
}

/**
 * @brief Create a shared object of type T with given globally unique name.
 *
 * @tparam T The type of shared object.
 * @param name Constant reference to the system unique name of the shared
 * object.
 * @returns Pointer to the shared object.
 */
template <class T>
T* Get(const std::string& name) {
  static_assert(std::is_trivial<T>::value,
                "The data type used does not have a trivial memory layout.");
  auto size = sizeof(T);

  auto fd = shm_open(name.c_str(), O_RDWR, S_IRUSR | S_IWUSR);
  if (fd == -1) {
    throw std::system_error(errno, std::generic_category(), "shm_open");
  }
  auto addr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (addr == MAP_FAILED) {
    throw std::system_error(errno, std::generic_category(), "mmap");
  }
  close(fd);
  return reinterpret_cast<T*>(addr);
}

/**
 * @brief Get or create a shared object of type T with given globally unique
 * name and CTOR parameters.
 *
 * The method gets an instance of a shared object of type T. If such an object
 * does not exist then a new object is created using the passed constructor
 * arguments.
 *
 * @tparam T The type of shared object.
 * @tparam Args Constructor argument types.
 * @param name Constant reference to the system unique name of the shared
 * object.
 * @param args Rvalue reference to the constructor arguments of the shared
 * object. Note that these value will not get used if the object already exists.
 * @returns Pointer to the shared object.
 */
template <class T, class... Args>
T* GetOrCreate(const std::string& name, Args&&... args) {
  T* rvalue;
  try {
    rvalue = Create<T>(name, std::forward<Args>(args)...);
  } catch (const std::system_error& error) {
    if (error.code().value() == EEXIST) {
      rvalue = Get<T>(name);
    }
  }
  return rvalue;
}

/**
 * @brief Mark the shared object with given system unique name for removal. The
 * object will get removed by the OS once no more references created by other
 * processes exist.
 *
 * @param name Constant reference to the system unique name of the shared
 * object.
 * @returns `true` on success else `false`.
 */
inline bool Remove(const std::string& name) {
  return shm_unlink(name.c_str()) < 0 ? false : true;
}

}  // namespace shared_object
}  // namespace details
}  // namespace inspector
