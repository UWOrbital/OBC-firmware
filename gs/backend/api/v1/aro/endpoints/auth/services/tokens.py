from datetime import datetime, timedelta
from uuid import UUID, uuid4

from gs.backend.data.enums.aro_auth_token import AROAuthToken
from pydantic import EmailStr

from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUsers,
)
from gs.backend.data.data_wrappers.wrappers import (
    AROUserAuthTokenWrapper,
    AROUsersWrapper,
)


TOKEN_EXPIRY_HOURS = 6.7

def create_auth_token(user_id: UUID, auth_type: AROAuthToken) -> AROUserAuthToken:
    # Create and persist an authentication token for a user.
    created_time = datetime.now()
    expiry = created_time + timedelta(hours=TOKEN_EXPIRY_HOURS)
    token_value = str(uuid4())

    token = AROUserAuthTokenWrapper()
    auth_token = token.create({
        "user_data_id" : user_id,
        "token" : token_value,
        "created_on" : created_time,
        "expiry" : expiry,
        "auth_type" : auth_type,
    })

    return auth_token

def create_oauth_user(google_id: str, email: EmailStr, first_name: str, last_name: str | None) -> AROUsers:
    # Create a new user from Google OAuth data.
    users = AROUsersWrapper()
    user = users.create({
        "google_id" : google_id,
        "email" : email,
        "first_name" : first_name,
        "last_name" : last_name,
        "call_sign" : None,
        "is_callsign_verified" : False,
    })

    return user
