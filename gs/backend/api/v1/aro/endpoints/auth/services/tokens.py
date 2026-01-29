from datetime import datetime, timedelta

from hashlib import pbkdf2_hmac
from uuid import UUID, uuid4
from sqlmodel import select
from fastapi import Depends, HTTPException, status

from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUserCallsigns,
    AROUsers,
)
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_auth_token import AROAuthToken

TOKEN_EXPIRY_HOURS = 6.7
HASH_ALGORITHM = "sha256"
HASH_ITERATIONS = 100_000

def create_auth_token(user_id: UUID, auth_type: AROAuthToken) -> AROUserAuthToken:
    # Create and persist an authentication token for a user.
    created_time = datetime.now()
    expiry = created_time + timedelta(hours=TOKEN_EXPIRY_HOURS)
    token_value = str(uuid4())

    with get_db_session() as session:
        token = AROUserAuthToken(
            user_data_id=user_id,
            token=token_value,
            created_on=created_time,
            expiry=expiry,
            auth_type=auth_type,
        )
        session.add(token)
        session.commit()
        session.refresh(token)
        return token

def create_oauth_user(google_id: str, email: str, first_name: str, last_name: str | None) -> AROUsers:
    # Create a new user from Google OAuth data.
    with get_db_session() as session:
        user = AROUsers(
            google_id=google_id,
            email=email,
            first_name=first_name,
            last_name=last_name,
            call_sign=None,
            is_callsign_verified=False,
        )
        session.add(user)
        session.commit()
        session.refresh(user)
        return user
