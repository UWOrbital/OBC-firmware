import csv

from gs.backend.data.tables.main_tables import MainCommand


# TODO: Pull this from a config file so that it's synced with the OBC
def main_commands() -> list[MainCommand]:
    """
    Returns the list of main commands
    """
    with open("gs/backend/data/resources/main_commands.csv", newline="") as csvfile:
        file = csv.reader(csvfile)
        return [
            MainCommand(
                id=int(row[0]),
                name=row[1],
                params=row[2],
                format=row[3],
                data_size=int(row[4]),
                total_size=int(row[5]),
            )
            for row in file
        ]
