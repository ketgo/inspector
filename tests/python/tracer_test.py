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

from typing import List

from python import inspector


def setup_module():
    inspector.Config().EVENT_QUEUE_NAME = "inspector-tracer-test"
    inspector.Config().REMOVE_EVENT_QUEUE_ON_EXIT = True


def consume() -> List[inspector.TraceEvent]:
    reader = inspector.Reader()
    return [inspector.TraceEvent.load(event) for event in reader]


def test_sync_begin():
    inspector.sync_begin("test_sync", 1, "one", arg=[])

    events = consume()
    assert len(events) == 1
    assert events[0].type == "B"
    assert events[0].name == "test_sync"
    assert events[0].payload == "1|one|arg=[]"


def test_sync_end():
    inspector.sync_end("test_sync")

    events = consume()
    assert len(events) == 1
    assert events[0].type == "E"
    assert events[0].name == "test_sync"
    assert events[0].payload == ""


def test_async_begin():
    inspector.async_begin("test_async", 1, "one", arg=[])

    events = consume()
    assert len(events) == 1
    assert events[0].type == "b"
    assert events[0].name == "test_async"
    assert events[0].payload == "1|one|arg=[]"


def test_async_instance():
    inspector.async_instance("test_async", 1, "one", arg=[])

    events = consume()
    assert len(events) == 1
    assert events[0].type == "n"
    assert events[0].name == "test_async"
    assert events[0].payload == "1|one|arg=[]"


def test_async_end():
    inspector.async_end("test_async", 1, "one", arg=[])

    events = consume()
    assert len(events) == 1
    assert events[0].type == "e"
    assert events[0].name == "test_async"
    assert events[0].payload == "1|one|arg=[]"


def test_flow_begin():
    inspector.flow_begin("test_flow", 1, "one", arg=[])

    events = consume()
    assert len(events) == 1
    assert events[0].type == "s"
    assert events[0].name == "test_flow"
    assert events[0].payload == "1|one|arg=[]"


def test_flow_instance():
    inspector.flow_instance("test_flow", 1, "one", arg=[])

    events = consume()
    assert len(events) == 1
    assert events[0].type == "t"
    assert events[0].name == "test_flow"
    assert events[0].payload == "1|one|arg=[]"


def test_flow_end():
    inspector.flow_end("test_flow", 1, "one", arg=[])

    events = consume()
    assert len(events) == 1
    assert events[0].type == "f"
    assert events[0].name == "test_flow"
    assert events[0].payload == "1|one|arg=[]"
