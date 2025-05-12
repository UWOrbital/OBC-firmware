from pydantic import BaseModel, EmailStr


class UserCreateRequest(BaseModel):
    """
    User creation request model.
    """

    call_sign: str
    email: EmailStr
    first_name: str
    last_name: str | None = None
    phone_number: str


class AROUserUpdateRequest(BaseModel):
    """
    Model used for updating the ARO user
    """

    call_sign: str
    first_name: str
    last_name: str | None = None
    phone_number: str
