from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import Packet


def get_all_packets() -> list:
    """
    @brief get all data wrapper for Packet
    """
    with get_db_session() as session:
        packets = session.exec(select(Packet)).all()
        return packets


def create_packet(packet_data: dict) -> Packet:
    """
    @brief post data wrapper for Packet
    """
    with get_db_session() as session:
        packet = Packet(**packet_data)
        session.add(packet)
        session.commit()
        session.refresh(packet)
        return packet


def delete_packet_by_id(packet_id: int) -> None:
    """
    @brief delete data wrapper for Packet
    """
    with get_db_session() as session:
        packet = session.get(Packet, packet_id)
        if not packet:
            return False
        session.delete(packet)
        session.commit()
        return True
