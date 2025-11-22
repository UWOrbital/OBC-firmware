import sys

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.resources.utils import add_callsigns, add_main_commands

if __name__ == "__main__":
    if len(sys.argv) > 2:
        print("Invalid input. ")
    elif len(sys.argv[1:]) == 0:
        print("Migrating callsign data...")
        add_callsigns(get_db_session())
        print("Migrating main command data...")
        add_main_commands(get_db_session())
    elif sys.argv[1] == "callsigns":
        print("Migrating callsign data...")
        add_callsigns(get_db_session())
    elif sys.argv[1] == "commands":
        print("Migrating main command data...")
        add_main_commands(get_db_session())
    else:
        print("Invalid input. Optional arguments include 'callsigns' or 'commands'.")
