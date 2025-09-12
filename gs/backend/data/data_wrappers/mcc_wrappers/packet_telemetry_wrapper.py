from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import PacketTelemetry


def get_all_packet_telemetries() -> list:
    """
    @brief get all data wrapper for PacketTelemetry
    """
    with get_db_session() as session:
        telemetries = session.exec(select(PacketTelemetry)).all()
        return telemetries


def create_packet_telemetry(telemetry_data: dict) -> PacketTelemetry:
    """
    @brief post data wrapper for PacketTelemetry
    """
    with get_db_session() as session:
        telemetry = PacketTelemetry(**telemetry_data)
        session.add(telemetry)
        session.commit()
        session.refresh(telemetry)
        return telemetry


def delete_packet_telemetry_by_id(telemetry_id: int) -> bool:
    """
    @brief delete data wrapper for PacketTelemetry
    """
    with get_db_session() as session:
        telemetry = session.get(PacketTelemetry, telemetry_id)
        if not telemetry:
            return False
        session.delete(telemetry)
        session.commit()
        return True
