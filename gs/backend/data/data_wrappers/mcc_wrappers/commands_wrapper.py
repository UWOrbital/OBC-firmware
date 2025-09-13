from typing import Any

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import Commands


def get_all_commands() -> list[Commands]:
    """
    @brief get all data wrapper for Commands
    """
    with get_db_session() as session:
        commands = list(session.exec(select(Commands))).all()
        return commands


def create_commands(command_data: dict[str, Any]) -> Commands:
    """
    @brief post data wrapper for Commands
    """
    with get_db_session() as session:
        command = Commands(**command_data)
        session.add(command)
        session.commit()
        session.refresh(command)
        return command


def delete_commands_by_id(command_id: int) -> bool:
    """
    @brief delete data wrapper for Commands
    """
    with get_db_session() as session:
        command = session.get(Commands, command_id)
        if not command:
            return False
        session.delete(command)
        session.commit()
        return True
