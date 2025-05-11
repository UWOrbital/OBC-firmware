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
