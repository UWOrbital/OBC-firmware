"""
Background
We need a way to retrieve ARO requests in our MCC.

Requirements
Create the following endpoints in gs/backend/api/v1/mcc/endpoints/aro_requests.py

Get /
Returns the aro requests. The response should be under the data field.

Parameters:

count
type: int
optional with default: 100
Gets the count most recent commands
If count ≤ 0, return all data
offset
type: int
optional with default of 0
Starting point to get the most recent commands at (used for paging)
filters
type: list of aro request type
Returns the aro requests that match any if the request types
If the list is empty, apply no filtering
"""

from fastapi import APIRouter, Query
from sqlmodel import select

from gs.backend.api.v1.mcc.models.responses import ARORequestsResponse
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_requests import ARORequestStatus
from gs.backend.data.tables.transactional_tables import ARORequest

aro_requests_router = APIRouter(tags=["MCC", "ARO Requests"])


@aro_requests_router.get("/", response_model=ARORequestsResponse)
async def get_aro_requests(
    count: int = Query(default=100),
    offset: int = Query(default=0),
    filters: list[ARORequestStatus] = Query(default=[]),
) -> ARORequestsResponse:
    """
    Gets all ARO requests with optional filtering and pagination

    :param count: Number of most recent requests to return. If ≤ 0, returns all data
    :param offset: Starting point for paging
    :param filters: List of request statuses to filter by. If empty, no filtering is applied
    :return: ARO requests matching the criteria
    """
    with get_db_session() as session:
        query = select(ARORequest).order_by(ARORequest.created_on.desc())

        if filters:
            query = query.where(ARORequest.status.in_(filters))
        if offset > 0:
            query = query.offset(offset)
        if count > 0:
            query = query.limit(count)

        requests = list(session.exec(query).all())

        return ARORequestsResponse(data=requests)
