from datetime import datetime
from decimal import Decimal
from typing import Final
from uuid import UUID, uuid4

from sqlalchemy.schema import MetaData
from sqlmodel import Field

from gs.backend.data.constants import (
    COORDINATE_DECIMAL_NUMBER,
    LATITUDE_MAX_DIGIT_NUMBER,
    LONGITUDE_MAX_DIGIT_NUMBER,
    PACKET_DATA_LENGTH,
    PACKET_RAW_LENGTH,
)
from gs.backend.data.enums.aro_requests import ARORequestStatus
from gs.backend.data.enums.transactional import (
    CommandStatus,
    MainPacketType,
    SessionStatus,
)
from gs.backend.data.tables.aro_user import ARO_USER_TABLE_NAME
from gs.backend.data.tables.base_model import BaseSQLModel
from gs.backend.data.tables.main import (
    MAIN_COMMAND_TABLE_NAME,
    MAIN_SCHEMA_NAME,
    MAIN_TELEMETRY_TABLE_NAME,
    MainTableID,
)

# Transactional schema related items
TRANSACTIONAL_SCHEMA_NAME: Final[str] = "transactional"
TRANSACTIONAL_SCHEMA_METADATA: Final[MetaData] = MetaData(TRANSACTIONAL_SCHEMA_NAME)

# Table names in database
ARO_REQUEST_TABLE_NAME: Final[str] = "aro_requests"
COMMS_SESSION_TABLE_NAME: Final[str] = "comms_session"
PACKET_TABLE_NAME: Final[str] = "packet"
TELEMETRY_TABLE_NAME: Final[str] = "telemetry"
COMMANDS_TABLE_NAME: Final[str] = "commands"
PACKET_TELEMETRY_TABLE_NAME: Final[str] = "packet_telemetry"
PACKET_COMMANDS_TABLE_NAME: Final[str] = "packet_commands"

# Transactional data tables


class ARORequest(BaseSQLModel, table=True):
    """
    Holds the data related to an ARO picture request
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    aro_id: UUID = Field(foreign_key=f"{ARO_USER_TABLE_NAME}.id")
    latitude: Decimal = Field(max_digits=LATITUDE_MAX_DIGIT_NUMBER, decimal_places=COORDINATE_DECIMAL_NUMBER)
    longitude: Decimal = Field(max_digits=LONGITUDE_MAX_DIGIT_NUMBER, decimal_places=COORDINATE_DECIMAL_NUMBER)
    created_on: datetime = Field(default_factory=datetime.now)
    request_sent_to_obc_on: datetime | None = Field(default=None)
    pic_taken_on: datetime | None = Field(default=None)
    pic_transmitted_on: datetime | None = Field(default=None)
    packet_id: UUID | None = Field(foreign_key=f"{PACKET_COMMANDS_TABLE_NAME}.id", default=None)
    status: ARORequestStatus = Field(default=ARORequestStatus.PENDING)

    # table information
    metadata = TRANSACTIONAL_SCHEMA_METADATA
    __tablename__ = ARO_REQUEST_TABLE_NAME


class Commands(BaseSQLModel, table=True):
    """
    An instance of a MainCommand.
    This table holds the data related to actual commands sent from the ground station up to the OBC.
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    status: CommandStatus = Field(default=CommandStatus.PENDING)
    type_: MainTableID = Field(foreign_key=f"{MAIN_SCHEMA_NAME}.{MAIN_COMMAND_TABLE_NAME}.id")
    params: str | None = None  # Must match the corresponding params in the main command table

    # table information
    metadata = TRANSACTIONAL_SCHEMA_METADATA
    __tablename__ = COMMANDS_TABLE_NAME


class Telemetry(BaseSQLModel, table=True):
    """
    An instance of a MainTelemetry.
    This table holds the data related to actual telemetry received from the OBC.
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    type_: MainTableID = Field(foreign_key=f"{MAIN_SCHEMA_NAME}.{MAIN_TELEMETRY_TABLE_NAME}.id")
    value: str | None = None  # Must match the corresponding params in the main telemetry table

    # table information
    metadata = TRANSACTIONAL_SCHEMA_METADATA
    __tablename__ = TELEMETRY_TABLE_NAME


# Communcation session information


class CommsSession(BaseSQLModel, table=True):
    """
    Holds basic information related to a downlink/uplink transmit cycle
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    start_time: datetime = Field(unique=True)
    end_time: datetime | None = Field(unique=True, default=None)
    status: SessionStatus = Field(default=SessionStatus.PENDING)

    # table information
    metadata = TRANSACTIONAL_SCHEMA_METADATA
    __tablename__ = COMMS_SESSION_TABLE_NAME


# Raw packet data


class Packet(BaseSQLModel, table=True):
    """
    Holds the information about a raw packet
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    session_id: UUID = Field(foreign_key=f"{COMMS_SESSION_TABLE_NAME}.id")
    raw_data: str = Field(max_length=PACKET_RAW_LENGTH)
    type_: MainPacketType
    # subtype enum # CSDC requirement. TODO: Figure out what this means
    payload_data: str = Field(max_length=PACKET_DATA_LENGTH)
    created_on: datetime = Field(default_factory=datetime.now)
    offset: int

    # table information
    metadata = TRANSACTIONAL_SCHEMA_METADATA
    __tablename__ = PACKET_TABLE_NAME


class PacketTelemetry(BaseSQLModel, table=True):
    """
    Holds data about the telemetry packet
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    packet_id: UUID = Field(foreign_key=f"{PACKET_TABLE_NAME}.id")
    telemetry_id: UUID = Field(foreign_key=f"{TELEMETRY_TABLE_NAME}.id")
    previous: UUID | None = Field(foreign_key=f"{PACKET_TELEMETRY_TABLE_NAME}.id", default=None)

    # table information
    metadata = TRANSACTIONAL_SCHEMA_METADATA
    __tablename__ = PACKET_TELEMETRY_TABLE_NAME


class PacketCommands(BaseSQLModel, table=True):
    """
    Holds data about a command packet
    """

    id: UUID = Field(default_factory=uuid4, primary_key=True, index=True)
    packet_id: UUID = Field(foreign_key=f"{PACKET_TABLE_NAME}.id")
    command_id: UUID = Field(foreign_key=f"{COMMANDS_TABLE_NAME}.id")
    previous: UUID | None = Field(foreign_key=f"{PACKET_COMMANDS_TABLE_NAME}.id", default=None)

    # table information
    metadata = TRANSACTIONAL_SCHEMA_METADATA
    __tablename__ = PACKET_COMMANDS_TABLE_NAME
