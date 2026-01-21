from typing import Any
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import PacketCommands


async def get_all_packet_commands() -> list[PacketCommands]:
    """
    Get all data wrapper for PacketCommands

    :return: a list of all packet_commands
    """
    async with get_db_session() as session:
        result = await session.exec(select(PacketCommands))
        commands = list(result.all())
        return commands


async def create_packet_command(command_data: dict[str, Any]) -> PacketCommands:
    """
    Post data wrapper for PacketCommands

    :param command_data: the JSON object of the packet_command to be created
    :return: the newly created packet_command
    """
    async with get_db_session() as session:
        command = PacketCommands(**command_data)
        session.add(command)
        await session.commit()
        await session.refresh(command)
        return command


async def delete_packet_command_by_id(command_id: UUID) -> PacketCommands:
    """
    Delete data wrapper for PacketCommands

    :param command_id: UUID of packet_command to be deleted
    :return: the deleted packet_command
    """
    async with get_db_session() as session:
        command = await session.get(PacketCommands, command_id)
        if not command:
            raise ValueError("Packet command not found.")
        session.delete(command)
        await session.commit()
        return command
