from datetime import datetime
from decimal import Decimal
from uuid import UUID

from sqlalchemy import desc
from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_requests import ARORequestStatus
from gs.backend.data.tables.transactional_tables import ARORequest


def get_all_requests(
    count: int = 100, offset: int = 0, filters: list[ARORequestStatus] | None = None
) -> list[ARORequest]:
    """
    Get all the requests from aro

    :param count: Number of most recent requests to return. If ≤ 0, returns all data
    :param offset: Starting point for paging
    :param filters: List of request statuses to filter by. If empty, no filtering is applied
    """

    if filters is None:
        filters = []

    with get_db_session() as session:
        query = select(ARORequest).order_by(desc(ARORequest.created_on))  # type: ignore

        if filters:
            query = query.where(ARORequest.status.in_(filters))  # type: ignore
        if offset > 0:
            query = query.offset(offset)
        if count > 0:
            query = query.limit(count)

        requests = list(session.exec(query).all())

        return requests


def add_request(
    aro_id: UUID,
    long: Decimal,
    lat: Decimal,
    created_on: datetime,
    request_sent_obc: datetime,
    taken_date: datetime,
    transmission: datetime,
    status: ARORequestStatus,
) -> ARORequest:
    """
    Add a request

    :param long: the longitude represented as a decimal of max 3 decimal places
    :param lat: the latitude represented as a decimal of max 3 decimal places
    :param created_on: datetime object representing the date this request was made. defaults to now
    :param request_sent_obc: datetime object representing when the request was sent to OBC
    :param taken_date: datetime object representing the date that this picture was taken on
    :param transmission: datetime object representing the date that this picture was transmitted
    :param status: the status of the request, can only be from the requets in ARORequestStatus
    """
    with get_db_session() as session:
        request = ARORequest(
            aro_id=aro_id,
            latitude=lat,
            longitude=long,
            created_on=created_on,
            request_sent_to_obc_on=request_sent_obc,
            pic_taken_on=taken_date,
            pic_transmitted_on=transmission,
            status=status,
        )

        session.add(request)
        session.commit()
        session.refresh(request)
        return request


def delete_request_by_id(request_id: str) -> list[ARORequest]:
    """
    Delete a request based on id

    :param request_id: unique identifier of the request
    """
    with get_db_session() as session:
        request = session.get(ARORequest, request_id)
        if request:
            session.delete(request)
            session.commit()
        else:
            raise ValueError("Request not found, ID does not exist")

        return get_all_requests()
