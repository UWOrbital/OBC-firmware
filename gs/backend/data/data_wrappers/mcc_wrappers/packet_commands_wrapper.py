from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import PacketCommands


def get_all_packet_commands() -> list:
    """
    @brief get all data wrapper for PacketCommands
    """
    with get_db_session() as session:
        commands = session.exec(select(PacketCommands)).all()
        return commands


def create_telemetry(command_data: dict) -> PacketCommands:
    """
    @brief post data wrapper for PacketCommands
    """
    with get_db_session() as session:
        command = PacketCommands(**command_data)
        session.add(command)
        session.commit()
        session.refresh(command)
        return command


def delete_telemetry_by_id(command_id: int) -> bool:
    """
    @brief delete data wrapper for PacketCommands
    """
    with get_db_session() as session:
        command = session.get(PacketCommands, command_id)
        if not command:
            return False
        session.delete(command)
        session.commit()
        return True
