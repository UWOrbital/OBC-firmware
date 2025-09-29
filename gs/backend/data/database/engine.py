from sqlalchemy import Engine
from sqlmodel import Session, create_engine, text

from gs.backend.config.config import DATABASE_CONNECTION_STRING
from gs.backend.data.tables.aro_user_tables import ARO_USER_SCHEMA_NAME
from gs.backend.data.tables.main_tables import MAIN_SCHEMA_NAME
from gs.backend.data.tables.transactional_tables import TRANSACTIONAL_SCHEMA_NAME


def get_db_engine() -> Engine:
    """
    Creates the database engine

    :return: engine
    """
    return create_engine(DATABASE_CONNECTION_STRING)


def get_db_session() -> Session:
    """
    Creates the database session.

    :warning: This function depends on the `get_db_engine`.

    :return: session
    """
    engine = get_db_engine()
    with Session(engine) as session:
        return session


def _create_schemas(session: Session) -> None:
    """
    Creates the schemas in the database.

    :param session: The session for which to create the schemas
    """
    connection = session.connection()
    schemas = [MAIN_SCHEMA_NAME, TRANSACTIONAL_SCHEMA_NAME, ARO_USER_SCHEMA_NAME]
    for schema in schemas:
        # sqlalchemy doesn't check if the schema exists before attempting to create one
        connection.execute(text(f"CREATE SCHEMA IF NOT EXISTS {schema}"))
    connection.commit()


def setup_database(session: Session) -> None:
    """
    Creates the schemas for the session.
    Table creation is now handled by Alembic migrations

    :param session: The session for which to create the schemas
    """
    _create_schemas(session)
