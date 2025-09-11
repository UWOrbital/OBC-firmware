from datetime import datetime
from os import urandom
from typing import Final
from uuid import UUID, uuid4

from pydantic import EmailStr
from sqlalchemy.schema import MetaData
from sqlmodel import Field

from gs.backend.config.data_config import (
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
ARO_USER_LOGIN: Final[str] = "user_login"


class AROUsers(BaseSQLModel, table=True):
    """
    Stores all the information about an ARO user

    :param id: ARO User ID. Auto generated on insert
    :type id: UUID
    :param call_sign: ARO User's call sign that we will use to communicate with them
    :type call_sign: str
    :param email: Valid email
    :type email: EmailStr
    :param first_name: First name of ARO user
    :type first_name: str
    :param last_name: Optional last name
    :type last_name: str, None
    :param phone_number: Valid phone number
    :type phone_number: str
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    call_sign: str = Field(min_length=CALL_SIGN_MIN_LENGTH, max_length=CALL_SIGN_MAX_LENGTH)
    email: EmailStr = Field(min_length=EMAIL_MIN_LENGTH, max_length=DEFAULT_MAX_LENGTH, unique=True)
    first_name: str = Field(max_length=DEFAULT_MAX_LENGTH)
    last_name: str | None = Field(max_length=DEFAULT_MAX_LENGTH, nullable=True, default=None)
    phone_number: str

    # table information
    metadata = ARO_USER_SCHEMA_METADATA
    __tablename__ = ARO_USER_TABLE_NAME


class AROUserLogin(BaseSQLModel, table=True):
    """
    Stores all information on AROUserLogin

    :param id: AROUserLogin id
    :param email: AROUserLogin email for login
    :param password: AROUserLogin password for login
    :param salt: 16 random bytes for password hashing
    :param created_on: datetime object of the time at which AROUserLogin was created
    :param hashing_algorithm_name: the name of the hashing algorithm for pwd hashing
    :param user_data_id: id created by AROUsers
    :param email_verification_token: given after user verifies
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)  # unique id for logins
    email: EmailStr = Field(min_length=EMAIL_MIN_LENGTH, max_length=DEFAULT_MAX_LENGTH, unique=True)
    password: str = Field(max_length=20)
    salt: bytes = urandom(16)
    created_on: datetime = datetime.now()
    hashing_algorithm_name: str = Field(min_length=1, max_length=20)
    user_data_id: UUID
    email_verification_token: str

    metadata = ARO_USER_SCHEMA_METADATA
    __tablename__ = ARO_USER_LOGIN
