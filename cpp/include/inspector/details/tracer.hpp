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

#include <sys/types.h>
#include <unistd.h>
#ifdef __APPLE__
#include <sys/syscall.h>
#endif

#include <chrono>
#include <sstream>

#include <inspector/writer.hpp>

namespace inspector {
namespace details {

/**
 * @brief Get writer for publishing trace events.
 *
 * @returns Reference to the event writer.
 */
inline Writer& TraceWriter() {
  static Writer writer;
  return writer;
}

// ---------------------------------------------

/**
 * @brief The class `TraceEvent` is used to create a trace event to be written
 * on the event queue.
 *
 */
class TraceEvent {
  // Delimiter used to separate arguments which make up the trace event.
  static constexpr auto delimiter_ = '|';

 public:
  /**
   * @brief Get the process identifier.
   *
   */
  static int32_t GetProcessId();

  /**
   * @brief Get the thread identifier.
   *
   */
  static int32_t GetThreadId();

  /**
   * @brief Construct a new TraceEvent object
   *
   * @param type Constant reference to the trace event type.
   * @param name Constant reference to the event name.
   */
  TraceEvent(const char type, const std::string& name);

  /**
   * @brief Method used for recursive parameter pack expansion.
   *
   * @note No operation performed.
   */
  void SetArgs();

  /**
   * @brief Set additional arguments in the trace event.
   *
   * @tparam T Type of first argument.
   * @tparam Args Type of other arguments.
   * @param arg Constant reference to the first argument.
   * @param args Constant reference to the other arguments.
   */
  template <class T, class... Args>
  void SetArgs(const T& arg, const Args&... args);

  /**
   * @brief Keyword argument type.
   *
   * @tparam T The type of argument.
   */
  template <class T>
  using Kwarg = std::pair<const char*, const T&>;

  /**
   * @brief Utility method to create a keyword argument from the given name and
   * value.
   *
   * @tparam T The type of argument.
   * @param name Name of the argument.
   * @param value Constant reference to the value of the argument.
   * @returns Keyword argument object.
   */
  template <class T>
  static Kwarg<T> MakeKwarg(const char* name, const T& value);

  /**
   * @brief Method used for recursive parameter pack expansion.
   *
   * @note No operation performed.
   */
  void SetKwargs();

  /**
   * @brief Set additional keyword arguments in the trace event.
   *
   * @tparam T Type of first argument.
   * @tparam Args Type of other arguments.
   * @param arg Constant reference to the first keyword argument.
   * @param args Constant reference to the other keyword arguments.
   */
  template <class T, class... Args>
  void SetKwargs(const Kwarg<T>& arg, const Kwarg<Args>&... args);

  /**
   * @brief Get string representation of the trace event.
   *
   */
  std::string String() const;

 private:
  std::stringstream stream_;
};

// -------------------------------------------
// TraceEvent Implementation
// -------------------------------------------

// static
inline int32_t TraceEvent::GetProcessId() { return getpid(); }

// static
inline int32_t TraceEvent::GetThreadId() {
#ifdef __APPLE__
  return syscall(SYS_thread_selfid);
#else
  return gettid();
#endif
}

inline TraceEvent::TraceEvent(const char type, const std::string& name)
    : stream_() {
  auto timestamp =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  auto pid = GetProcessId();
  auto tid = GetThreadId();
  stream_ << timestamp << delimiter_ << pid << delimiter_ << tid << delimiter_
          << type << delimiter_ << name;
}

inline void TraceEvent::SetArgs() {}

template <class T, class... Args>
void TraceEvent::SetArgs(const T& arg, const Args&... args) {
  stream_ << delimiter_ << arg;
  SetArgs(args...);
}

// static
template <class T>
typename TraceEvent::Kwarg<T> TraceEvent::MakeKwarg(const char* name,
                                                    const T& value) {
  return {name, value};
}

inline void TraceEvent::SetKwargs() {}

template <class T, class... Args>
void TraceEvent::SetKwargs(const Kwarg<T>& arg, const Kwarg<Args>&... args) {
  stream_ << delimiter_ << arg.first << "=" << arg.second;
  SetKwargs(args...);
}

inline std::string TraceEvent::String() const { return stream_.str(); }

// -------------------------------------------

}  // namespace details
}  // namespace inspector