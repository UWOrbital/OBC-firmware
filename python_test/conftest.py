import os
import subprocess
from contextlib import asynccontextmanager
from datetime import datetime
from typing import Any

import pytest
from gs.backend.data.database.engine import setup_database
from gs.backend.data.tables.transactional_tables import CommsSession
from sqlalchemy import Engine, NullPool
from sqlmodel import Session, create_engine


class AsyncSessionWrapper:
    """Wraps a synchronous Session to make it compatible with async/await syntax in tests."""

    def __init__(self, session: Session):
        self._session = session

    async def execute(self, *args, **kwargs):
        """Async wrapper for execute"""
        return self._session.execute(*args, **kwargs)

    async def commit(self):
        """Async wrapper for commit - in tests, just flush to make changes visible"""
        # Flush to database but don't commit the transaction
        self._session.flush()

    async def refresh(self, *args, **kwargs):
        """Async wrapper for refresh"""
        return self._session.refresh(*args, **kwargs)

    async def get(self, *args, **kwargs):
        """Async wrapper for get"""
        return self._session.get(*args, **kwargs)

    def add(self, *args, **kwargs):
        """Passthrough for add (not async in original)"""
        return self._session.add(*args, **kwargs)

    def delete(self, *args, **kwargs):
        """Passthrough for delete (not async in SQLAlchemy)"""
        return self._session.delete(*args, **kwargs)

    def expunge_all(self):
        """Expunge all objects from the session"""
        return self._session.expunge_all()

    def __getattr__(self, name: str) -> Any:
        """Fallback to get any other attributes from the wrapped session"""
        return getattr(self._session, name)


@pytest.fixture
def db_engine(postgresql) -> Engine:
    """
    Creates a database engine fixture for the postgresql.
    This is a function level fixture.
    """
    from gs.backend.data.tables.aro_user_tables import ARO_USER_SCHEMA_NAME
    from gs.backend.data.tables.main_tables import MAIN_SCHEMA_NAME
    from gs.backend.data.tables.transactional_tables import TRANSACTIONAL_SCHEMA_NAME
    from sqlalchemy import text

    connection = f"postgresql+psycopg://{postgresql.info.user}:@{postgresql.info.host}:{postgresql.info.port}/{postgresql.info.dbname}"
    engine = create_engine(connection, echo=False, poolclass=NullPool)

    # Set up schemas and run migrations
    with Session(engine) as setup_session:
        conn = setup_session.connection()

        #  Create schemas (idempotent)
        for schema in [MAIN_SCHEMA_NAME, TRANSACTIONAL_SCHEMA_NAME, ARO_USER_SCHEMA_NAME]:
            conn.execute(text(f"CREATE SCHEMA IF NOT EXISTS {schema}"))
        conn.commit()

        # Run Alembic migrations
        repo_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        env = os.environ.copy()
        env["SQLALCHEMY_DATABASE_URL"] = connection
        subprocess.run(["alembic", "upgrade", "head"], cwd=repo_root, env=env, check=True, capture_output=True)

        # Clean all data from tables (in case of previous test run leftovers)
        try:
            conn.execute(text(f"TRUNCATE TABLE {ARO_USER_SCHEMA_NAME}.users_data CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {ARO_USER_SCHEMA_NAME}.user_login CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {ARO_USER_SCHEMA_NAME}.auth_tokens CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {ARO_USER_SCHEMA_NAME}.callsigns CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {TRANSACTIONAL_SCHEMA_NAME}.aro_requests CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {TRANSACTIONAL_SCHEMA_NAME}.commands CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {TRANSACTIONAL_SCHEMA_NAME}.telemetry CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {TRANSACTIONAL_SCHEMA_NAME}.comms_session CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {TRANSACTIONAL_SCHEMA_NAME}.packet CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {TRANSACTIONAL_SCHEMA_NAME}.packet_commands CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {TRANSACTIONAL_SCHEMA_NAME}.packet_telemetry CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {MAIN_SCHEMA_NAME}.commands CASCADE"))
            conn.execute(text(f"TRUNCATE TABLE {MAIN_SCHEMA_NAME}.telemetry CASCADE"))
            conn.commit()
        except Exception:
            # Tables might not exist yet on first run
            conn.rollback()

    return engine


@pytest.fixture
def db_session(db_engine: Engine) -> Session:
    """
    Creates a database session fixture for the postgresql.
    This is a function level fixture.
    """
    # Create a simple session without transaction wrapping
    with Session(db_engine) as session:
        yield session


@pytest.fixture
def default_start_time() -> datetime:
    return datetime(2025, 1, 1, 12, 25, 38)


@pytest.fixture
def default_comms_session(default_start_time: datetime) -> CommsSession:
    """
    Creates the comms session
    This is a function level fixture.
    """
    comms_session_item = CommsSession(start_time=default_start_time)
    return comms_session_item


@pytest.fixture(autouse=True)
def test_get_db_session(request, monkeypatch):
    """
    When testing any database function that requires the `get_db_session()` function, you must add the module path to the list below.
    This fixture mocks get_db_session to return an async context manager that yields a wrapped test db_session.
    Only activates when db_session is actually used by the test.
    """
    # Check if the test actually uses db_session
    if "db_session" not in request.fixturenames:
        return

    # Get the db_session fixture
    db_session = request.getfixturevalue("db_session")

    path_list: list[str] = [
        "gs.backend.data.data_wrappers.abstract_wrapper",
        "gs.backend.data.data_wrappers.aro_wrapper.aro_request_wrapper",
        "gs.backend.data.data_wrappers.aro_wrapper.aro_user_data_wrapper",
        "gs.backend.data.data_wrappers.aro_wrapper.aro_user_auth_token_wrapper",
        "gs.backend.data.data_wrappers.aro_wrapper.aro_user_login_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.commands_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.comms_session_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.main_command_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.main_telemetry_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.packet_commands_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.packet_telemetry_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.packet_wrapper",
        "gs.backend.data.data_wrappers.mcc_wrappers.telemetry_wrapper",
    ]

    @asynccontextmanager
    async def mock_get_db_session():
        yield AsyncSessionWrapper(db_session)

    for path in path_list:
        monkeypatch.setattr(path + ".get_db_session", mock_get_db_session, raising=True)
