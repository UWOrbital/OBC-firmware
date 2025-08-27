from typing import Final, TypeAlias

from sqlalchemy import Integer
from sqlalchemy.schema import MetaData
from sqlmodel import Field

from gs.backend.data.tables.base_model import BaseSQLModel

# Schema information
MAIN_SCHEMA_NAME: Final[str] = "main"
MAIN_SCHEMA_METADATA: Final[MetaData] = MetaData(MAIN_SCHEMA_NAME)

# Table names in database
MAIN_COMMAND_TABLE_NAME: Final[str] = "commands"
MAIN_TELEMETRY_TABLE_NAME: Final[str] = "telemetry"


MainTableID: TypeAlias = int
MainTableIDDatabase: TypeAlias = Integer


class MainCommand(BaseSQLModel, table=True):
    """
    Main command model.
    This table represents all the possible commands that can be issued.

    List of commands: `https://docs.google.com/spreadsheets/d/1XWXgp3--NHZ4XlxOyBYPS-M_LOU_ai-I6TcvotKhR1s/edit?gid=564815068#gid=564815068`
    """

    id: MainTableID = Field(primary_key=True, index=True)  # NOTE: Must be synced with obc_gs_command_id
    name: str
    params: str | None = None  # None if no params needed
    format: str | None = None  # None if no format needed
    data_size: int = Field(gt=0)
    total_size: int = Field(gt=0)

    # table information
    metadata = MAIN_SCHEMA_METADATA
    __tablename__ = MAIN_COMMAND_TABLE_NAME


class MainTelemetry(BaseSQLModel, table=True):
    """
    Main telemetry model.
    This table represents all the possible telemetry that can be issued.

    List of telemetry: `https://docs.google.com/spreadsheets/d/1XWXgp3--NHZ4XlxOyBYPS-M_LOU_ai-I6TcvotKhR1s/edit?gid=0#gid=0`
    """

    id: MainTableID = Field(primary_key=True, index=True)  # NOTE: Must be synced with obc_gs_telemetry_id
    name: str
    format: str | None = None  # None if no format needed
    data_size: int = Field(gt=0)
    total_size: int = Field(gt=0)

    # table information
    metadata = MAIN_SCHEMA_METADATA
    __tablename__ = MAIN_TELEMETRY_TABLE_NAME
