from typing import Any

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import Telemetry


def get_all_telemetries() -> list[Telemetry]:
    """
    @brief get all data wrapper for Telemetry
    """
    with get_db_session() as session:
        telemetries = list(session.exec(select(Telemetry)).all())
        return telemetries


def create_telemetry(telemetry_data: dict[str, Any]) -> Telemetry:
    """
    @brief post data wrapper for Telemetry
    """
    with get_db_session() as session:
        telemetry = Telemetry(**telemetry_data)
        session.add(telemetry)
        session.commit()
        session.refresh(telemetry)
        return telemetry


def delete_telemetry_by_id(telemetry_id: int) -> bool:
    """
    @brief delete data wrapper for Telemetry
    """
    with get_db_session() as session:
        telemetry = session.get(Telemetry, telemetry_id)
        if not telemetry:
            return False
        session.delete(telemetry)
        session.commit()
        return True
