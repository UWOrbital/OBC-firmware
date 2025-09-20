from pydantic import BaseModel

from gs.backend.data.tables.aro_user_tables import AROUsers


class AROUsersListResponse(BaseModel):
    """
    List of all commands
    """

    data: list[AROUsers]


class AROUsersSingleResponse(BaseModel):
    """
    Single command 
    """
    data: AROUsers
