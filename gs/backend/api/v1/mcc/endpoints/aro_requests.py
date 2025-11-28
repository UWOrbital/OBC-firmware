from fastapi import APIRouter, Query

from gs.backend.api.v1.mcc.models.responses import ARORequestsResponse
from gs.backend.data.data_wrappers.aro_wrapper.aro_request_wrapper import get_all_requests
from gs.backend.data.enums.aro_requests import ARORequestStatus

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
    requests = get_all_requests(count, offset, filters)
    return ARORequestsResponse(data=requests)
