"""
gs.backend.api.v1.aro.endpoints.services 的 Docstring

Helper functions for our endpoints.

1. Authentication
"""

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

def get_user_by_email(email: str) -> AROUsers | None:
    # Find a user by their email address.
    with get_db_session() as session:
        found_user = session.exec(
            select(AROUsers).where(AROUsers.email == email)
        ).first()
    return found_user

def get_user_by_google_id(google_id: str) -> AROUsers | None:
    # Find a user from their Google ID.
    with get_db_session() as session:
        found_user = session.exec(
            select(AROUsers).where(AROUsers.google_id == google_id)
        ).first()
    return found_user
