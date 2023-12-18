"""
 Copyright 2023 Ketan Goyal
 
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

import inspector
from tools.reader.reader_py import Reader


def setup_module():
    inspector.Config.set_event_queue_name("inspector-trace-test")


def teardown_modlue():
    inspector.testing.remove_event_queue()


def setup_function():
    inspector.testing.empty_event_queue()


def test_iterate():
    inspector.sync_begin("test_event_1")
    inspector.sync_end("test_event_1")

    reader = Reader()
    events = [event for event in reader]

    assert len(events) == 2
