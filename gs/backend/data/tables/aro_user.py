from typing import Final
from uuid import UUID, uuid4

from pydantic import EmailStr
from sqlmodel import Field

from gs.backend.data.constants import CALL_SIGN_MAX_LENGTH, CALL_SIGN_MIN_LENGTH, DEFAULT_MAX_LENGTH, EMAIL_MIN_LENGTH
from gs.backend.data.tables.base_model import BaseSQLModel

ARO_USER_TABLE_NAME: Final[str] = "aro_users"


class AROUsers(BaseSQLModel, table=True):
    """
    Stores all the information about an ARO user
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    call_sign: str = Field(min_length=CALL_SIGN_MIN_LENGTH, max_length=CALL_SIGN_MAX_LENGTH)
    email: EmailStr = Field(min_length=EMAIL_MIN_LENGTH, max_length=DEFAULT_MAX_LENGTH, unique=True)
    first_name: str = Field(max_length=DEFAULT_MAX_LENGTH)
    last_name: str | None = Field(max_length=DEFAULT_MAX_LENGTH, nullable=True)
    phone_number: str
    __tablename__ = ARO_USER_TABLE_NAME
