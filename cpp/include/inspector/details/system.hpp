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

/**
 * @brief The header file contains all methods to interact with the OS.
 * Currently only POSIX complient OS are supported.
 *
 */

#pragma once

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef __APPLE__
#include <sys/syscall.h>
#endif

namespace inspector {
namespace details {

/**
 * @brief Get the OS unique idenfier of the process calling the method.
 *
 */
inline int32_t GetProcessId() { return getpid(); }

/**
 * @brief Get the OS unique idenfier of the thread calling the method.
 *
 */
inline int32_t GetThreadId() {
#ifdef __APPLE__
  return syscall(SYS_thread_selfid);
#else
  return gettid();
#endif
}

}  // namespace details
}  // namespace inspector
