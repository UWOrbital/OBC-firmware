
from datetime import datetime
from os import urandom
from typing import Any, cast
from uuid import uuid4

from authlib.integrations.starlette_client import OAuth, OAuthError
from starlette.requests import Request
from starlette.config import Config
from fastapi import APIRouter, HTTPException, status, Depends
from pydantic import BaseModel, EmailStr
from sqlmodel import select

from gs.backend.api.v1.aro.endpoints.auth.services.services import (
    hash_password,
    verify_password,
    create_auth_token,
    get_user_by_email,
    get_user_by_google_id,
    create_oauth_user,
    verify_callsign,
)
from gs.backend.data.data_wrappers.wrappers import (
    AROUserAuthTokenWrapper,
    AROUserLoginWrapper,
    ARORequestWrapper,
    AROUsersWrapper,
)
from gs.backend.config.config import (
    GOOGLE_CLIENT_ID,
    GOOGLE_CLIENT_SECRET,
)
from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUserCallsigns,
    AROUserLogin,
    AROUsers,
)

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_auth_token import AROAuthToken

router = APIRouter(prefix='/auth', tags=['authentication'])

@router.get("/currentuser")
async def get_current_user(token: str) -> AROUsers:
    """
    get_current_user

    Get the current user's information from their auth token.
    """
    token_wrapper = AROUserAuthTokenWrapper()
    user_wrapper = AROUsersWrapper()

    auth_token = next(
        (t for t in token_wrapper.get_all() if t.token == token),
        None,
    )

    if not auth_token:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Couldn't find your login credentials. How did you even log in?",
        )

    # Check for expiracy
    if (auth_token.expiry < datetime.now()):
        token_wrapper.delete_by_id(auth_token.id)
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Your login expired.",
        )
    
    return user_wrapper.get_by_id(auth_token.user_data_id)


@router.post("/is-verified")
def require_verified_user(user: AROUsers = Depends(get_current_user)) -> AROUsers:
    if not user.is_callsign_verified:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN, 
            detail="Callsign verification required"
        )
    return user
