from pydantic import BaseModel

from gs.backend.data.tables.main import MainCommand


class MainCommandsResponse(BaseModel):
    """
    The main commands response model.
    """

    data: list[MainCommand]
