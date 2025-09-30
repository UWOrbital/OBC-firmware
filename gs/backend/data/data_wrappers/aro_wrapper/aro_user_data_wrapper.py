from typing import Any
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.aro_user_tables import AROUsers


# selects all objects of type AROUser from db and returns them in list
def get_all_users() -> list[AROUsers]:
    """
    Gets all user
    """
    with get_db_session() as session:
        users = list(session.exec(select(AROUsers)).all())
        return users


# adds user to database of type AROUser then fetches the user from database
# so that the user now has an assigned ID
def add_user(call_sign: str, email: str, f_name: str, l_name: str, phone_number: str) -> AROUsers:
    """
    Add a new user to the AROUser table in database

    :param call_sign: a 6 character string such as ABCDEF
    :param email: unique email which is bound to the user
    :param f_name: first name of user
    :param l_name: last name of user
    :param phone_numer: phone number of user
    """
    with get_db_session() as session:
        # check if the user already exists with email as it is unique
        existing_user = session.exec(select(AROUsers).where(AROUsers.email == email)).first()

        if existing_user:
            raise ValueError("User already exsits based on email")

        user = AROUsers(
            call_sign=call_sign, email=email, first_name=f_name, last_name=l_name, phone_number=phone_number
        )

        session.add(user)
        session.commit()
        session.refresh(user)
        return user


# deletes the user with given id and returns the remaining users
def delete_user_by_id(userid: UUID) -> list[AROUsers]:
    """
    Use the user.id to delete a user from table

    :param userid: identifier unique to the user
    """
    with get_db_session() as session:
        user = session.get(AROUsers, userid)

        if user:
            session.delete(user)
            session.commit()
        else:
            raise ValueError("User ID does not exist")

        return get_all_users()


# deletes the user with given id and returns the remaining users
def get_user_by_id(userid: UUID) -> list[AROUsers]:
    """
    Use the user.id to delete a user from table

    :param userid: identifier unique to the user
    """
    with get_db_session() as session:
        user = session.get(AROUsers, userid)

        if user:
            return user
        else:
            raise ValueError("User ID does not exist")


def modify_user(userid: UUID, **kwargs: dict[str, Any]) -> AROUsers:
    """
    Modifies the target user

    :param userid: identifier unique to the user
    """
    with get_db_session() as session:
        user = session.get(AROUsers, userid)
        if not user:
            raise ValueError("User does not exist based on ID")

        for field, value in kwargs.items():
            setattr(user, field, value)

        session.commit()
        session.refresh(user)
        return user
