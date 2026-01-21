from typing import Any
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import Commands


async def get_all_commands() -> list[Commands]:
    """
    Get all data wrapper for Commands

    :return: a list of all commands
    """
    async with get_db_session() as session:
        result = await session.exec(select(Commands))
        commands = list(result.all())
        return commands


async def create_commands(command_data: dict[str, Any]) -> Commands:
    """
    Post data wrapper for Commands

    :param command_data: the JSON object of the command to be created
    :return: the newly created command
    """
    async with get_db_session() as session:
        command = Commands(**command_data)
        session.add(command)
        await session.commit()
        await session.refresh(command)
        return command


async def delete_commands_by_id(command_id: UUID) -> Commands:
    """
    Delete data wrapper for Commands

    :param command_id: UUID of command to be deleted
    :return: the deleted command
    """
    async with get_db_session() as session:
        command = await session.get(Commands, command_id)
        if not command:
            raise ValueError("Command not found.")
        session.delete(command)
        await session.commit()
        return command
