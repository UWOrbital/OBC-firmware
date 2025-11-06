# TODO:(335) Improve loading the configuration
from os import environ
from typing import Any, Final, TypeVar

from dotenv import load_dotenv

from .cors_config import CORSConfig
from .logger_config import LoggerConfig

load_dotenv()

T = TypeVar("T")


class SingletonMeta(type):
    """
    singleton metaclass used for defining BackendConfiguration class
    """

    _instances: dict[type, Any] = {}

    def __call__(cls: type[T], *args: tuple[Any, ...], **kwargs: dict[str, Any]) -> T:
        """Return the singleton instance of the class, creating it if it does not exist."""
        if cls not in cls._instances:
            instance = super().__call__(*args, **kwargs)
            cls._instances[cls] = instance
        else:
            instance = cls._instances[cls]

        return instance


class BackendConfiguration(metaclass=SingletonMeta):

    """
    Class for storing backend configuration settings
    """

    def __init__(self) -> None:
        self.cors_config = CORSConfig()
        self.logger_config = LoggerConfig()


# TODO: Make these throw an exception if they are None
GS_DATABASE_USER = environ.get("GS_DATABASE_USER")
GS_DATABASE_PASSWORD = environ.get("GS_DATABASE_PASSWORD")
GS_DATABASE_LOCATION = environ.get("GS_DATABASE_LOCATION")
GS_DATABASE_PORT = environ.get("GS_DATABASE_PORT")
GS_DATABASE_NAME = environ.get("GS_DATABASE_NAME")

DATABASE_CONNECTION_STRING: Final[
    str
] = f"postgresql+psycopg2://{GS_DATABASE_USER}:{GS_DATABASE_PASSWORD}@{GS_DATABASE_LOCATION}:{GS_DATABASE_PORT}/{GS_DATABASE_NAME}"
