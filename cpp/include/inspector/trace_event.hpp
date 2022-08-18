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

#include <inspector/details/system.hpp>

namespace inspector {

/**
 * @brief The class `details::TraceEvent` is used to create a trace event in the
 * format written on the event queue. All events are stored in the catapult
 * format. See the following link for more details:
 *
 * https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU
 *
 */
class TraceEvent {
  // Delimiter used to separate arguments which make up the trace event.
  static constexpr auto delimiter_ = '|';

 public:
  /**
   * @brief Parse trace event from the given string.
   *
   */
  static TraceEvent Parse(const std::string& data);

  /**
   * @brief Construct a new TraceEvent object
   *
   * @param type Constant reference to the trace event type.
   * @param name Constant reference to the event name.
   */
  TraceEvent(const char type, const std::string& name);

  int64_t Timestamp() const;
  int32_t ProcessId() const;
  int32_t ThreadId() const;
  char Type() const;
  const std::string& Name() const;
  std::string Payload() const;

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
   * @brief Construct a new Trace Event object.
   *
   */
  TraceEvent() = default;

  int64_t timestamp_;
  int32_t pid_;
  int32_t tid_;
  char type_;
  std::string name_;
  std::stringstream payload_;
};

// -------------------------------------------
// TraceEvent Implementation
// -------------------------------------------

// static
inline TraceEvent TraceEvent::Parse(const std::string& event_str) {
  TraceEvent event;
  char delimiter;
  std::istringstream stream(event_str);
  stream >> event.timestamp_ >> delimiter >> event.pid_ >> delimiter >>
      event.tid_ >> delimiter >> event.type_ >> delimiter >> event.name_;
  auto pos = event.name_.find(delimiter_);
  if (pos != std::string::npos) {
    event.payload_ << event.name_.substr(pos + sizeof(delimiter_));
    event.name_ = event.name_.substr(0, pos);
  }
  return event;
}

inline TraceEvent::TraceEvent(const char type, const std::string& name)
    : timestamp_(
          std::chrono::high_resolution_clock::now().time_since_epoch().count()),
      pid_(details::GetProcessId()),
      tid_(details::GetThreadId()),
      type_(type),
      name_(name),
      payload_() {}

int64_t TraceEvent::Timestamp() const { return timestamp_; }

int32_t TraceEvent::ProcessId() const { return pid_; }

int32_t TraceEvent::ThreadId() const { return tid_; }

char TraceEvent::Type() const { return type_; }

const std::string& TraceEvent::Name() const { return name_; }

std::string TraceEvent::Payload() const { return payload_.str(); }

inline void TraceEvent::SetArgs() {}

template <class T, class... Args>
inline void TraceEvent::SetArgs(const T& arg, const Args&... args) {
  payload_ << delimiter_ << arg;
  SetArgs(args...);
}

inline std::string TraceEvent::String() const {
  std::stringstream stream;
  stream << timestamp_ << delimiter_ << pid_ << delimiter_ << tid_ << delimiter_
         << type_ << delimiter_ << name_ << payload_.str();
  return stream.str();
}

}  // namespace inspector