from collections.abc import Callable
from os import getenv

from dependency_injector import containers, providers
from dependency_injector.providers import Configuration
from dotenv import load_dotenv

from gs.backend.config.config_models import CORSConfig, LoggerConfig
from gs.backend.config.utils import Singleton


class DeclarativeContainer(containers.DeclarativeContainer):
    """a dependancy injector class"""

    config = Configuration()

    cors_config: Callable[[], CORSConfig] = providers.Factory(
        CORSConfig,
        allow_origins=config.allow_origins,
        allow_credentials=config.allow_credentials,
        allow_methods=config.allow_methods,
        allow_headers=config.allow_headers,
    )

    logger_config: Callable[[], LoggerConfig] = providers.Factory(
        LoggerConfig,
        excluded_endpoints=config.excluded_endpoints,
    )


class BackendConfigurator(metaclass=Singleton):
    """a class to configure backend middleware"""

    DEFAULT_ORIGINS = "http://localhost:5173"

    def __init__(self) -> None:
        self._container = DeclarativeContainer()

    def setup_environment(self, env: str = "") -> None:
        """load environment variables (regular/testing)"""
        if env:  # for tests
            load_dotenv(dotenv_path=env)
        else:
            load_dotenv()
        self._setup_cors_config()
        self._setup_logger_config()

    @property
    def cors_config(self) -> CORSConfig:
        """returns the cors config"""
        return self._container.cors_config()

    @property
    def logger_config(self) -> LoggerConfig:
        """returns the logger config"""
        return self._container.logger_config()

    def _setup_cors_config(self) -> None:
        """load cors configuration from .env file"""

        origins = getenv("ALLOW_ORIGINS", default=BackendConfigurator.DEFAULT_ORIGINS).split(",")
        credentials = getenv("ALLOW_CREDENTIALS", default="true").lower() == "true"
        methods = getenv("ALLOW_METHODS", default="*").split(",")
        headers = getenv("ALLOW_HEADERS", default="*").split(",")

        self._container.config.allow_origins.from_value(origins)
        self._container.config.allow_credentials.from_value(credentials)
        self._container.config.allow_methods.from_value(methods)
        self._container.config.allow_headers.from_value(headers)

    def _setup_logger_config(self) -> None:
        """load logger configuration from .env file"""

        excluded_endpoints = getenv("EXCLUDED_ENDPOINTS", default="").split(",")
        self._container.config.excluded_endpoints.from_value(excluded_endpoints)
