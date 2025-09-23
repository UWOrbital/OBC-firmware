from fastapi import APIRouter, Request, HTTPException
from gs.backend.data.data_wrappers.mcc_wrappers.commands_wrapper import get_all_commands, create_commands, delete_commands_by_id

commands_router = APIRouter(tags=["MCC", "Commands"])

'''
IMPORTANT: Get endpoint is not properly implemented. 
'''
@commands_router.get("/")
async def get_commands(request: Request, count: int = 100, offset: int = 0):
    # Implementation to retrieve the most recent commands
    commands = get_all_commands()  # Placeholder for actual data retrieva

    return {
        "data": commands[offset:offset + count],
        "operations": {
            "delete": request.url_for("delete_command", id="{id}")  # Dynamic URL generation
        },
        "datetime": "placeholder_for_datetime"  # Placeholder for actual datetime
    }



# Post
@commands_router.post("/")
async def create_command(payload: dict):
    # Implementation to create a new command
    commands = get_all_commands()  # Placeholder for actual data retrieval
    #check if payload is in commands
    if payload in commands:
        raise HTTPException(status_code=400, detail="Invalid command payload")
    return create_commands(payload)

# Delete
@commands_router.delete("/{id}")
async def delete_command(id: int):
    # Implementation to delete a command by id
    commands = get_all_commands()  # Placeholder for actual data retrieval
    command_to_delete = next((cmd for cmd in commands if cmd["id"] == id), None)

    if not command_to_delete:
        raise FileNotFoundError(f"Command with id {id} not found")
    
    delete_commands_by_id(id)
    return {"message": f"Command with id {id} deleted successfully"}