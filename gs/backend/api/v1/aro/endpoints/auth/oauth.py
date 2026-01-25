"""
gs.backend.api.v1.aro.auth.oauth 的 Docstring

Supports two authentication methods.

1. Google OAuth
2. Email & Password

After initial authentication, the user will need to additionally verify with their callsign.
"""

from datetime import datetime
from os import urandom
from typing import Any, cast
from uuid import UUID, uuid4

from authlib.integrations.starlette_client import OAuth, OAuthError
from starlette.requests import Request
from starlette.config import Config
from fastapi.responses import RedirectResponse
from fastapi import APIRouter, HTTPException, status
from pydantic import BaseModel, EmailStr
from sqlmodel import select

from gs.backend.api.v1.aro.endpoints.services import (
    hash_password,
    verify_password,
    create_auth_token,
    get_user_by_email,
    get_user_by_google_id,
    create_oauth_user,
)

from gs.backend.config.config import (
    GOOGLE_CLIENT_ID,
    GOOGLE_CLIENT_SECRET,
    JWT_SECRET_KEY,
)
from gs.backend.data.tables.aro_user_tables import (
    AROUserAuthToken,
    AROUserLogin,
    AROUsers,
)
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_auth_token import AROAuthToken

# -----------------------------------------------------------------------
# CONFIG
# -----------------------------------------------------------------------

TOKEY_EXPIRY_HOURS = 6.7
HASH_ALGORITHM = "sha256"
HASH_ITERATIONS = 100_000

router = APIRouter(prefix='/auth', tags=['authentication'])

# Auth Setup
config = Config(".env")
oauth = OAuth(config)
oauth.register(
    name="google",
    client_id=GOOGLE_CLIENT_ID,
    client_secret=GOOGLE_CLIENT_SECRET,
    server_metadata_url="https://accounts.google.com/.well-known/openid-configuration",
    client_kwargs={ "scope": "openid email profile" },
)

# -----------------------------------------------------------------------
# Request & Responses
# -----------------------------------------------------------------------

class RegisterRequest(BaseModel):
    """
    RegisterRequest

    Request body for email/password registration.

    :param email Emailstr
    :param password str
    :param first_name str
    :param last_name str
    :param phone_number str
    """

    email: EmailStr
    password: str
    first_name: str
    last_name: str | None = None
    phone_number: str | None = None

class LoginRequest(BaseModel):
    """
    LoginRequest

    Request body for email/password login.

    :param email EmailStr
    :param password str
    """
    email: EmailStr
    password: str

class TokenResponse(BaseModel):
    """
    TokenResponse

    Request body for the authentication token.

    :param token str
    :param user_id UUID
    :param expires_at datetime
    """
    token: str
    user_id: UUID
    expires_at: datetime

class UserResponse(BaseModel):
    """
    UserResponse

    Response containing the user data.

    :param id UUID
    :param email str
    :param first_name str
    :param last_name str
    :param is_callsigned-
    """
    id: UUID
    email: str
    first_name: str
    last_name: str | None
    is_callsign_verified: bool

    class Config:
        from_attributes = True

# -----------------------------------------------------------------------
# Google OAuth Endpoints
# -----------------------------------------------------------------------

@router.get('/google/login')
async def google_login(request: Request) -> Any:
    """
    google_login 的 Docstring
    
    :param request: 说明
    :type request: Request
    :return: 说明
    :rtype: RedirectResponse

    Initiate Google OAuth flow.
    Redirect the user back to Google's consent screen.
    """
    # The callback URL must match what's configured on Google Cloud Console
    redirect_uri = request.url_for("google_callback")
    return await oauth.google.authorize_redirect(request, redirect_uri)

@router.get("/google/callback")
async def google_callback(request: Request) -> TokenResponse:
    """
    google_callback

    Handle Google OAuth callback.
   
    Creates a new user if first login, otherwise finds existing user.
    Returns an auth token for the session.
    """
    try:
       token = await oauth.google.authorize_access_token(request)
    except OAuthError as e:
        raise HTTPException (
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail=f"OAuth authentication failed: {e.error}",
        )
    
    ### DEBUG
    print(token.keys())

    # Extract user info from the ID token
    user_info = token.get("userinfo")
    if not user_info:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Could not retrieve user data from Google.",
        )
    
    # Extract the rest of the data
    google_id = user_info.get("sub")
    email = user_info.get("email")
    first_name = user_info.get("given_name", "")
    last_name = user_info.get("family_name")

    if not (google_id and email):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Missing required user information from Google.",
        )
    
    # Create new user
    user = get_user_by_google_id(google_id=google_id)
    if not user:
        # Does the user email already exist?
        existing_user = get_user_by_email(email)
        if existing_user:
            # Link Google Account to existing email
            with get_db_session() as session:
                existing_user.google_id = google_id
                session.add(existing_user)
                session.commit()
                session.refresh(existing_user)
                user = existing_user
        else:
            # Create a new user
            user = create_oauth_user(google_id, email, first_name, last_name)

    # Create session token (auth_token)
    auth_token = create_auth_token(user.id, AROAuthToken.GOOGLE_OAUTH)

    return TokenResponse(
        token=auth_token.token,
        user_id=user.id,
        expires_at=auth_token.expiry,
    )

#print(type(oauth.google))
#print("authorize_redirect" in dir(oauth.google))

# -----------------------------------------------------------------------
# Email / Password Endpoints
# -----------------------------------------------------------------------

@router.post("/register", response_model=TokenResponse)
async def register(request: RegisterRequest) -> TokenResponse:
    """
    register

    Register a new user with email and password.
   
    Creates both AROUsers and AROUserLogin records.
    Returns an auth token for immediate login.
    """
    # Check if email exists
    existing_user = get_user_by_email(request.email)
    if existing_user:
        raise HTTPException(
            status_code=status.HTTP_409_CONFLICT,
            detail="Email has already been taken.",
        )
    
    # Create all user data
    with get_db_session() as session:
        user = AROUsers(
            call_sign=None,
            is_callsign_verified=False,
            email=request.email,
            first_name=request.first_name,
            last_name=request.last_name,
            phone_number=request.phone_number,
        )
        session.add(user)
        session.commit()
        session.refresh(user)

        # We now create the login credentials
        salt = urandom(16)
        hashed_password = hash_password(request.password, salt)
        verification_token = str(uuid4())

        login = AROUserLogin(
            email=user.email,
            password=hashed_password,
            password_salt=salt.hex(),
            hashing_algorithm_name=HASH_ALGORITHM,
            user_data_id=user.id,
            email_verification_token=verification_token,
        )
        session.add(login)
        session.commit()

        # Create our session token
        auth_token = create_auth_token(user.id, AROAuthToken.EMAIL_PASSWORD)

        return TokenResponse(
            token=auth_token.token,
            user_id=user.id,
            expires_at=auth_token.expiry,
        )

@router.post("/login", response_model=TokenResponse)
async def login(request: LoginRequest) -> TokenResponse:
    """
    login

    Validates credentials and returns an auth token.
    If unsuccessful, gives appropriate errors.
    """
    with get_db_session() as session:
        login_record = session.exec(
            select(AROUserLogin).where(AROUserLogin.email == request.email)
        ).first()

        if not login_record:
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Login credentials invalid.",
            )
        
        # Verify password
        if not verify_password(request.password, login_record.password_salt, login_record.password):
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Incorrect email or password.",
            )
        
        # Get the user data
        user = session.get(AROUsers, login_record.user_data_id)
        if not user:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="User data not found.",
            )

    # Create our session auth token
    auth_token = create_auth_token(user.id, AROAuthToken.EMAIL_PASSWORD)

    return TokenResponse(
        token=auth_token.token,
        user_id=user.id,
        expires_at=auth_token.expiry,
    )

# -----------------------------------------------------------------------
# Email / Password Endpoints
# -----------------------------------------------------------------------

@router.post("/logout")
async def logout(token: str) -> dict[str, str]:
    """
    logout

    Invalidate an auth token (logout).
    Deletes the token from the database.
    """
    with get_db_session() as session:
        auth_token = session.exec(
            select(AROUserAuthToken).where(AROUserAuthToken.token == token)
        ).first()

        if not auth_token:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Couldn't find your login credentials. How did you even log in?",
            )
        
        session.delete(auth_token)
        session.commit()
    
    return {
        "message" : "Logged out successfully."
    }


@router.get("/currentuser")
async def get_current_user(token: str) -> UserResponse:
    """
    get_current_user

    Get the current user's information from their auth token.
    """
    with get_db_session() as session:
        auth_token = session.exec(
            select(AROUserAuthToken).where(AROUserAuthToken.token == token)
        ).first()

        if not auth_token:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Couldn't find your login credentials. How did you even log in?",
            )
        
        # Check for expiracy
        if (auth_token.expiry < datetime.now()):
            session.delete(auth_token)
            session.commit()
            raise HTTPException(
                status_code=status.HTTP_401_UNAUTHORIZED,
                detail="Your login expired.",
            )

        user = session.get(AROUsers, auth_token.user_data_id)
        if not user:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="User not found.",
            )

        return UserResponse(
            id=user.id,
            email=user.email,
            first_name=user.first_name,
            last_name=user.last_name,
            is_callsign_verified=user.is_callsign_verified,
        )
