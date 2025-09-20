from fastapi import APIRouter, Query

from gs.backend.data.data_wrappers.mcc_wrappers.telemetry_wrapper import get_all_telemetries
from gs.backend.data.tables.transactional_tables import Telemetry

telemetry_router = APIRouter(tags=["MCC", "Telemetry"])


@telemetry_router.get("/")
async def get_telemetry(
    count: int | None = 100, offset: int | None = 0, filters: list[int] | None = Query(default=[])
) -> list[Telemetry]:
    """ """
    telemetries = get_all_telemetries()
    telemetries.reverse()
    if filters:
        telemetries = [obj for obj in telemetries if obj.type_ in filters]
    telemetries = telemetries[offset : offset + count] if count > 0 else telemetries[offset:]
    return telemetries
