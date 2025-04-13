from collections.abc import AsyncGenerator
from contextlib import asynccontextmanager

from fastapi import FastAPI

from gs.backend.data.data.utils import add_main_commands
from gs.backend.data.database.engine import get_db_session, setup_database


@asynccontextmanager
async def lifespan(_: FastAPI) -> AsyncGenerator[None, None]:
    """Lifecycle event for the FastAPI app."""
    # Must all the get_db_session each time when pass it into a separate function.
    # Otherwise, will get transaction is inactive error
    setup_database(get_db_session())
    add_main_commands(get_db_session())
    yield
