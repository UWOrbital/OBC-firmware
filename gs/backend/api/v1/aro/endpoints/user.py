from fastapi import APIRouter, HTTPException, status

from gs.backend.api.v1.aro.models.requests import ChangeUserInfo, CreateUser, GetUserData
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_data_wrapper import add_user, get_user_by_id, modify_user
from gs.backend.data.tables.aro_user_tables import AROUsers

aro_user_router = APIRouter(tags=["ARO", "User Information"])


@aro_user_router.post("/create_user/", status_code=status.HTTP_200_OK)
def create_user(payload: CreateUser) -> dict[str, AROUsers]:
    """
    :param payload: Payload of type CreateUser, contains first_name, last_name,
      call_sign, email, phone numer
      all of type str
    """
    try:
        user = add_user(
            call_sign=payload.call_sign,
            email=payload.email,
            f_name=payload.first_name,
            l_name=payload.last_name,
            phone_number=payload.phone_number,
        )
    except ValueError as e:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=f"Invalid input: {e}") from e

    return {"user": user}


@aro_user_router.get("/", status_code=status.HTTP_200_OK)
def get_user(payload: GetUserData) -> dict[str, AROUsers]:
    """
    :param payload: Payload of type GetUserData, contains user_id of type str
    """
    try:
        user_id = payload.user_id

    except KeyError as e:
        raise HTTPException(status_code=status.HTTP_422_UNPROCESSABLE_ENTITY, detail="Missing user_id") from e
    try:
        user = get_user_by_id(user_id)
        return {"user": user}

    except ValueError as e:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail=f"User does not exist, {e}") from e


@aro_user_router.put("/", status_code=status.HTTP_200_OK)
def change_user_info(payload: ChangeUserInfo) -> dict[str, AROUsers]:
    """
    :params payload: Payload of type ChangeUserInfo, contains user_id of type UUID, first_name,
      last_name, call_sign
    """
    try:
        user_id = payload.user_id

    except KeyError as e:
        raise HTTPException(status_code=status.HTTP_422_UNPROCESSABLE_ENTITY, detail="Missing user_id") from e

    allowed_fields = {"first_name", "last_name", "call_sign"}
    update_fields = {k: v for k, v in payload.model_dump().items() if v is not None and k in allowed_fields}

    try:
        return {"data": modify_user(userid=user_id, **update_fields)}

    except ValueError as e:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail=str(e)) from e
