from fastapi import APIRouter, Depends, HTTPException
from sqlmodel import Session, select

from gs.backend.api.v1.aro.models.requests import AROUsersRequest
from gs.backend.data.tables.aro_user_tables import AROUsers
from gs.backend.api.v1.aro.models.responses import AROUsersListResponse, AROUsersSingleResponse
from gs.backend.data.database.engine import get_db_session


aro_user_router = APIRouter(tags=["ARO", "User Information"])

@aro_user_router.get("/", response_model=AROUsersListResponse)
def get_users(db: Session = Depends(get_db_session)):
    """
    Gets user data

    :return: Returns user list under the data field
    """
    query = select(AROUsers)
    users = db.exec(query).all()
    return {"data": users}

@aro_user_router.post("/", response_model=AROUsersSingleResponse)
def create_user(payload: AROUsersRequest, db: Session = Depends(get_db_session)):
    """
    Creates a user with the given payload in the database and returns this payload after pulling it from the database 

    :param payload: The data used to create a user
    :return: returns a json object with field of "data" under which there is the payload now pulled from the database 
    """
    user = AROUsers(**payload.dict())
    db.add(user)
    db.commit()
    db.refresh(user)

    return {"data": user} 

@aro_user_router.put("/{id}", response_model=AROUsersSingleResponse)
def edit_user(id: str, payload: AROUsersRequest, db: Session = Depends(get_db_session)):
    """
    Modifies the current user’s info based on the payload

    :param payload: The data used to update a user
    :return: returns a json object with field of "data" under which there is the payload now pulled from the database 
    """
    user = db.get(AROUsers, id)
    if not user:
        raise HTTPException(status_code=404, detail=f"User with id {id} not found")
    
    for key, value in payload.dict().items():
        setattr(user, key, value)

    db.commit()
    db.refresh(user)  # refresh to get updated values

    return {"data": user}