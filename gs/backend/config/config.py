# TODO:(335) Improve loading the configuration
from os import environ, getenv
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


def getenv(config: str) -> str:
    """
    Validates whether or not database env values exist in .env. If not, throws a value error.

    :param config: Variable in .env
    :type config: str
    :return: Value of variable in .env
    :rtype: str
    """
    value = environ.get(config)
    if not value:
        raise ValueError(f"{config} is missing from .env.")
    return value

GOOGLE_CLIENT_ID = getenv("GOOGLE_CLIENT_ID")
GOOGLE_CLIENT_SECRET = getenv("GOOGLE_CLIENT_SECRET")
JWT_SECRET_KEY = getenv("JWT_SECRET_KEY")

GS_DATABASE_USER = getenv("GS_DATABASE_USER")
GS_DATABASE_PASSWORD = getenv("GS_DATABASE_PASSWORD")
GS_DATABASE_LOCATION = getenv("GS_DATABASE_LOCATION")
GS_DATABASE_PORT = getenv("GS_DATABASE_PORT")
GS_DATABASE_NAME = getenv("GS_DATABASE_NAME")

DATABASE_CONNECTION_STRING: Final[
    str
] = f"postgresql+psycopg2://{GS_DATABASE_USER}:{GS_DATABASE_PASSWORD}@{GS_DATABASE_LOCATION}:{GS_DATABASE_PORT}/{GS_DATABASE_NAME}"
