from datetime import datetime

from fastapi import APIRouter
from pydantic import BaseModel

logs_router = APIRouter(tags=["Logs"])


class LogItem(BaseModel):
    """
    Represents a log item with date and message.

    Attributes:
        date (datetime): Time that the messaged was logged.
        log (str): Information that was logged
    """

    date: datetime
    log: str


# Example hardcoded logs
hardcoded_logs = [
    {"date": "2025-09-28 10:05:00", "log": "ARO request received."},
    {"date": "2025-09-28 10:10:00", "log": "Mission Command request received"},
]


@logs_router.get("/", response_model=list[LogItem])
async def get_recent_logs() -> list[LogItem]:
    """
    Gets the hardcoded test logs for MCC frontend testing

    :return: list of logs
    """
    return hardcoded_logs
