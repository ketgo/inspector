"""
 Copyright 2022 Ketan Goyal
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
     http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
"""

import logging

from inspector import DebugArg
from inspector import testing


def test_trace_event():
    event = testing.get_test_trace_event()
    assert event.type() == 1
    assert event.counter() == 2
    assert event.pid() == 1
    assert event.tid() == 1
    assert event.timestamp_ns() == 1000
    assert event.name() == "test-event"
    debug_args = event.debug_args()
    expected = [
        (DebugArg.TYPE_DOUBLE, 3.5),
        (DebugArg.TYPE_INT64, -1),
        (DebugArg.TYPE_STRING, "test-data"),
    ]
    assert debug_args.size() == len(expected)
    idx = 0
    for arg in debug_args:
        assert arg.type() == expected[idx][0]
        assert arg.value() == expected[idx][1]
        idx += 1
