from pydantic import BaseModel

from gs.backend.data.tables.transactional_tables import ARORequest, Packet


class PictureRequests(BaseModel):
    """ """

    class OperationsModel(BaseModel):
        delete: str | None
        download: str | None

    data: list[ARORequest]
    operations: OperationsModel


class CreatePictureRequest(BaseModel):
    """ """

    class OperationsModel(BaseModel):
        get: str | None

    data: ARORequest
    operations: OperationsModel


class ShowPictureRequest(BaseModel):
    """ """

    data: Packet


class DeletePictureRequest(BaseModel):
    """ """

    pass
