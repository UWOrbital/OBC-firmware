from datetime import datetime
from fastapi import APIRouter, HTTPException, status, Depends

from gs.backend.data.data_wrappers.wrappers import (
    AROUserAuthTokenWrapper,
    AROUsersWrapper,
)
from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUserCallsigns,
    AROUserLogin,
    AROUsers,
)
from gs.backend.data.enums.aro_auth_token import AROAuthToken
from gs.backend.api.v1.aro.endpoints.auth.auth_schemas import UserResponse

router = APIRouter(prefix='/auth', tags=['authentication'])

@router.get("/currentuser")
async def get_current_user(token: str) -> UserResponse:
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
    
    user = user_wrapper.get_by_id(auth_token.user_data_id)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Couldn't find the user from ID."
        )

    return UserResponse(
        id=user.id,
        email=user.email,
        first_name=user.first_name,
        last_name=user.last_name,
        is_callsign_verified=user.is_callsign_verified,
    )

@router.post("/isverified")
def require_verified_user(user: AROUsers = Depends(get_current_user)) -> AROUsers:

    if not user.is_callsign_verified:
        raise HTTPException(
            status_code=status.HTTP_403_FORBIDDEN, 
            detail="Callsign verification required"
        )
    return user
