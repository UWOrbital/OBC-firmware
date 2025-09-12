from fastapi import APIRouter

from gs.backend.api.v1.mcc.models.responses import MainCommandsResponse
from gs.backend.data.data_wrappers.mcc_wrappers.main_command_wrapper import get_all_main_commands

main_commands_router = APIRouter(tags=["MCC", "Main Commands"])


@main_commands_router.get("/")
async def get_main_commands() -> MainCommandsResponse:
    """
    @brief Gets the main commands that are available for the MCC
    """
    items = get_all_main_commands()
    return MainCommandsResponse(data=items)
