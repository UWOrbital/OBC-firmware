from uuid import UUID

from pydantic import BaseModel


class CreateUser(BaseModel):
    """
    Base model for create user, used for the create user endpoint
    """

    first_name: str
    last_name: str
    call_sign: str
    email: str
    phone_number: str


class ChangeUserInfo(BaseModel):
    """
    Base model for changing user info, used for the change user info endpoint
    """

    user_id: UUID
    first_name: str | None = None
    last_name: str | None = None
    call_sign: str | None = None


class GetUserData(BaseModel):
    """
    Base model for getting user info, used by the get user info endpoint
    """

    user_id: UUID
