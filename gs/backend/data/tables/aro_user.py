from typing import Final
from uuid import UUID, uuid4

from pydantic import EmailStr
from sqlalchemy.schema import MetaData
from sqlmodel import Field

from gs.backend.data.constants import (
    CALL_SIGN_MAX_LENGTH,
    CALL_SIGN_MIN_LENGTH,
    DEFAULT_MAX_LENGTH,
    EMAIL_MIN_LENGTH,
)
from gs.backend.data.tables.base_model import BaseSQLModel

# Schema information
ARO_USER_SCHEMA_NAME: Final[str] = "aro_users"
ARO_USER_SCHEMA_METADATA: Final[MetaData] = MetaData(ARO_USER_SCHEMA_NAME)

# Table names in database
ARO_USER_TABLE_NAME: Final[str] = "users_data"


class AROUsers(BaseSQLModel, table=True):
    """
    @brief Stores all the information about an ARO user

    @attribute id (UUID) - ARO User ID. Auto generated on insert
    @attribute call_sign (str) - ARO User's call sign that we will use to communicate with them
    @attribute email (EmailStr) - Valid email
    @attribute first_name (str) -  First name of ARO user
    @attribute last_name (str | None) -  Optional last name
    @attribute phone_number (str) - Valid phone number
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    call_sign: str = Field(min_length=CALL_SIGN_MIN_LENGTH, max_length=CALL_SIGN_MAX_LENGTH)
    email: EmailStr = Field(min_length=EMAIL_MIN_LENGTH, max_length=DEFAULT_MAX_LENGTH, unique=True)
    first_name: str = Field(max_length=DEFAULT_MAX_LENGTH)
    last_name: str | None = Field(max_length=DEFAULT_MAX_LENGTH, nullable=True)
    phone_number: str

    # table information
    metadata = ARO_USER_SCHEMA_METADATA
    __tablename__ = ARO_USER_TABLE_NAME
