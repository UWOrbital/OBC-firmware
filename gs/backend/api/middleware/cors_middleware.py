from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware

from gs.backend.config.config import backend_config


def add_cors_middleware(app: FastAPI) -> None:
    """
    Adds the cors middleware to the FastAPI app

    :param app: FastAPI app to add the middleware to
    """
    app.add_middleware(
        CORSMiddleware,
        allow_origins=backend_config.cors_config.allow_origins,
        allow_credentials=backend_config.cors_config.allow_credentials,
        allow_methods=backend_config.cors_config.allow_methods,
        allow_headers=backend_config.cors_config.allow_headers,
    )
