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
 ---

 The module implements a periodic task decorator for async methods.

"""

import time

from typing import Callable


class PeriodicTask:
    """
    Periodic task
    """

    def __init__(self, period: float) -> None:
        self._period = period
        
    def run(self, func: Callable, *args, **kwargs) -> None:
        """
        Execute the periodic task.
        """
        while True:
            func(*args, **kwargs)
            time.sleep(self._period)
