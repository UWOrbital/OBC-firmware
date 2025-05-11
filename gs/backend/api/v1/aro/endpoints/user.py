from fastapi import APIRouter, Depends
from sqlmodel import Session, or_, select

from gs.backend.api.v1.aro.models.requests import UserCreateRequest
from gs.backend.api.v1.aro.models.responses import UserCreateResponse
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.aro_user_tables import AROUsers
from gs.backend.exceptions.exceptions import InvalidArgumentError

aro_user_router = APIRouter(tags=["ARO", "User Information"])


@aro_user_router.post("/")
def create_user(payload: UserCreateRequest, db_session: Session = Depends(get_db_session)) -> UserCreateResponse:
    """
    Creates a user based on the data.
    Checks makes sure the email, phone number and callsign are unique before creating a user.

    @param payload (UserCreateRequest) - The data with which to create the user
    @return the provided data along with the generated user id
    """
    user_query = select(AROUsers).where(or_(AROUsers.email == payload.email, AROUsers.call_sign == payload.call_sign))
    user = db_session.exec(user_query).first()
    print(f"{user = }")
    if user is not None:
        raise InvalidArgumentError(
            f"""
            Cannot create user with email={payload.email} or call_sign={payload.call_sign} as one of them is already
            used
            """
        )
    user_model = AROUsers(**payload.model_dump())
    db_session.add(user_model)
    db_session.commit()
    db_session.refresh(user_model)
    return UserCreateResponse(data=user_model)
