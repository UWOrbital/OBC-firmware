from collections.abc import Callable
from os import getenv

from dependency_injector import containers, providers
from dotenv import load_dotenv

from gs.backend.config.config_models import CORSConfig, LoggingConfig

# class Singleton(type):
#     """a singleton metaclass"""

#     _instances: dict = {}

#     def __call__(cls, *args, **kwargs):
#         if cls not in cls._instances:
#             cls._instances[cls] = super().__call__(*args, **kwargs)
#         return cls._instances[cls]


class Container(containers.DeclarativeContainer):
    """a dependancy injector class"""

    config = providers.Configuration()

    cors_config: Callable[[], CORSConfig] = providers.Factory(
        CORSConfig,
        allow_origins=config.allow_origins,
        allow_credentials=config.allow_credentials,
        allow_methods=config.allow_methods,
        allow_headers=config.allow_headers,
    )

    logging_config: Callable[[], LoggingConfig] = providers.Factory(
        LoggingConfig,
        excluded_endpoints=config.excluded_endpoints,
    )


class BackendConfigurator:  # to add:BackendConfiguration(metaclass=Singleton)
    """a class to configure backend middleware"""

    def __init__(self) -> None:
        self.container = Container()
        self.setup_cors_config()
        self.setup_logging_config()

    def get_cors_config(self) -> CORSConfig:
        """returns the cors config"""
        return self.container.cors_config()

    def get_logging_config(self) -> LoggingConfig:
        """returns the logging config"""
        return self.container.logging_config()

    def setup_cors_config(self) -> None:
        """load cors configuration from .env file"""
        load_dotenv()

        origins = getenv("ALLOW_ORIGINS", default="http://localhost:5173").split(",")
        credentials = getenv("ALLOW_CREDENTIALS", default="true").lower() == "true"
        methods = getenv("ALLOW_METHODS", default="*").split(",")
        headers = getenv("ALLOW_HEADERS", default="*").split(",")

        self.container.config.allow_origins.from_value(origins)
        self.container.config.allow_credentials.from_value(credentials)
        self.container.config.allow_methods.from_value(methods)
        self.container.config.allow_headers.from_value(headers)

    def setup_logging_config(self) -> None:
        """load logging configuration from .env file"""
        load_dotenv()

        excluded_endpoints = getenv("EXLCLUDED_ENDPOINTS", default="").split(",")
        print(excluded_endpoints)
        self.container.config.excluded_endpoints.from_value(excluded_endpoints)
