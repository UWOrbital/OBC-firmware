"""
gs.backend.api.v1.aro.auth.oauth 的 Docstring

Supports two authentication methods.

1. Google OAuth
2. Email & Password

After initial authentication, the user will need to additionally verify with their callsign.
"""

from typing import Any
from starlette.config import Config
from starlette.requests import Request
from authlib.integrations.starlette_client import OAuth, OAuthError

from fastapi import APIRouter, HTTPException, status
from gs.backend.api.v1.aro.endpoints.auth.services.google import google_auth
from gs.backend.api.v1.aro.endpoints.auth.services.register import (
    register_user,
    login_user,
    logout_user,
)

from gs.backend.config.config import (
    GOOGLE_CLIENT_ID,
    GOOGLE_CLIENT_SECRET,
)

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

from gs.backend.api.v1.aro.endpoints.auth.auth_schemas import (
    RegisterRequest,
    LoginRequest,
    TokenResponse,
)

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

    # Extract user info from the ID token
    user_info = token.get("userinfo")
    if not user_info:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Could not retrieve user data from Google.",
        )
    
    # Extract the rest of the data
    auth_token, user = google_auth(
        google_id=user_info.get("sub"),
        email=user_info.get("email"),
        first_name=user_info.get("given_name", ""),
        last_name=user_info.get("family_name")
    )

    return TokenResponse(
        token=auth_token.token,
        user_id=user.id,
        expires_at=auth_token.expiry,
    )

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
    auth_token, user = register_user(
        email=request.email,
        password=request.password,
        first_name=request.first_name,
        last_name=request.last_name,
        phone_number=request.phone_number,
    )

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
    
    auth_token, user = login_user(request.email, request.password)

    return TokenResponse(
        token=auth_token.token,
        user_id=user.id,
        expires_at=auth_token.expiry,
    )

@router.post("/logout")
async def logout(token: str) -> dict[str, str]:
    """
    logout

    Invalidate an auth token (logout).
    Deletes the token from the database.
    """
    logout_user(token)

    return {
        "message" : "Logged out successfully."
    }
