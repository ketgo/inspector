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
    Service configuration file
"""

import os


class Config:
    """
    Default configurations
    """

    # Default flask configuration settings
    DEBUG = True
    TESTING = False

    # Celery settings
    CELERY_BROKER_URL = os.environ.get("INSPECTOR_BROKER_URL", "amqp://")
    CELERY_RESULT_BACKEND = os.environ.get("INSPECTOR_RESULT_BACKEND", "rpc")


class TestConfig(Config):
    """
    Test configuration
    """

    TESTING = True


class DevConfig(Config):
    """
    Dev environment configuration
    """


class QaConfig(Config):
    """
    QA environment configuration
    """


class ProdConfig(Config):
    """
    Production environment configuration
    """


CONFIG = {
    "default": Config,
    "test": TestConfig,
    "dev": DevConfig,
    "qa": QaConfig,
    "prod": ProdConfig,
}
