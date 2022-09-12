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

import argparse
import logging

from periodic_task import PeriodicTask
from tools.reader.reader_py import BasicReader

LOG = logging.getLogger(__name__)


class TraceReader:
    """
    Trace event reader
    """

    def __init__(self) -> None:
        self._reader = BasicReader()

    def __call__(self) -> None:
        LOG.debug("Consuming trace events...")
        for trace in self._reader:
            print(trace)


def parse_args() -> argparse.Namespace:
    """
    Parse command line arguments
    """
    parser = argparse.ArgumentParser(prog="monitor", description="Trace monitor CLI.")
    parser.add_argument(
        "--period", type=float, help="Period of monitoring.", default=0.1
    )
    return parser.parse_args()


def main():
    logging.basicConfig(level=logging.INFO)
    args = parse_args()

    LOG.info("Starting trace consumer...")
    PeriodicTask(args.period).run(TraceReader())


if __name__ == "__main__":
    main()
