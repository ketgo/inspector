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

namespace inspector {

/**
 * @brief Data struct containing keyword arguments and values to be stored as
 * part of a trace scope.
 *
 * @tparam T Type of value.
 */
template <class T>
struct KwArgs {
  const char* name;  //<- Name of argument.
  const T& value;    //<- Value of argument.
};

}  // namespace inspector