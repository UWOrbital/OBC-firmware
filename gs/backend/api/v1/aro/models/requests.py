from datetime import datetime
from decimal import Decimal
from uuid import UUID

from pydantic import BaseModel

from gs.backend.data.enums.aro_requests import ARORequestStatus


class PictureRequests(BaseModel):
    """
    Request model for fetching picture requests.

    Parameters:
        count (int): Optional, default 100. Gets the X most recent commands.
        offset (int): Optional, default 0. Starting point for paging.
    """

    count: int = 100
    offset: int = 0


class CreatePictureRequest(BaseModel):
    """
    Request model for creating picture requests.
    """

    aro_id: UUID
    long: Decimal
    lat: Decimal
    created_on: datetime
    request_sent_obc: datetime
    taken_date: datetime
    transmission: datetime
    status: ARORequestStatus
