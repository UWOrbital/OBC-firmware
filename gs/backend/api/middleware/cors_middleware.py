from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware


def add_cors_middleware(app: FastAPI) -> None:
    """
    Adds the cors middleware to the FastAPI app

    @param app: FastAPI app to add the middleware to
    """
    print(app.state.config.cors_config)
    app.add_middleware(
        CORSMiddleware,
        allow_origins=app.state.config.cors_config.allow_origins,
        allow_credentials=app.state.config.cors_config.allow_credentials,
        allow_methods=app.state.config.cors_config.allow_methods,
        allow_headers=app.state.config.cors_config.allow_headers,
    )
