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
  // Delimiter used to seprate arguments which make up the trace event.
  static constexpr auto delimiter_ = "|";

 public:
  /**
   * @brief Construct a new TraceEvent object
   *
   * @tparam Args Type of arguments to attach to the event.
   * @param args Constant reference to the argument values.
   */
  template <class... Args>
  TraceEvent(const Args&... args);

  /**
   * @brief Get string representation of the trace event.
   *
   */
  std::string String() const;

 private:
  /**
   * @brief Append given argument to the trace event.
   *
   * @tparam T Type of argument.
   * @param arg Constant reference to the argument.
   */
  template <class T>
  void Append(const T& arg);

  /**
   * @brief Append given arguments to the trace event.
   *
   * @tparam T Type of argument.
   * @tparam Args Type of arguments.
   * @param arg Constant reference to the first argument.
   * @param args COnstant reference to the reset of the arguments.
   */
  template <class T, class... Args>
  void Append(const T& arg, const Args&... args);

  std::stringstream stream_;
};

// -------------------------------------------
// TraceEvent Implementation
// -------------------------------------------

template <class T>
void TraceEvent::Append(const T& arg) {
  stream_ << delimiter_ << arg;
}

template <class T, class... Args>
void TraceEvent::Append(const T& arg, const Args&... args) {
  stream_ << delimiter_ << arg;
  Append(args...);
}

// ------------ public -----------------------

template <class... Args>
TraceEvent::TraceEvent(const Args&... args) : stream_() {
  auto timestamp =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  auto pid = getpid();
  auto tid = gettid();
  stream_ << timestamp << delimiter_ << pid << delimiter_ << tid;
  Append(args...);
}

std::string TraceEvent::String() const { return stream_.str(); }

// -------------------------------------------

}  // namespace details
}  // namespace inspector