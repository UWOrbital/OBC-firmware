from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from gs.backend.config.config import BackendConfigurator


def add_cors_middleware(app: FastAPI) -> None:
    """
    Adds the cors middleware to the FastAPI app

    @param app: FastAPI app to add the middleware to
    """
    configurator = BackendConfigurator()
    cors_config = configurator.get_cors_config()

    app.add_middleware(
        CORSMiddleware,
        allow_origins=cors_config.allow_origins,
        allow_credentials=cors_config.allow_credentials,
        allow_methods=cors_config.allow_methods,
        allow_headers=cors_config.allow_headers,
    )
