import pytest
from gs.backend.data.database.engine import setup_database
from sqlalchemy import Engine, NullPool
from sqlmodel import Session, create_engine


@pytest.fixture
def db_engine(postgresql) -> Engine:
    """
    Creates a database engine fixture for the postgresql.
    This is a function level fixture.
    """
    connection = f"postgresql+psycopg://{postgresql.info.user}:@{postgresql.info.host}:{postgresql.info.port}/{postgresql.info.dbname}"
    return create_engine(connection, echo=False, poolclass=NullPool)


@pytest.fixture
def db_session(db_engine: Engine) -> Session:
    """
    Creates a database session fixture for the postgresql.
    This is a function level fixture.
    """
    with Session(db_engine) as session:
        setup_database(session)
        return session
