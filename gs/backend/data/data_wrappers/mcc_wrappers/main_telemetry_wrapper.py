from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.main_tables import MainTelemetry


def get_all_main_telemetries() -> list:
    """
    @brief get all data wrapper for MainTelemetry
    """
    with get_db_session() as session:
        telemetries = session.exec(select(MainTelemetry)).all()
        return telemetries


def create_main_telemetry(telemetry_data: dict) -> MainTelemetry:
    """
    @brief post data wrapper for MainTelemetry
    """
    with get_db_session() as session:
        telemetry = MainTelemetry(**telemetry_data)
        session.add(telemetry)
        session.commit()
        session.refresh(telemetry)
        return telemetry


def delete_main_telemetry_by_id(telemetry_id: int) -> bool:
    """
    @brief delete data wrapper for MainTelemetry
    """
    with get_db_session() as session:
        telemetry = session.get(MainTelemetry, telemetry_id)
        if not telemetry:
            return False
        session.delete(telemetry)
        session.commit()
        return True
