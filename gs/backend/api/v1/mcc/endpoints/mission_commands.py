from fastapi import APIRouter
from pydantic import BaseModel

mission_commands_router = APIRouter(tags=["Mission Control"])


class MissionCommandRequest(BaseModel):
    """
    Represents a body for a mission command request

    Attributes:
        command (str): Title of the command request.
    """

    command: str


class MissionCommandResponse(BaseModel):
    """
    Represents a body for a mission command response

    Attributes:
        response (str): Response message.
    """

    response: str


@mission_commands_router.post("/", response_model=MissionCommandResponse)
async def execute_mission_command(cmd: MissionCommandRequest) -> MissionCommandResponse:
    """
    Gets the hardcoded mission command response for MCC frontend testing

    :return: mission command respone
    """
    return MissionCommandResponse(response=f"Executed command: {cmd.command}")
