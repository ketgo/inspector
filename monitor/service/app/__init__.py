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

import os

from flask import Flask, render_template
from flask_cors import CORS
from flask_socketio import SocketIO
from flask_celeryext import FlaskCeleryExt
from healthcheck import HealthCheck

socketio = SocketIO(logger=True, engineio_logger=True)
ext = FlaskCeleryExt()
health = HealthCheck()


def create_app(config_file=None) -> Flask:
    """
    Create Flask app object after initialization of service.

    :param config_file: configuration file
    :return: flask app object
    """

    # Create flask app
    app = Flask(
        __name__, template_folder="{}/monitor/service/templates".format(os.getcwd())
    )
    # app = Flask(__name__)

    # Configure flask app
    app.config.from_object(config_file)

    # Setting HTML routes
    @app.route("/")
    def index():
        return render_template("index.html")

    # Setting health check endpoint
    app.add_url_rule("/health", view_func=lambda: health.run())

    # Setting up CORS for routes
    CORS(app, expose_headers=["ETag"])

    # Register celery extension
    ext.init_app(app)

    # Register SocketIO extension
    socketio.init_app(app, message_queue="amqp://", cors_allowed_origins="*")

    from app.controller import SocketAPI

    socketio.on_namespace(SocketAPI("/test"))

    return app
