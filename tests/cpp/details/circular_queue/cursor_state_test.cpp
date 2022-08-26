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

#include <gtest/gtest.h>

#include <inspector/details/circular_queue/cursor_state.hpp>

using namespace inspector::details;

namespace {
using CursorState = circular_queue::CursorState;
}

TEST(CursorStateTestFixture, TestEqualityAndInequality) {
  CursorState state_a(false, 4802), state_b(false, 4802), state_c(true, 5028);

  ASSERT_EQ(state_a, state_b);
  ASSERT_NE(state_a, state_c);
}