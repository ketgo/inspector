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
from typing import Union, List

from inspector_core import *


def trace(
    scope_name: Union[None, str] = None,
    args_idx: Union[None, List[int]] = None,
    kwargs_keys: Union[None, List[str]] = None,
):
    """
    Decorator to instrument python methods.

    :param scope_name: Name of the scope. Default set to method name.
    :param args_idx: Non-keyword arguments to record as part of the scope.
                     The parameter takes indecies to the arguments.
    :param kwargs_keys: Keyword arguments to record as part of the scope.
                     The parameter takes key names to the arguments.
    """

    def _trace(func):
        _scope_name = scope_name if scope_name else func.__name__
        _args_idx = list(set(args_idx)) if args_idx else []
        _kwargs_keys = kwargs_keys if kwargs_keys else []

        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            _args = [args[i] for i in _args_idx]
            _kwargs = {key: kwargs[key] for key in _kwargs_keys if key in kwargs}
            sync_begin(_scope_name, *_args, **_kwargs)
            result = func(*args, **kwargs)
            sync_end(_scope_name)
            return result

        return wrapper

    return _trace


class scope:
    """
    The class `scope` is a context manager which can be used for instrumenting blocks of instructions.
    """

    def __init__(self, scope_name: str, *args, **kwargs) -> None:
        self._name = scope_name
        self._args = args
        self._kwargs = kwargs

    def __enter__(self) -> "scope":
        sync_begin(self._name, *self._args, **self._kwargs)
        return self

    def __exit__(self, _1, _2, _3):
        sync_end(self._name)
