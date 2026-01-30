from datetime import datetime
from uuid import UUID
from pydantic import BaseModel, EmailStr

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
    email: EmailStr
    first_name: str
    last_name: str | None
    is_callsign_verified: bool

    class Config:
        from_attributes = True

class CallsignRequest(BaseModel):
    call_sign: str
