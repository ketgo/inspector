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

#include <inspector/details/event_header.hpp>
#include <inspector/details/system.hpp>

namespace inspector {
namespace details {

/**
 * @brief The class `TraceEvent` is used to create user trace events in the
 * catapult format. See the following link for more details:
 *
 * https://docs.google.com/document/d/1CvAClvFfyA5R-PhYUmn5OOQtYMH4h6I0nSsKchNAySU
 *
 */
class TraceEvent {
  // Delimiter used to separate arguments which make up the trace event.
  static constexpr auto delimiter_ = '|';

 public:
  /**
   * @brief Trace event identifier.
   *
   */
  static constexpr int Id = 0;

  /**
   * @brief Construct a new TraceEvent object.
   *
   * @param phase Phase of the event.
   * @param name Constant reference to the event name.
   */
  TraceEvent(const char phase, const std::string& name);

  /**
   * @brief Get string representation of the trace event.
   *
   */
  std::string String() const;

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

 private:
  std::stringstream payload_;
};

// -------------------------------------------
// TraceEvent Implementation
// -------------------------------------------

inline TraceEvent::TraceEvent(const char phase, const std::string& name)
    : payload_() {
  EventHeader header;
  header.type = Id;
  header.timestamp =
      std::chrono::system_clock::now().time_since_epoch().count();
  header.pid = details::system::GetProcessId();
  header.tid = details::system::GetThreadId();
  payload_ << std::string(reinterpret_cast<char*>(&header), sizeof(EventHeader))
           << phase << delimiter_ << name;
}

inline void TraceEvent::SetArgs() {}

template <class T, class... Args>
inline void TraceEvent::SetArgs(const T& arg, const Args&... args) {
  payload_ << delimiter_ << arg;
  SetArgs(args...);
}

inline std::string TraceEvent::String() const { return payload_.str(); }

// -------------------------------------------

}  // namespace details
}  // namespace inspector