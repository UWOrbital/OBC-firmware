from fastapi import APIRouter
from fastapi_cache.decorator import cache

from gs.backend.api.v1.mcc.models.responses import MainCommandsResponse
from gs.backend.data.data_wrappers.wrappers import MainCommandWrapper

main_commands_router = APIRouter(tags=["MCC", "Main Commands"])


@main_commands_router.get("/")
@cache(expire=300)  # Cache for 5 minutes
async def get_main_commands() -> MainCommandsResponse:
    """
    Gets the main commands that are available for the MCC

    :return: list of all commands
    """
    items = MainCommandWrapper().get_all()
    return MainCommandsResponse(data=items)
