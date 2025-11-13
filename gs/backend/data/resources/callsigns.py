import csv

from gs.backend.data.tables.aro_user_tables import AROUserCallsigns


def callsigns() -> list[AROUserCallsigns]:
    """
    Returns the list of callsigns
    """
    with open("gs/backend/data/resources/callsigns.csv", newline="") as csvfile:
        file = csv.reader(csvfile)
        return [AROUserCallsigns(call_sign=row[0]) for row in file]
