from fastapi import APIRouter, Depends
from sqlmodel import Session, select

from gs.backend.api.v1.mcc.models.responses import MainCommandsResponse
from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.main_tables import MainCommand

main_commands_router = APIRouter(tags=["MCC", "Main Commands"])


@main_commands_router.get("/")
async def get_main_commands(db_session: Session = Depends(get_db_session)) -> MainCommandsResponse:
    """
    @brief Gets the main commands that are available for the MCC
    """
    main_commands_query = select(MainCommand)
    items = list(db_session.exec(main_commands_query).all())
    return MainCommandsResponse(data=items)
