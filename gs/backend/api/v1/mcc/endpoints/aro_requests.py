from fastapi import APIRouter, Query
from sqlalchemy import desc
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
        query = select(ARORequest).order_by(desc(ARORequest.created_on))

        if filters:
            query = query.where(ARORequest.status.in_(filters))  # type: ignore
        if offset > 0:
            query = query.offset(offset)
        if count > 0:
            query = query.limit(count)

        requests = list(session.exec(query).all())

        return ARORequestsResponse(data=requests)
