# TODO:(335) Improve loading the configuration

from .cors_config import CORSConfig
from .database_config import DatabaseConfig
from .logger_config import LoggerConfig


class BackendConfiguration:
    """
    Class for storing backend configuration settings
    """

    def __init__(self) -> None:
        self.cors = CORSConfig()
        self.logger = LoggerConfig()
        self.db = DatabaseConfig()


settings = BackendConfiguration()
