import asyncio
import sys

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.resources.utils import add_callsigns, add_main_commands, add_telemetry

"""
To migrate pre-determined datainto your local database,
you can run `python3 gs/backend/migrate.py` from the top
level directory.

Alternatively, you can include `callsigns`, `commands`, or 'telemetries'
as command arguments to migrate those respective datasets
individually.
"""


async def main() -> None:
    """Main async function to run migrations"""
    if len(sys.argv) > 2:
        raise ValueError(f"Invalid input. Expected at most 1 argument, received {len(sys.argv)}")
    elif len(sys.argv[1:]) == 0:
        async with get_db_session() as session:
            print("Migrating callsign data...")
            await add_callsigns(session)
            print("Migrating main command data...")
            await add_main_commands(session)
            print("Migrating telemetry data...")
            await add_telemetry(session)
    else:
        match sys.argv[1]:
            case "callsigns":
                async with get_db_session() as session:
                    print("Migrating callsign data...")
                    await add_callsigns(session)
            case "commands":
                async with get_db_session() as session:
                    print("Migrating main command data...")
                    await add_main_commands(session)
            case "telemetries":
                async with get_db_session() as session:
                    print("Migrating telemetry data...")
                    await add_telemetry(session)
            case _:
                raise ValueError("Invalid input. Optional arguments include 'callsigns', 'commands', or 'telemetries'.")


if __name__ == "__main__":
    asyncio.run(main())
