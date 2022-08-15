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

from random import random

from python.inspector import Writer, Reader, Config


def setup_module():
    config = Config()
    config.EVENT_QUEUE_SYSTEM_UNIQUE_NAME = "inspector-{}-testing".format(random())
    Writer.set_config(config)
    config.REMOVE = True
    Reader.set_config(config)


def test_writer_reader():
    writer = Writer()
    writer.write("testing_1")
    writer.write("testing_2")

    reader = Reader()
    events = [event for event in reader]
    assert len(events) == 2
    assert events[0] == "testing_1"
    assert events[1] == "testing_2"
