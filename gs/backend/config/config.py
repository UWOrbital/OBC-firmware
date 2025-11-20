# TODO:(335) Improve loading the configuration
from os import environ
from typing import Final

from dotenv import load_dotenv

from .cors_config import CORSConfig
from .logger_config import LoggerConfig

load_dotenv()


class BackendConfiguration:
    """
    Class for storing backend configuration settings
    """

    def __init__(self) -> None:
        self.cors_config = CORSConfig()
        self.logger_config = LoggerConfig()


backend_config = BackendConfiguration()

# TODO: Make these throw an exception if they are None
GS_DATABASE_USER = environ.get("GS_DATABASE_USER")
GS_DATABASE_PASSWORD = environ.get("GS_DATABASE_PASSWORD")
GS_DATABASE_LOCATION = environ.get("GS_DATABASE_LOCATION")
GS_DATABASE_PORT = environ.get("GS_DATABASE_PORT")
GS_DATABASE_NAME = environ.get("GS_DATABASE_NAME")

DATABASE_CONNECTION_STRING: Final[
    str
] = f"postgresql+psycopg2://{GS_DATABASE_USER}:{GS_DATABASE_PASSWORD}@{GS_DATABASE_LOCATION}:{GS_DATABASE_PORT}/{GS_DATABASE_NAME}"
