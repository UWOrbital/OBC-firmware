from datetime import datetime
from decimal import Decimal

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.enums.aro_requests import ARORequestStatus
from gs.backend.data.tables.transactional_tables import ARORequest


def get_all_requests() -> list[ARORequest]:
    """
    @breif get all the requests from aro
    """
    with get_db_session() as session:
        requests = list(session.exec(select(ARORequest)).all())
        return requests


def add_request(
    long: Decimal,
    lat: Decimal,
    created_on: datetime,
    request_sent_obc: datetime,
    taken_date: datetime,
    transmission: datetime,
    status: ARORequestStatus,
) -> ARORequest:
    """
    @brief add a request
    """
    with get_db_session() as session:
        request = ARORequest(
            latitude=lat,
            longitude=long,
            created_on=created_on,
            request_sent_to_obc_on=request_sent_obc,
            taken_date=taken_date,
            transmission=transmission,
            status=status,
        )

        session.add(request)
        session.commit()
        session.refresh(request)
        return request


def delete_request(request_id: str) -> list[ARORequest]:
    """
    @brief delete a request based on id
    """
    with get_db_session() as session:
        request = session.exec(select(ARORequest).where(ARORequest.id == request_id)).first()

        if request:
            session.delete(request)
            session.commit()
        else:
            print("Request not found, ID does not exist")

        return get_all_requests()
