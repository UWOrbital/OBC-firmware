from sqlmodel import Session, select

from gs.backend.data.resources.callsigns import callsigns
from gs.backend.data.resources.main_commands import main_commands
from gs.backend.data.tables.main_tables import MainCommand


def add_main_commands(session: Session) -> None:
    """
    Setup the main commands to the database
    """
    query = select(MainCommand).limit(1)  # Check if the db is empty
    result = session.exec(query).first()
    if not result:
        session.add_all(main_commands())
        session.commit()


def add_callsigns(session: Session) -> None:
    """
    Setup the valid callsigns to the database
    """
    query = select(MainCommand).limit(1)
    result = session.exec(query).first()
    if not result:
        session.add_all(callsigns())
        session.commit()
