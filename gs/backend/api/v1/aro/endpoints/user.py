from uuid import UUID

from fastapi import APIRouter, HTTPException

from gs.backend.api.v1.aro.models.requests import UserRequest
from gs.backend.api.v1.aro.models.responses import UserResponse, UsersResponse
from gs.backend.data.data_wrappers.aro_wrapper.aro_user_data_wrapper import (
    add_user,
    delete_user_by_id,
    get_all_users,
    update_user_by_id,
)

aro_user_router = APIRouter(tags=["ARO", "User Information"])


@aro_user_router.get("/", response_model=UsersResponse)
async def get_users() -> UsersResponse:
    """
    Gets all users

    :return: all users
    """
    users = get_all_users()
    return UsersResponse(data=users)


@aro_user_router.post("/", response_model=UserResponse)
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


@aro_user_router.put("/{userid}", response_model=UserResponse)
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


@aro_user_router.delete("/{userid}", response_model=UsersResponse)
def delete_user(userid: str) -> UsersResponse:
    """
    Deletes a user based on the user ID
    :param userid: The unique identifier of the user to be deleted
    :return: returns the user deleted
    """
    try:
        users = delete_user_by_id(UUID(userid))
        return UsersResponse(data=users)
    except ValueError as e:
        raise HTTPException(status_code=404, detail=str(e)) from e
