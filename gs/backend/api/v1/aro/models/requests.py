from pydantic import BaseModel, EmailStr

class AROUsersRequest(BaseModel):
    """
    Model representing the user to be created
    """

    call_sign: str
    email: EmailStr
    first_name: str
    last_name: str | None
    phone_number: str
