from datetime import datetime

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_auth_token import AROAuthToken as AROEnums
from gs.backend.data.tables.aro_user_tables import AROAuthToken


def get_all_auth_tokens() -> list[AROAuthToken]:
    """
    @brief get all the auth tokens
    """
    with get_db_session() as session:
        auth_tokens = session.exec(select(AROAuthToken).all())
        return auth_tokens


def add_auth_token(token: str, expiry: datetime, auth_type: AROEnums) -> AROAuthToken:
    """
    @brief add auth token to the db
    """
    with get_db_session() as session:
        auth_token = AROAuthToken(token=token, expiry=expiry, auth_type=auth_type)

        session.add(auth_token)
        session.commit()
        session.refresh(auth_token)
        return auth_token


def delete_auth_token(token_id: str) -> list[AROAuthToken]:
    """
    @brief delete the auth token based on the token id
    """

    with get_db_session() as session:
        auth_token = session.exec(select(AROAuthToken).where(AROAuthToken.id == AROAuthToken)).first()

        if AROAuthToken:
            session.delete(auth_token)
            session.commit()
        else:
            print("Token does not exist")

        return get_all_auth_tokens()
