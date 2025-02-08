from typing import Final
from uuid import UUID

from sqlmodel import Field

from gs.backend.data.tables.base_model import BaseSQLModel

MAIN_COMMAND_TABLE_NAME: Final[str] = "master_commands"
MAIN_TELEMETRY_TABLE_NAME: Final[str] = "master_telemetry"


class MainCommand(BaseSQLModel, table=True):
    """
    Main command model.
    This table represents all the possible commands that can be issued.

    List of commands: https://docs.google.com/spreadsheets/d/1XWXgp3--NHZ4XlxOyBYPS-M_LOU_ai-I6TcvotKhR1s/edit?gid=564815068#gid=564815068
    """

    id: UUID = Field(primary_key=True, index=True)
    name: str
    params: str | None = None
    format: str | None = None
    data_size: int
    total_size: int
    __tablename__ = MAIN_COMMAND_TABLE_NAME


class MainTelemetry(BaseSQLModel, table=True):
    """
    Main telemetry model.
    This table represents all the possible telemetry that can be issued.

    List of telemetry: https://docs.google.com/spreadsheets/d/1XWXgp3--NHZ4XlxOyBYPS-M_LOU_ai-I6TcvotKhR1s/edit?gid=0#gid=0
    """

    id: UUID = Field(primary_key=True, index=True)
    name: str
    format: str | None = None
    data_size: int
    total_size: int
    __tablename__ = MAIN_TELEMETRY_TABLE_NAME
