from typing import Any
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import PacketTelemetry


async def get_all_packet_telemetries() -> list[PacketTelemetry]:
    """
    Get all data wrapper for PacketTelemetry

    :return: a list of all packet_telemetries
    """
    async with get_db_session() as session:
        result = await session.exec(select(PacketTelemetry))
        telemetries = list(result.all())
        return telemetries


async def create_packet_telemetry(telemetry_data: dict[str, Any]) -> PacketTelemetry:
    """
    Post data wrapper for PacketTelemetry

    :param command_data: the JSON object of the packet_telemetry to be created
    :return: the newly created packet_telemetry
    """
    async with get_db_session() as session:
        telemetry = PacketTelemetry(**telemetry_data)
        session.add(telemetry)
        await session.commit()
        await session.refresh(telemetry)
        return telemetry


async def delete_packet_telemetry_by_id(telemetry_id: UUID) -> PacketTelemetry:
    """
    Delete data wrapper for PacketTelemetry

    :param command_id: UUID of packet_telemetry to be deleted
    :return: the deleted packet_telemetry
    """
    async with get_db_session() as session:
        telemetry = await session.get(PacketTelemetry, telemetry_id)
        if not telemetry:
            raise ValueError("Packet telemetry not found.")
        session.delete(telemetry)
        await session.commit()
        return telemetry
