from pydantic import EmailStr
from sqlmodel import Field, Uuid

from gs.backend.data.tables.base_model import BaseSQLModel


class AROUsers(BaseSQLModel, table=True):
    """
    Stores all the information about an ARO user
    """

    id: Uuid = Field(primary_key=True, index=True)
    call_sign: str = Field(max_length=6, min_length=5)
    email: EmailStr = Field(max_length=255, min_length=5, unique=True)
    first_name: str = Field(max_length=255)
    last_name: str | None = Field(max_length=255, nullable=True)
    phone_number: str
