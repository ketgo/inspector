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

import os
import argparse

from app import create_app, socketio, ext
from config import CONFIG

# Getting environment type
env = os.environ.get("FLASK_ENV", "default")

# Creating flask application
app = create_app(CONFIG.get(env))

# Celery application
celery = ext.celery


def main() -> None:
    """
    Main entrypoint.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("--type", choices=["server", "worker"], default="server")
    args = parser.parse_args()

    if args.type == "server":
        socketio.run(app, host="0.0.0.0", port=8000)
    elif args.type == "worker":
        celery.Worker(loglevel="DEBUG").start()


if __name__ == "__main__":
    main()
