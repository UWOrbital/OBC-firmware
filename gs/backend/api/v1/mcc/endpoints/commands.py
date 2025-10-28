from typing import Any
from uuid import UUID

from fastapi import APIRouter, HTTPException

from gs.backend.data.data_wrappers.mcc_wrappers.commands_wrapper import (
    create_commands,
    delete_commands_by_id,
    get_all_commands,
)
from gs.backend.data.tables.transactional_tables import Commands

commands_router = APIRouter(tags=["MCC", "Commands"])


@commands_router.post("/")
async def create_command(payload: dict[str, Any]) -> Commands:
    """
    Create a new command.

    :param payload: The data used to create a command
    :return: returns the created command object
    """
    commands = get_all_commands()

    # Check for duplicate commands by comparing business fields (excluding auto-generated id)
    if any({k: v for k, v in cmd.model_dump().items() if k != "id"} == payload for cmd in commands):
        raise HTTPException(status_code=400, detail="Invalid command payload")

    return create_commands(payload)


@commands_router.delete("/{command_id}")
async def delete_command(command_id: UUID) -> dict[str, Any]:
    """
    Delete a command by ID.

    :param command_id: The id which is to be deleted.
    :return: returns a dict giving confirmation that command with id of command_id has been deleted.
    """
    commands = get_all_commands()
    command_to_delete = next((cmd for cmd in commands if cmd.id == command_id), None)

    if not command_to_delete:
        raise FileNotFoundError(f"Command with id {command_id} not found")

    delete_commands_by_id(UUID(str(command_id)))
    return {"message": f"Command with id {command_id} deleted successfully"}
