from fastapi import APIRouter, Depends, HTTPException
from sqlmodel import Session, select

from gs.backend.api.v1.aro.models import requests, responses
from gs.backend.data.data_wrappers.aro_wrapper.aro_request_wrapper import add_request
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import ARORequest, Packet

picture_requests_router = APIRouter(tags=["ARO", "Picture Requests"])


@picture_requests_router.get("/")
async def get_picture_requests(
    req: requests.PictureRequests, db_session: Session = Depends(get_db_session)
) -> responses.PictureRequests:
    """
    Gets the most recent picture requests.
    """
    picture_requests_query = select(ARORequest).limit(req.count).offset(req.offset)
    items = list(db_session.exec(picture_requests_query).all())
    return responses.PictureRequests(
        data=items,
        operations=responses.PictureRequests.OperationsModel(delete=req.url_for("delete_picture_request"), download=""),
    )


@picture_requests_router.post("/")
async def create_picture_request(req: requests.CreatePictureRequest) -> responses.CreatePictureRequest:
    """
    Creates a picture request.
    """
    picture_request = add_request(
        aro_id=req.aro_id,
        latitude=req.lat,
        longitude=req.long,
        created_on=req.created_on,
        request_sent_to_obc_on=req.request_sent_obc,
        taken_date=req.taken_date,
        transmission=req.transmission,
        status=req.status,
    )
    return responses.CreatePictureRequest(
        data=picture_request,
        operations=responses.CreatePictureRequest.OperationsModel(get=req.url_for("get_picture_requests")),
    )


@picture_requests_router.get("/{req_id}/packet")
async def show_picture_request(
    req_id: int, db_session: Session = Depends(get_db_session)
) -> responses.ShowPictureRequest:
    """
    Get a picture request by id.
    """
    picture_request = db_session.get(ARORequest, req_id)
    if not picture_request:
        raise HTTPException(404, "Request not found")
    packet = db_session.get(Packet, picture_request.packet_id)
    if not packet:
        raise HTTPException(404, "Packet not found")
    return responses.ShowPictureRequest(data=packet)


@picture_requests_router.delete("/{req_id}")
async def delete_picture_request(
    req_id: int, db_session: Session = Depends(get_db_session)
) -> responses.DeletePictureRequest:
    """
    Delete a picture request by id.
    """
    picture_request = db_session.get(ARORequest, req_id)
    if not picture_request:
        raise HTTPException(404, "Request not found")
    db_session.delete(picture_request)
    return responses.DeletePictureRequest()
