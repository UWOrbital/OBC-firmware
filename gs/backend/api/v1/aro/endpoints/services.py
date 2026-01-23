"""
gs.backend.api.v1.aro.endpoints.services 的 Docstring

Helper functions for our endpoints.

1. Authentication
"""

from datetime import datetime, timedelta

from hashlib import pbkdf2_hmac
from uuid import UUID, uuid4
from sqlmodel import select

from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUsers,
)
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_auth_token import AROAuthToken
from gs.backend.data.tables.aro_user_tables import AROAuthToken

TOKEN_EXPIRY_HOURS = 6.7
HASH_ALGORITHM = "sha256"
HASH_ITERATIONS = 100_000

def hash_password(password: str, salt: bytes) -> str:
    # Hash a password using PBKDF2
    hashed = pbkdf2_hmac(HASH_ALGORITHM, password.encode(), salt, HASH_ITERATIONS)
    return hashed.hex()

def verify_password(password: str, salt: bytes, hashed: str) -> bool:
    # Verify a hashed password against its hash
    return hash_password(password, salt) == hashed

def create_auth_token(user_id: UUID, auth_type: AROUserAuthToken) -> AROUserAuthToken:
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

def get_user_by_email(email: str) -> AROUsers | None:
    # Find a user by their email address.
    with get_db_session() as session:
        found_user = session.exec(select(AROUsers).where(AROUsers.email == email))
        return found_user

def get_user_by_google_id(google_id: str) -> AROUsers | None:
    # Find a user from their Google ID.
    with get_db_session() as session:
        found_user = session.exec(select(AROUsers).where(AROUsers.google_id == google_id))
        return found_user

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
