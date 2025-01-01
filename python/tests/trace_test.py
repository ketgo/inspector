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

from typing import Dict, List, Any, Tuple

import pytest
import inspector


def extract_debug_args(event: inspector.TraceEvent) -> Tuple[List[Any], Dict[str, Any]]:
    """
    Utility method to extract debug arguments in the given trace event.
    """
    debug_args = event.debug_args()
    args = []
    kwargs = {}
    for arg in debug_args:
        value = arg.value()
        if arg.type() == inspector.DebugArg.Type.TYPE_KWARG:
            kwargs[value[0]] = value[1]
        else:
            args.append(value)

    return args, kwargs


def setup_module():
    inspector.Config.set_event_queue_name("inspector-trace-test")


def teardown_modlue():
    inspector.testing.remove_event_queue()


def setup_function():
    inspector.testing.empty_event_queue()


@pytest.mark.parametrize(
    "func,type,name,debug_args,debug_kwargs,invalid_debug_args",
    [
        (
            inspector.sync_begin,
            inspector.EventType.kSyncBeginTag,
            "test_sync",
            [1, "one"],
            {"arg_1": 1, "arg_2": 2},
            [1, "one", []],
        ),
        (
            inspector.sync_end,
            inspector.EventType.kSyncEndTag,
            "test_sync",
            [],
            {},
            [1],
        ),
        (
            inspector.async_begin,
            inspector.EventType.kAsyncBeginTag,
            "test_async",
            [1, "one"],
            {},
            [1, "one", []],
        ),
        (
            inspector.async_instance,
            inspector.EventType.kAsyncInstanceTag,
            "test_async",
            [1, "one"],
            {},
            [1, "one", []],
        ),
        (
            inspector.async_end,
            inspector.EventType.kAsyncEndTag,
            "test_async",
            [1, "one"],
            {},
            [1, "one", []],
        ),
        (
            inspector.flow_begin,
            inspector.EventType.kFlowBeginTag,
            "test_flow",
            [1, "one"],
            {},
            [1, "one", []],
        ),
        (
            inspector.flow_instance,
            inspector.EventType.kFlowInstanceTag,
            "test_flow",
            [1, "one"],
            {},
            [1, "one", []],
        ),
        (
            inspector.flow_end,
            inspector.EventType.kFlowEndTag,
            "test_flow",
            [1, "one"],
            {},
            [1, "one", []],
        ),
        (
            inspector.counter,
            inspector.EventType.kCounterTag,
            "test_flow",
            [1],
            {},
            ["one"],
        ),
    ],
)
def test_core(func, type, name, debug_args, debug_kwargs, invalid_debug_args):
    func(name, *debug_args, **debug_kwargs)

    event = inspector.read_trace_event()
    assert event.type() == type.value
    assert event.name() == name
    args, kwargs = extract_debug_args(event)
    assert debug_args == args
    assert debug_kwargs == kwargs

    with pytest.raises((RuntimeError, TypeError)):
        func(name, *invalid_debug_args)


def test_trace_decorator():
    @inspector.trace(scope_name="test-scope", args_idx=[0], kwargs_keys=["b"])
    def add(a: int, b: int) -> int:
        return a + b

    assert add(1, b=2) == 3

    event = inspector.read_trace_event()
    assert event.type() == inspector.EventType.kSyncBeginTag.value
    assert event.name() == "test-scope"
    args, kwargs = extract_debug_args(event)
    assert args == [1]
    assert kwargs == {"b": 2}

    event = inspector.read_trace_event()
    assert event.type() == inspector.EventType.kSyncEndTag.value
    assert event.name() == "test-scope"


def test_scope():
    with inspector.scope("test-scope"):
        pass

    event = inspector.read_trace_event()
    assert event.type() == inspector.EventType.kSyncBeginTag.value
    assert event.name() == "test-scope"

    event = inspector.read_trace_event()
    assert event.type() == inspector.EventType.kSyncEndTag.value
    assert event.name() == "test-scope"
