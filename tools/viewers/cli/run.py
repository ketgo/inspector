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

from inspector import TraceEvent
from tools.reader.reader_py import Reader

LOG = logging.getLogger(__name__)


def parse_args() -> argparse.Namespace:
    """
    Parse command line arguments
    """
    parser = argparse.ArgumentParser(prog="monitor", description="Trace monitor CLI.")
    parser.add_argument(
        "--timeout",
        type=float,
        help="Timeout in seconds when waiting for new events to consume.",
        default=30.0,
    )
    parser.add_argument(
        "--polling_interval_us",
        type=float,
        help="Event queue consumer polling interval in seconds.",
        default=0.01,
    )
    return parser.parse_args()


def main():
    logging.basicConfig(level=logging.INFO)
    args = parse_args()

    LOG.info("Starting trace consumer...")

    reader = Reader(
        # timeout=args.timeout,
        # polling_interval=args.polling_interval,
    )
    LOG.debug("Consuming trace events...")
    for trace in reader:
        print(trace)


if __name__ == "__main__":
    main()
