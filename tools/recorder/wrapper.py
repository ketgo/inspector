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

"""
Wrapper script to run Bazel binary targets under trace recorder.
"""

import argparse
import logging
import subprocess
import signal
import tempfile
import os
from pathlib import Path
from typing import Tuple, List, Union

LOG = logging.getLogger(__name__)


def sigint_handler(sig, frame) -> None:
    print("Ctrl+C recivied.")


signal.signal(signal.SIGINT, sigint_handler)


class Recorder:
    """
    Class to record captured trace and metric events.
    """

    def __init__(self, cwd: Path, out_dir: Path) -> None:
        self._started = False
        self._process = None
        self._cwd = cwd
        self._out_dir = out_dir
        if not self._out_dir.exists():
            self._out_dir.mkdir(parents=True)
        self._stderr = None

    @property
    def output_directoy(self) -> Union[None, Path]:
        return self._out_dir

    def __enter__(self) -> "Recorder":
        self.start()
        return self

    def __exit__(self, _a, _b, _c) -> None:
        self.stop()

    def start(self) -> None:
        if self._started:
            LOG.warning("Recorder already running. Ignoring request.")
            return

        LOG.info(f"Starting trace recorder. Data will be stored in {self._out_dir}")
        self._stderr = open(self._out_dir / "trace.log", "w+")
        self._process = subprocess.Popen(
            [
                "tools/recorder/recorder",
                f"--out={self._out_dir}",
            ],
            cwd=self._cwd,
            stderr=self._stderr,
        )
        self._started = True

    def stop(self) -> None:
        if not self._started:
            LOG.warning("Recorder not running. Ignoring request.")
            return

        LOG.info(f"Stopping trace recorder. Data can be found in {self._out_dir}")
        self._process.terminate()
        self._process.wait()
        self._stderr.close()
        self._started = False


def parse_args() -> Tuple[argparse.Namespace, List[str]]:
    """
    Parse CLI arguments.
    """

    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("target", type=str, help="Path to bazel target to run.")
    parser.add_argument(
        "--inspector-recorder-args",
        type=str,
        default="",
        help="Arguments to be passed to recorder.",
    )

    args, target_args = parser.parse_known_args()

    sub_parser = argparse.ArgumentParser(prog="recorder")
    sub_parser.add_argument("--out", type=Path, default=tempfile.mkdtemp("__inspector"))

    recorder_args = sub_parser.parse_args(args.inspector_recorder_args.split())
    recorder_args.target = args.target

    return recorder_args, target_args


def main() -> None:
    logging.basicConfig(level=logging.INFO)

    manifest_file = os.environ.get("RUNFILES_MANIFEST_FILE", None)
    cwd = (
        Path(os.getcwd())
        if not manifest_file
        else Path(manifest_file.replace(".runfiles_manifest", ".runfiles/_main"))
    )

    recorder_args, target_args = parse_args()
    recorder = Recorder(cwd=cwd, out_dir=recorder_args.out)
    with recorder:
        LOG.info("Running target...")
        process = subprocess.Popen(
            [recorder_args.target] + target_args,
        )
        LOG.info(f"Target PID: {process.pid}")
        process.wait()

    LOG.info(f"Trace data stored in '{recorder.output_directoy}'.")


if __name__ == "__main__":
    main()
