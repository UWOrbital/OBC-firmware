from contextlib import asynccontextmanager
from datetime import datetime
from fastapi import FastAPI
from sqlmodel import SQLModel, Session, select
from gs.backend.data.data_models import MainCommand
from backend.data.engine import get_db
from gs.backend.data.mock_data import commands, main_commands
from backend.utils.logging import logger_setup
from backend.utils.time import to_unix_time


def create_startup(session: Session) -> None:
    logger_setup()
    SQLModel.metadata.create_all(session.connection())
    default_time = "2024-01-01T00:00:00"
    default_datetime = datetime.strptime(default_time, "%Y-%m-%dT%H:%M:%S")
    unix_time = to_unix_time(default_datetime)
    # Setup the db with mock data
    query = select(MainCommand).limit(1)  # Check if the db is empty
    result = session.exec(query).first()
    if result is None:
        session.add_all(main_commands())
        session.commit()
        session.add_all(commands(unix_time))
        session.commit()


@asynccontextmanager
async def lifespan(_: FastAPI):
    """Lifecycle event for the FastAPI app."""
    create_startup(get_db())
    yield
    print("Closing lifespan")