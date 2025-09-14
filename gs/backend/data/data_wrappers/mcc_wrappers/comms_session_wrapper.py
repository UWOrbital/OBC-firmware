from typing import Any

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import CommsSession


def get_all_comms_sessions() -> list[CommsSession]:
    """
    @brief get all data wrapper for CommsSession
    """
    with get_db_session() as session:
        sessions = list(session.exec(select(CommsSession)).all())
        return sessions


def create_comms_session(session_data: dict[str, Any]) -> CommsSession:
    """
    @brief post data wrapper for CommsSession
    """
    with get_db_session() as session:
        comms_session = CommsSession(**session_data)
        session.add(comms_session)
        session.commit()
        session.refresh(comms_session)
        return comms_session


def delete_telemetry_by_id(session_id: int) -> bool:
    """
    @brief delete data wrapper for CommsSession
    """
    with get_db_session() as session:
        comms_session = session.get(CommsSession, session_id)
        if not comms_session:
            return False
        session.delete(comms_session)
        session.commit()
        return True
