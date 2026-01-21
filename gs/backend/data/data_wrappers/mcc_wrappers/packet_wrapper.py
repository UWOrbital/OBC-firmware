from typing import Any
from uuid import UUID

from sqlmodel import select

from gs.backend.data.database.engine import get_db_session
from gs.backend.data.tables.transactional_tables import Packet


async def get_all_packets() -> list[Packet]:
    """
    Get all data wrapper for Packet

    :return: a list of all packets
    """
    async with get_db_session() as session:
        result = await session.exec(select(Packet))
        packets = list(result.all())
        return packets


async def create_packet(packet_data: dict[str, Any]) -> Packet:
    """
    Post data wrapper for Packet

    :param packet_data: the JSON object of the packet to be created
    :return: the newly created packet
    """
    async with get_db_session() as session:
        packet = Packet(**packet_data)
        session.add(packet)
        await session.commit()
        await session.refresh(packet)
        return packet


async def delete_packet_by_id(packet_id: UUID) -> Packet:
    """
    Delete data wrapper for Packet

    :param packet_id: UUID of packet to be deleted
    :return: the deleted packet
    """
    async with get_db_session() as session:
        packet = await session.get(Packet, packet_id)
        if not packet:
            raise ValueError("Packet not found.")
        session.delete(packet)
        await session.commit()
        return packet
