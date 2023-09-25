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

#include <inspector/event_.hpp>

namespace inspector {

uint8_t Event::type() const { return type_; }

uint8_t Event::category() const { return category_; }

int64_t Event::timestamp() const { return timestamp_; }

int32_t Event::pid() const { return pid_; }

int32_t Event::tid() const { return tid_; }

uint32_t Event::argsCount() const { return args_count_; }

}  // namespace inspector