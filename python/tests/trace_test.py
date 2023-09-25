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

import py_inspector as inspector


def setup_module():
    inspector.Config().EVENT_QUEUE_NAME = "inspector-trace-test"
    inspector.Config().REMOVE_EVENT_QUEUE_ON_EXIT = True


def consume() -> inspector.testing.Event:
    return inspector.testing.consume_test_event()


def test_sync_begin():
    inspector.sync_begin("test_sync", 1, "one", arg=[])

    event = consume()
    assert event.phase == "B"
    assert event.name == "test_sync"
    assert event.args == "1|one|arg=[]"


def test_sync_end():
    inspector.sync_end("test_sync")

    event = consume()
    assert event.phase == "E"
    assert event.name == "test_sync"
    assert event.args == ""


def test_async_begin():
    inspector.async_begin("test_async", 1, "one", arg=[])

    event = consume()
    assert event.phase == "b"
    assert event.name == "test_async"
    assert event.args == "1|one|arg=[]"


def test_async_instance():
    inspector.async_instance("test_async", 1, "one", arg=[])

    event = consume()
    assert event.phase == "n"
    assert event.name == "test_async"
    assert event.args == "1|one|arg=[]"


def test_async_end():
    inspector.async_end("test_async", 1, "one", arg=[])

    event = consume()
    assert event.phase == "e"
    assert event.name == "test_async"
    assert event.args == "1|one|arg=[]"


def test_flow_begin():
    inspector.flow_begin("test_flow", 1, "one", arg=[])

    event = consume()
    assert event.phase == "s"
    assert event.name == "test_flow"
    assert event.args == "1|one|arg=[]"


def test_flow_instance():
    inspector.flow_instance("test_flow", 1, "one", arg=[])

    event = consume()
    assert event.phase == "t"
    assert event.name == "test_flow"
    assert event.args == "1|one|arg=[]"


def test_flow_end():
    inspector.flow_end("test_flow", 1, "one", arg=[])

    event = consume()
    assert event.phase == "f"
    assert event.name == "test_flow"
    assert event.args == "1|one|arg=[]"
