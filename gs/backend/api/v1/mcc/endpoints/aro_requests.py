from fastapi import APIRouter
from pydantic import BaseModel

aro_requests_router = APIRouter(tags=["MCC", "ARO Requests"])


class AROItem(BaseModel):
    """
    Represents an ARO request item with location and status.

    Attributes:
        id (int): Unique identifier of the ARO request.
        latitude (float): Latitude of the request location.
        longitude (float): Longitude of the request location.
        status (str): Status of the request.
    """

    id: int
    latitude: float
    longitude: float
    status: str


hardcoded_aro_requests = [
    {"id": 1, "latitude": 43.468, "longitude": -80.540, "status": "pending"},
    {"id": 2, "latitude": 43.471, "longitude": -80.544, "status": "complete"},
    {"id": 3, "latitude": 43.473, "longitude": -80.539, "status": "in-progress"},
]


@aro_requests_router.get("/", response_model=list[AROItem])
async def get_aro_requests() -> list[AROItem]:
    """
    Retrieve the hardcoded ARO requests for MCC frontend testing.

    Returns:
        List[AROItem]: A list of all hardcoded ARO requests.
    """
    return hardcoded_aro_requests
