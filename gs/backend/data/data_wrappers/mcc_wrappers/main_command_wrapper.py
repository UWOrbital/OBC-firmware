from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.main_tables import MainCommand


def get_all_commands() -> list:
    """
    @brief data wrapper for main_commands
    """
    with get_db_session() as session:
        commands = session.exec(select(MainCommand)).all()
        return commands
