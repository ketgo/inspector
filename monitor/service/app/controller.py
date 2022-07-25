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
    Service controllers to process client requests.
"""

from flask import request
from flask_socketio import Namespace, emit, join_room

from app.tasks import message_to_client


class SocketAPI(Namespace):
    """
    Namespace to handle socket events.
    """

    def on_connect(self):
        emit(
            "confirmation",
            {"connection_confirmation": ""},
        )

    def on_disconnect(self):
        pass

    def on_submit_name(self, message):
        session_id = request.sid
        roomstr = session_id
        join_room(roomstr)
        name = message["name"]
        message_to_client.delay(name, roomstr)
