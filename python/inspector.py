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

import functools
from typing import Union

from inspector_core import *


def trace(scope_name: Union[None, str] = None):
    """
    Decorator to instrument python methods.

    :param scope_name: Name of the scope. Default set to method name.
    """

    def _trace(func):
        _scope_name = scope_name if scope_name else func.__name__

        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            sync_begin(_scope_name)
            result = func(*args, **kwargs)
            sync_end(_scope_name)
            return result

        return wrapper

    return _trace


class scope:
    """
    The class `scope` is a context manager which can be used for instrumenting blocks of instructions.
    """

    def __init__(self, scope_name: str) -> None:
        self._name = scope_name

    def __enter__(self) -> "scope":
        sync_begin(self._name)
        return self

    def __exit__(self, _1, _2, _3):
        sync_end(self._name)
