from sqlalchemy.ext.asyncio import AsyncSession
from sqlmodel import select

from gs.backend.data.resources.callsigns import callsigns
from gs.backend.data.resources.main_commands import main_commands
from gs.backend.data.resources.main_telemetry import main_telemetry
from gs.backend.data.tables.aro_user_tables import AROUserCallsigns
from gs.backend.data.tables.main_tables import MainCommand, MainTelemetry


async def add_main_commands(session: AsyncSession) -> None:
    """
    Setup the main commands to the database
    """
    query = select(MainCommand).limit(1)  # Check if the db is empty
    result = await session.execute(query)
    if not result.scalars().first():
        session.add_all(main_commands())
        await session.commit()


async def add_callsigns(session: AsyncSession) -> None:
    """
    Setup the valid callsigns to the database
    """
    query = select(AROUserCallsigns).limit(1)
    result = await session.execute(query)
    if not result.scalars().first():
        session.add_all(callsigns())
        await session.commit()


async def add_telemetry(session: AsyncSession) -> None:
    """
    Setup the main telemetry to the database
    """
    query = select(MainTelemetry).limit(1)  # Check if the db is empty
    result = await session.execute(query)
    if not result.scalars().first():
        session.add_all(main_telemetry())
        await session.commit()
