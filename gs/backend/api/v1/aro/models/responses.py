from pydantic import BaseModel

from gs.backend.data.tables.aro_user_tables import AROUsers


class AROUserResponse(BaseModel):
    """
    User creation response model.
    """

    data: AROUsers
