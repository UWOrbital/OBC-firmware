from fastapi import FastAPI
from sqlmodel import Session, select

from gs.backend.data.resources.main_commands import main_commands
from gs.backend.data.tables.aro_user_tables import AROUsers
from gs.backend.data.tables.main_tables import MainCommand


def add_main_commands(session: Session) -> None:
    """
    Setup the main commands to the database
    """
    query = select(MainCommand).limit(1)  # Check if the db is empty
    result = session.exec(query).first()
    if result is None:
        session.add_all(main_commands())
        session.commit()


def add_default_user(app: FastAPI, session: Session) -> None:
    """
    @brief Creates the default user in the database to be used in the app
    TODO: Remove this function once we implement auth

    @param app (FastAPI) - The app instance for which to attach the user
    @param session (Session) - The database session in which to create the default user
    """
    # Check to make sure user doesn't exists before inserting it
    email = "test1@test.com"
    user_query = select(AROUsers).where(AROUsers.email == email)
    user = session.exec(user_query).first()

    if user is None:
        user = AROUsers(call_sign="12356", email=email, first_name="John", last_name="Doe", phone_number="123456789")
        session.add(user)
        session.commit()
        session.refresh(user)
    app.state.user_id = user.id
