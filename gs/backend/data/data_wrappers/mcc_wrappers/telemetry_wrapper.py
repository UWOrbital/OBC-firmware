from typing import Any
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import Telemetry


async def get_all_telemetries() -> list[Telemetry]:
    """
    Get all data wrapper for Telemetry

    :return: a list of all telemetries
    """
    async with get_db_session() as session:
        result = await session.execute(select(Telemetry))
        telemetries = list(result.scalars().all())
        return telemetries


async def create_telemetry(telemetry_data: dict[str, Any]) -> Telemetry:
    """
    Post data wrapper for Telemetry

    :param telemetry_data: the JSON object of the telemetry to be created
    :return: the newly created telemetry
    """
    async with get_db_session() as session:
        telemetry = Telemetry(**telemetry_data)
        session.add(telemetry)
        await session.commit()
        await session.refresh(telemetry)
        return telemetry


async def delete_telemetry_by_id(telemetry_id: UUID) -> Telemetry:
    """
    Delete data wrapper for Telemetry

    :param telemetry_id: UUID of telemetry to be deleted
    :return: the deleted telemetry
    """
    async with get_db_session() as session:
        telemetry = await session.get(Telemetry, telemetry_id)
        if not telemetry:
            raise ValueError("Telemetry not found.")
        session.delete(telemetry)
        await session.commit()
        return telemetry
