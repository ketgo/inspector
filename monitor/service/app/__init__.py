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
    Security service application
"""


from flask import Flask
from flask_cors import CORS
from flask_socketio import SocketIO
from healthcheck import HealthCheck

socketio = SocketIO(logger=True, engineio_logger=True)
health = HealthCheck()


def create_app(config_file=None) -> Flask:
    """
        Create Flask app object after initialization of service.

        :param config_file: configuration file
        :return: flask app object
    """

    # Create flask app
    app = Flask(__name__)

    # Configure flask app
    app.config.from_object(config_file)

    # SocketIO initialization
    socketio.init_app(app, cors_allowed_origins="*")

    # Setting health check endpoint
    app.add_url_rule("/health", view_func=lambda: health.run())

    # Setting up CORS for routes
    CORS(app, expose_headers=["ETag"])

    return app
