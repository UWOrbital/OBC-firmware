from backend.api.endpoints.command import command_router
from backend.api.endpoints.main_command import main_command_router
from backend.api.middlewares.cors_middleware import add_cors_middleware
from backend.api.middlewares.logger_middleware import LoggerMiddleware
from fastapi import FastAPI


def setup_routes(app: FastAPI) -> None:
    """Adds the routes to the app"""
    app.include_router(router=command_router, prefix="/commands")
    app.include_router(router=main_command_router, prefix="/main-commands")


def setup_middlewares(app: FastAPI) -> None:
    """Adds the middlewares to the app"""
    add_cors_middleware(app)
    app.add_middleware(LoggerMiddleware)