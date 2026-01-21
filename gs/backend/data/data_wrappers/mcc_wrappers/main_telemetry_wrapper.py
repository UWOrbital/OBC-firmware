from typing import Any

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.main_tables import MainTelemetry


async def get_all_main_telemetries() -> list[MainTelemetry]:
    """
    Get all data wrapper for MainTelemetry

    :return: a list of all main_telemetries
    """
    async with get_db_session() as session:
        result = await session.exec(select(MainTelemetry))
        telemetries = list(result.all())
        return telemetries


async def create_main_telemetry(telemetry_data: dict[str, Any]) -> MainTelemetry:
    """
    Post data wrapper for MainTelemetry

    :param command_data: the JSON object of the main_telemetry to be created
    :return: the newly created main_telemetry
    """
    async with get_db_session() as session:
        telemetry = MainTelemetry(**telemetry_data)
        session.add(telemetry)
        await session.commit()
        await session.refresh(telemetry)
        return telemetry


async def delete_main_telemetry_by_id(telemetry_id: int) -> MainTelemetry:
    """
    Delete data wrapper for MainTelemetry

    :param command_id: id of main_telemetry to be deleted
    :return: the deleted main_telemetry
    """
    async with get_db_session() as session:
        telemetry = await session.get(MainTelemetry, telemetry_id)
        if not telemetry:
            raise ValueError("Main telemetry not found.")
        session.delete(telemetry)
        await session.commit()
        return telemetry
