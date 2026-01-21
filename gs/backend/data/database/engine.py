from sqlalchemy import text
from sqlalchemy.ext.asyncio import AsyncEngine, AsyncSession, create_async_engine

from gs.backend.config.config import DATABASE_CONNECTION_STRING
from gs.backend.data.tables.aro_user_tables import ARO_USER_SCHEMA_NAME
from gs.backend.data.tables.main_tables import MAIN_SCHEMA_NAME
from gs.backend.data.tables.transactional_tables import TRANSACTIONAL_SCHEMA_NAME


def get_db_engine() -> AsyncEngine:
    """
    Creates the database engine

    :return: engine
    """
    return create_async_engine(DATABASE_CONNECTION_STRING)


async def get_db_session() -> AsyncSession:
    """
    Creates the database session.

    :warning: This function depends on the `get_db_engine`.

    :return: session
    """
    engine = get_db_engine()
    async with AsyncSession(engine) as session:
        yield session


async def _create_schemas(session: AsyncSession) -> None:
    """
    Creates the schemas in the database.

    :param session: The session for which to create the schemas
    """
    connection = await session.connection()
    schemas = [MAIN_SCHEMA_NAME, TRANSACTIONAL_SCHEMA_NAME, ARO_USER_SCHEMA_NAME]
    for schema in schemas:
        # sqlalchemy doesn't check if the schema exists before attempting to create one
        await connection.execute(text(f"CREATE SCHEMA IF NOT EXISTS {schema}"))
    await connection.commit()


'''Deprecated method to create tables, now handled by Alembic migrations
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
'''


async def setup_database(session: AsyncSession) -> None:
    """
    Creates the schemas for the session.
    Table creation is now handled by Alembic migrations

    :param session: The session for which to create the schemas
    """
    await _create_schemas(session)
