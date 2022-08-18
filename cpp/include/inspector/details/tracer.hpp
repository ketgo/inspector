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

#include <chrono>
#include <sstream>

#include <inspector/writer.hpp>
#include <inspector/details/system.hpp>

// TODO: Need a parser for reading trace events. Thus split the `TraceEvent`
// class into a serializer and a de-serializer.

namespace inspector {
namespace details {

/**
 * @brief The class `Kwarg` represents a keyword argument.
 *
 * @tparam T The type of argument.
 */
template <class T>
class Kwarg {
 public:
  /**
   * @brief Construct a new keyword argument.
   *
   * @param name Name of the argument.
   * @param value Constant reference to the argument value.
   */
  Kwarg(const char* name, const T& value) : name_(name), value_(value) {}

  /**
   * @brief Write keyword argument to output stream.
   *
   * @param out Reference to the output stream.
   * @param arg Constant reference to the keyword argument.
   */
  friend std::ostream& operator<<(std::ostream& out, const Kwarg& arg) {
    out << arg.name_ << "=" << arg.value_;
    return out;
  }

 private:
  const char* name_;
  const T& value_;
};

// -------------------------------------------

/**
 * @brief The class `TraceEvent` is used to create a trace event to be written
 * on the event queue. A trace event object is automatically written to the
 * event queue during DTOR.
 *
 */
class TraceEvent {
  // Delimiter used to separate arguments which make up the trace event.
  static constexpr auto delimiter_ = '|';

 public:
  /**
   * @brief Construct a new TraceEvent object
   *
   * @param type Constant reference to the trace event type.
   * @param name Constant reference to the event name.
   */
  TraceEvent(const char type, const std::string& name);

  /**
   * @brief Destroy the TraceEvent object.
   *
   */
  ~TraceEvent();

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
   * @brief Get string representation of the trace event.
   *
   */
  std::string String() const;

 private:
  /**
   * @brief Get writer for publishing trace events.
   *
   * @returns Reference to the event writer.
   */
  static Writer& TraceWriter();

  std::stringstream stream_;
};

// -------------------------------------------
// TraceEvent Implementation
// -------------------------------------------

// static
inline Writer& TraceEvent::TraceWriter() {
  static Writer writer;
  return writer;
}

// ---------------- public -------------------

inline TraceEvent::TraceEvent(const char type, const std::string& name)
    : stream_() {
  auto timestamp =
      std::chrono::high_resolution_clock::now().time_since_epoch().count();
  auto pid = GetProcessId();
  auto tid = GetThreadId();
  stream_ << timestamp << delimiter_ << pid << delimiter_ << tid << delimiter_
          << type << delimiter_ << name;
}

inline TraceEvent::~TraceEvent() { TraceWriter().Write(stream_.str()); }

inline void TraceEvent::SetArgs() {}

template <class T, class... Args>
inline void TraceEvent::SetArgs(const T& arg, const Args&... args) {
  stream_ << delimiter_ << arg;
  SetArgs(args...);
}

inline std::string TraceEvent::String() const { return stream_.str(); }

// -------------------------------------------

}  // namespace details
}  // namespace inspector