from uuid import UUID

from fastapi import APIRouter, HTTPException

from gs.backend.api.v1.aro.models.requests import UserRequest
from gs.backend.api.v1.aro.models.responses import AllUsersResponse, UserResponse
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_data_wrapper import (
    add_user,
    delete_user_by_id,
    update_user_by_id,
)
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_data_wrapper import get_all_users as get_all_db_users

aro_user_router = APIRouter(tags=["ARO", "User Information"])


@aro_user_router.get("/get_all_users", response_model=AllUsersResponse)
async def get_all_users() -> AllUsersResponse:
    """
    Gets all users

    :return: all users
    """
    users = get_all_db_users()
    return AllUsersResponse(data=users)


@aro_user_router.post("/create_user", response_model=UserResponse)
def create_user(payload: UserRequest) -> UserResponse:
    """
    Creates a user with the given payload
    :param payload: The data used to create a user
    :return: returns the user created
    """

    user = add_user(
        call_sign=payload.call_sign,
        email=payload.email,
        f_name=payload.first_name,
        l_name=payload.last_name,
        phone_number=payload.phone_number,
    )

    return UserResponse(data=user)


@aro_user_router.put("/update_user/{userid}", response_model=UserResponse)
def update_user(userid: str, payload: UserRequest) -> UserResponse:
    """
    Modifies the user’s info based on the payload
    :param payload: The data used to update a user
    :return: returns the user updated
    """
    try:
        user = update_user_by_id(
            userid=UUID(userid),
            call_sign=payload.call_sign,
            email=payload.email,
            f_name=payload.first_name,
            l_name=payload.last_name,
            phone_number=payload.phone_number,
        )
        return UserResponse(data=user)
    except ValueError as e:
        raise HTTPException(status_code=404, detail=str(e)) from e


@aro_user_router.delete("/delete_user/{userid}", response_model=AllUsersResponse)
def delete_user(userid: str) -> AllUsersResponse:
    """
    Deletes a user based on the user ID
    :param userid: The unique identifier of the user to be deleted
    :return: returns the user deleted
    """
    try:
        users = delete_user_by_id(UUID(userid))
        return AllUsersResponse(data=users)
    except ValueError as e:
        raise HTTPException(status_code=404, detail=str(e)) from e
