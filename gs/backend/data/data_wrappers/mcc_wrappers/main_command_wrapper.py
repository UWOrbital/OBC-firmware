from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.main_tables import MainCommand


def get_all_commands() -> list:
    """
    @brief get all data wrapper for MainCommands
    """
    with get_db_session() as session:
        commands = session.exec(select(MainCommand)).all()
        return commands


def create_command(command_data: dict) -> MainCommand:
    """
    @brief post data wrapper for MainCommands
    """
    with get_db_session() as session:
        command = MainCommand(**command_data)
        session.add(command)
        session.commit()
        session.refresh(command)
        return command


def delete_command_by_id(command_id: int) -> bool:
    """
    @brief delete data wrapper for MainCommands
    """
    with get_db_session() as session:
        command = session.get(MainCommand, command_id)
        if not command:
            return False
        session.delete(command)
        session.commit()
        return True
