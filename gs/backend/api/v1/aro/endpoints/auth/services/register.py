from os import urandom
from uuid import uuid4
from fastapi import HTTPException, status
from pydantic import EmailStr
from gs.backend.api.v1.aro.endpoints.auth.services.password import (
    hash_password, 
    verify_password,
)
from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUsers,
)
from gs.backend.data.data_wrappers.wrappers import (
    AROUserAuthTokenWrapper,
    ARORequestWrapper,
    AROUserLoginWrapper,
    AROUsersWrapper,
)
from gs.backend.api.v1.aro.endpoints.auth.services.tokens import create_auth_token
from gs.backend.data.enums.aro_auth_token import AROAuthToken

HASH_ALGORITHM = "sha256"

def register_user(
        email: EmailStr, 
        password: str, 
        first_name: str, 
        last_name: str | None, 
        phone_number: str | None
    ) -> tuple[AROUserAuthToken, AROUsers]:
    users = AROUsersWrapper()
    logins = AROUserLoginWrapper()

    # check for existing email
    existing_user = next((u for u in users.get_all() if u.email == email), None)
    if existing_user:
        raise HTTPException(
            status_code=status.HTTP_409_CONFLICT,
            detail="Email has already been taken.",
        )

    # 2. create email
    user = users.create({
        "call_sign" : None,
        "is_callsign_verified" : False,
        "email": email,
        "first_name": first_name,
        "last_name": last_name,
        "phone_number": phone_number,
    })

    # We now create the login credentials
    salt = urandom(16)
    hashed_password = hash_password(password, salt)
    verification_token = str(uuid4())

    logins.create({
        "email": email,
        "password": hashed_password,
        "password_salt": salt.hex(),
        "hashing_algorithm_name": HASH_ALGORITHM,
        "user_data_id": user.id,
        "email_verification_token": verification_token,
    })

    # create an auth_token
    auth_token = create_auth_token(
        user.id,
        AROAuthToken.EMAIL_PASSWORD,
    )

    return (auth_token, user)

def login_user(email: EmailStr, password: str) -> tuple[AROUserAuthToken, AROUsers]:
    users = AROUsersWrapper()
    login = AROUserLoginWrapper()

    # check for existing email
    login_record = next((l for l in login.get_all() if l.email == email), None)
    if not login_record:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Login credentials invalid.",
        )
    
    # verify password
    if not verify_password(password, login_record.password_salt, login_record.password):
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Incorrect email or password.",
        )

    # get user data
    user = users.get_by_id(login_record.user_data_id)
    if not user:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="User data not found.",
        )

    auth_token = create_auth_token(user.id, AROAuthToken.EMAIL_PASSWORD)

    return (auth_token, user)

def logout_user(token: str) -> None:
    auth_tokens = AROUserAuthTokenWrapper()
    auth_token = auth_tokens.get_by_token(token)

    if not auth_token:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Couldn't find your login credentials. How did you even log in?",
        )

    auth_tokens.delete_by_id(auth_token.id)
