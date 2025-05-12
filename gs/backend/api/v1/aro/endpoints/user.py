from uuid import UUID

from fastapi import APIRouter, Depends, Request
from sqlmodel import Session, or_, select

from gs.backend.api.v1.aro.models.requests import AROUserUpdateRequest, UserCreateRequest
from gs.backend.api.v1.aro.models.responses import AROUserResponse
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.aro_user_tables import AROUsers
from gs.backend.exceptions.exceptions import InvalidArgumentError, InvalidStateError

aro_user_router = APIRouter(tags=["ARO", "User Information"])


@aro_user_router.post("/")
def create_user(payload: UserCreateRequest, db_session: Session = Depends(get_db_session)) -> AROUserResponse:
    """
    Creates a user based on the data.
    Checks makes sure the email, phone number and callsign are unique before creating a user.

    @param payload (UserCreateRequest) - The data with which to create the user
    @return the provided data along with the generated user id
    """
    user_query = select(AROUsers).where(or_(AROUsers.email == payload.email, AROUsers.call_sign == payload.call_sign))
    user = db_session.exec(user_query).first()
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
    return AROUserResponse(data=user_model)


@aro_user_router.get("/")
def get_current_user(request: Request, db_session: Session = Depends(get_db_session)) -> AROUserResponse:
    """
    @brief Gets the current user information for the logged in user.

    @return The current user's data
    """
    # TODO: Switch to a better way of storing the current user once we implement auth
    user_id: UUID | None = request.app.state.user_id

    if user_id is None or not isinstance(user_id, UUID):
        raise InvalidStateError(f"No user logged in or {user_id=} is invalid")

    user_query = select(AROUsers).where(AROUsers.id == user_id)
    users = db_session.exec(user_query).all()

    if len(users) == 0:
        raise InvalidStateError(f"No users match the given id={user_id}")
    if len(users) > 1:
        raise InvalidStateError(f"Multiple users match the given id={user_id}")

    current_user = users[0]
    return AROUserResponse(data=current_user)


@aro_user_router.put("/")
def update_current_user(
    payload: AROUserUpdateRequest, request: Request, db_session: Session = Depends(get_db_session)
) -> AROUserResponse:
    """
    @brief Updates the current user info based on the payload

    @param payload (AROUserUpdateRequest) - The information to update for the user
    @return Returns the updated information
    """
    # TODO: Switch to a better way of storing the current user once we implement auth
    user_id: UUID | None = request.app.state.user_id

    if user_id is None or not isinstance(user_id, UUID):
        raise InvalidStateError(f"No user logged in or {user_id=} is invalid")

    user_query = select(AROUsers).where(AROUsers.id == user_id)
    users = db_session.exec(user_query).all()

    if len(users) == 0:
        raise InvalidStateError(f"No users match the given id={user_id}")
    if len(users) > 1:
        raise InvalidStateError(f"Multiple users match the given id={user_id}")

    current_user = users[0]
    current_user.call_sign = payload.call_sign
    current_user.first_name = payload.first_name
    current_user.last_name = payload.last_name
    current_user.phone_number = payload.phone_number

    db_session.add(current_user)
    db_session.commit()
    db_session.refresh(current_user)
    return AROUserResponse(data=current_user)
