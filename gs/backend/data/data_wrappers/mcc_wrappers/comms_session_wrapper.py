from typing import Any
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import CommsSession


async def get_all_comms_sessions() -> list[CommsSession]:
    """
    Get all data wrapper for CommsSession

    :return: a list of all sessions
    """
    async with get_db_session() as session:
        result = await session.execute(select(CommsSession))
        sessions = list(result.scalars().all())
        return sessions


async def create_comms_session(session_data: dict[str, Any]) -> CommsSession:
    """
    Post data wrapper for CommsSession

    :param session_data: the JSON object of the comms_session to be created
    :return: the newly created comms_session
    """
    async with get_db_session() as session:
        comms_session = CommsSession(**session_data)
        session.add(comms_session)
        await session.commit()
        await session.refresh(comms_session)
        return comms_session


async def delete_telemetry_by_id(session_id: UUID) -> CommsSession:
    """
    Delete data wrapper for CommsSession

    :param session_id: UUID of session to be deleted
    :return: the deleted session
    """
    async with get_db_session() as session:
        comms_session = await session.get(CommsSession, session_id)
        if not comms_session:
            raise ValueError("Comms session not found.")
        session.delete(comms_session)  # type: ignore[unused-coroutine]
        await session.commit()
        return comms_session
