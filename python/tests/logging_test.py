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

from py_inspector import testing


def test_logging(caplog):
    caplog.set_level(logging.INFO)
    testing.write_test_log()
    assert len(caplog.records) == 1
    assert caplog.records[0].levelname == logging.getLevelName(logging.ERROR)
    assert caplog.records[0].msg == "Test Log Message"
