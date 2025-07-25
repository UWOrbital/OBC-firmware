from sqlalchemy import Engine
from sqlmodel import Session, create_engine, text

from gs.backend.config.config import DATABASE_CONNECTION_STRING
from gs.backend.data.tables.aro_user_tables import ARO_USER_SCHEMA_METADATA, ARO_USER_SCHEMA_NAME
from gs.backend.data.tables.main_tables import MAIN_SCHEMA_METADATA, MAIN_SCHEMA_NAME
from gs.backend.data.tables.transactional_tables import TRANSACTIONAL_SCHEMA_METADATA, TRANSACTIONAL_SCHEMA_NAME


def get_db_engine() -> Engine:
    """
    Creates the database engine

    :return: engine
    """
    # TODO: Add loguru to sqlalchemy.engine logger
    return create_engine(DATABASE_CONNECTION_STRING, echo=True)


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


def _create_tables(session: Session) -> None:
    """
    Creates the tables.

    :warning: This assumes the relevant schemas were already created

    :param session: The session for which to create the schemas
    """
    metadatas = [MAIN_SCHEMA_METADATA, ARO_USER_SCHEMA_METADATA, TRANSACTIONAL_SCHEMA_METADATA]
    connection = session.connection()
    for metadata in metadatas:
        metadata.create_all(connection)
        connection.commit()


def setup_database(session: Session) -> None:
    """
    Creates the schemas and tables for the session.

    :param session: The session for which to create the schemas
    """
    _create_schemas(session)
    _create_tables(session)
