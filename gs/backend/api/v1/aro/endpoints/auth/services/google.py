from gs.backend.data.data_wrappers.wrappers import (
    AROUsersWrapper,
    AROUserLoginWrapper,
)
from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUsers,
)
from gs.backend.data.enums.aro_auth_token import AROAuthToken
from gs.backend.api.v1.aro.endpoints.auth.services.tokens import create_auth_token
from pydantic import EmailStr
from uuid import uuid4

def google_auth(google_id: str, email: EmailStr, first_name: str, last_name: str) -> tuple[AROUserAuthToken, AROUsers]:
    users = AROUsersWrapper()
    logins = AROUserLoginWrapper()

    # Try google ID
    all_users = users.get_all()
    user = next((u for u in all_users if u.google_id == google_id), None)

    # If not, try the email
    if not user:
        user = next((u for u in all_users if u.email == email), None)

        if user:
            # Link user account
            user = users.update(
                user.id,
                {"google_id" : google_id},
            )
        else:
            # Create a new account
            user = users.create({
                "email": email,
                "first_name": first_name,
                "last_name": last_name,
                "google_id": google_id,
                "is_callsign_verified": False,
            })

            logins.create(
                {
                    "email": email,
                    "user_data_id": user.id,
                    "email_verification_token": str(uuid4()),
                }
            )

    # 3. Create auth token
    auth_token = create_auth_token(user.id, AROAuthToken.GOOGLE_OAUTH)
    
    return (auth_token, user)
        