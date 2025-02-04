from fastapi import FastAPI

from gs.backend.api.middleware.cors_middleware import add_cors_middleware


def setup_routes(app: FastAPI) -> None:
    """Adds the routes to the app"""


def setup_middlewares(app: FastAPI) -> None:
    """Adds the middlewares to the app"""
    add_cors_middleware(app)
