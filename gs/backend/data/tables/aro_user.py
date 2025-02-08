from typing import Final
from uuid import UUID

from pydantic import EmailStr
from sqlmodel import Field

from gs.backend.data.tables.base_model import BaseSQLModel

ARO_USER_TABLE_NAME: Final[str] = "aro_users"


class AROUsers(BaseSQLModel, table=True):
    """
    Stores all the information about an ARO user
    """

    id: UUID = Field(primary_key=True, index=True)
    call_sign: str = Field(max_length=6, min_length=5)
    email: EmailStr = Field(max_length=255, min_length=5, unique=True)
    first_name: str = Field(max_length=255)
    last_name: str | None = Field(max_length=255, nullable=True)
    phone_number: str
    __tablename__ = ARO_USER_TABLE_NAME
