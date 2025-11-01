from collections.abc import AsyncGenerator
from contextlib import asynccontextmanager

from fastapi import FastAPI

from gs.backend.common.logger import logger_close, logger_setup
from gs.backend.data.database.engine import get_db_session, setup_database
from gs.backend.data.resources.utils import add_main_commands


@asynccontextmanager
async def lifespan(_: FastAPI) -> AsyncGenerator[None, None]:
    """Lifecycle event for the FastAPI app."""
    # Must all the get_db_session each time when pass it into a separate function.
    # Otherwise, will get transaction is inactive error

    # setup logger
    logger_setup(enqueue=True)

    # Setup database and commands
    setup_database(get_db_session())
    add_main_commands(get_db_session())

    # yield control to FASTApi app
    yield

    # shutdown logger when app closes
    await logger_close()
