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

from typing import List, Any

import pytest
import inspector


def has_debug_args(event: inspector.TraceEvent, args: List[Any]) -> bool:
    """
    Utility method to check if the given trace event `event` has the debug
    arguments specified in `args`.
    """
    debug_args = event.debug_args()
    if len(args) != len(debug_args):
        return False
    idx = 0
    for arg in debug_args:
        if arg.value() != args[idx]:
            return False
        idx += 1

    return True


def setup_module():
    inspector.Config.set_event_queue_name("inspector-trace-test")


def teardown_modlue():
    inspector.testing.remove_event_queue()


def setup_function():
    inspector.testing.empty_event_queue()


@pytest.mark.parametrize(
    "func,type,name,debug_args,invalid_debug_args",
    [
        (
            inspector.sync_begin,
            inspector.EventType.kSyncBeginTag,
            "test_sync",
            [1, "one"],
            [1, "one", []],
        ),
        (
            inspector.sync_end,
            inspector.EventType.kSyncEndTag,
            "test_sync",
            [],
            [1],
        ),
        (
            inspector.async_begin,
            inspector.EventType.kAsyncBeginTag,
            "test_async",
            [1, "one"],
            [1, "one", []],
        ),
        (
            inspector.async_instance,
            inspector.EventType.kAsyncInstanceTag,
            "test_async",
            [1, "one"],
            [1, "one", []],
        ),
        (
            inspector.async_end,
            inspector.EventType.kAsyncEndTag,
            "test_async",
            [1, "one"],
            [1, "one", []],
        ),
        (
            inspector.flow_begin,
            inspector.EventType.kFlowBeginTag,
            "test_flow",
            [1, "one"],
            [1, "one", []],
        ),
        (
            inspector.flow_instance,
            inspector.EventType.kFlowInstanceTag,
            "test_flow",
            [1, "one"],
            [1, "one", []],
        ),
        (
            inspector.flow_end,
            inspector.EventType.kFlowEndTag,
            "test_flow",
            [1, "one"],
            [1, "one", []],
        ),
        (
            inspector.counter,
            inspector.EventType.kCounterTag,
            "test_flow",
            [1],
            ["one"],
        ),
    ],
)
def test_trace(func, type, name, debug_args, invalid_debug_args):
    func(name, *debug_args)

    event = inspector.read_trace_event()
    assert event.type() == type.value
    assert event.name() == name
    assert has_debug_args(event, debug_args)

    with pytest.raises((RuntimeError, TypeError)):
        func(name, *invalid_debug_args)
