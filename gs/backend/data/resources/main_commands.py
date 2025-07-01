from gs.backend.data.tables.main_tables import MainCommand


# TODO: Pull this from a config file so that it's synced with the OBC
# TODO: Add more main commands
def main_commands() -> list[MainCommand]:
    """
    @brief returns the list of main commands
    """
    return [
        MainCommand(
            id=1,
            name="RTC Sync",
            params="time",
            format="int 7 bytes",
            data_size=7,
            total_size=8,
        ),
        MainCommand(
            id=2,
            name="Manually activate an emergency mode for a specified amount of time",
            params="mode_state_number,time",
            format="int 1 byte, int 7 bytes",
            data_size=8,
            total_size=9,
        ),
    ]
