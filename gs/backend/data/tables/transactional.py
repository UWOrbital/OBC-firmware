from datetime import datetime
from decimal import Decimal
from typing import Final

from sqlmodel import Field, Uuid

from gs.backend.data.enums.aro_requests import AroRequestStatus
from gs.backend.data.tables.base_model import BaseSQLModel

ARO_REQUEST_TABLE_NAME: Final[str] = "aro_requests"


class ARORequest(BaseSQLModel, table=True):
    """
    Holds the data related to an ARO picture request
    """

    id: Uuid = Field(primary_key=True, index=True)
    aro_id: Uuid = Field(foreign_key="users.id")
    latitude: Decimal
    longitude: Decimal
    created_on: datetime = Field(default_factory=datetime.now)
    request_sent_to_obc_on: datetime | None = Field(default=None)
    pic_taken_on: datetime | None = Field(default=None)
    pic_transmitted_on: datetime | None = Field(default=None)
    packet_id: Uuid | None = Field(foreign_key="packet_commands.id", default=None)
    status: AroRequestStatus = Field(default=AroRequestStatus.PENDING)
    __tablename__ = ARO_REQUEST_TABLE_NAME
