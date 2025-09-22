from fastapi import APIRouter, Depends, HTTPException
from sqlmodel import Session, select, insert, delete

import gs.backend.api.v1.aro.models.requests as Request
import gs.backend.api.v1.aro.models.responses as Response
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import ARORequest, Packet
from gs.backend.data.data_wrappers.aro_wrapper.aro_request_wrapper import add_request

picture_requests_router = APIRouter(tags=["ARO", "Picture Requests"])

@picture_requests_router.get("/")
async def get_picture_requests(req: Request.PictureRequests, db_session: Session = Depends(get_db_session)) -> PictureRequestsResponse:
    """
    Gets the most recent requests.
    """
    picture_requests_query = select(ARORequest).limit(req.count)
    items = list(db_session.exec(picture_requests_query).all())
    return Response.PictureRequests(
        data = items,
        operations = Response.PictureRequests.OperationsModel(
            delete=req.url_for("delete_picture_request"),
            download=""
        )
    )

@picture_requests_router.post("/")
async def create_picture_request(req : Request.CreatePictureRequest) -> Response.CreatePictureRequest:
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
    return Response.CreatePictureRequest(
        data = picture_request,
        operations = Response.CreatePictureRequest.OperationsModel(
            get=req.url_for("get_picture_requests")
        )
    )

@picture_requests_router.get("/{id}/packet")
async def show_picture_request(id: int, db_session: Session = Depends(get_db_session)) -> Response.ShowPictureRequest:
    picture_request = db_session.get(ARORequest, id)
    if not picture_request:
        raise HTTPException(404, "Request not found")
    packet = db_session.get(Packet, picture_request.packet_id)
    if not packet:
        raise HTTPException(404, "Packet not found")
    return Response.ShowPictureRequest(
        data = packet
    )

@picture_requests_router.delete("/{id}")
async def delete_picture_request(id: int, db_session: Session = Depends(get_db_session)) -> Response.DeletePictureRequest:
    picture_request = db_session.get(ARORequest, id)
    if not picture_request:
        raise HTTPException(404, "Request not found")
    db_session.delete(picture_request)
    return Response.DeletePictureRequest()