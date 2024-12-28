import argparse
import logging
import os
import subprocess
import time

LOG = logging.getLogger(__name__)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--create", action="store_true")

    return parser.parse_args()


def main() -> None:
    logging.basicConfig(
        format="%(asctime)s.%(msecs)03d %(levelname)-8s %(message)s",
        level=logging.INFO,
        datefmt="%Y-%m-%d %H:%M:%S",
    )

    pid = os.getpid()
    LOG.info(f"Process ID: {pid}")

    args = parse_args()
    if args.create:
        process = subprocess.Popen(["tools/recorder/experimental"], start_new_session=True)

    try:
        while True:
            pass
    except KeyboardInterrupt:
        LOG.info(f"{pid}: Keyboard Interrupt")

    time.sleep(1)

    if args.create:
        LOG.info(f"{pid} Terminating child process.")
        process.terminate()


if __name__ == "__main__":
    main()
