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

#include <string>

namespace inspector {

/**
 * @brief Generic trace or metric event published by the inspector library.
 *
 */
class Event {
 public:
  static Event Parse(const char* start, const std::size_t size);

  const int8_t& Type() const;
  const int64_t& Timestamp() const;
  const int32_t& Pid() const;
  const int32_t& Tid() const;
  const std::string& Payload() const;

 protected:
  int8_t type_;
  int64_t timestamp_;
  int32_t pid_;
  int32_t tid_;
  std::string payload_;
};

}  // namespace inspector