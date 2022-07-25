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

"""
    Background celery tasks
"""

import time

from app import ext
from flask_socketio import SocketIO


@ext.celery.task()
def message_to_client(name, room):
    socketio = SocketIO(message_queue="amqp://")
    count = 5
    while count > 1:
        count -= 1
        socketio.emit("response", {"count": count}, namespace="/test", room=room)
        time.sleep(1)
    socketio.emit("response", {"name": name}, namespace="/test", room=room)
