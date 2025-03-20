from fastapi import APIRouter, Query
from typing import List, Optional
#from gs.backend.api.v1.mcc.models.database import get_telemetry_data -> This will be to import the telemetry data from postgre database

telemetry_router = APIRouter(tags=["MCC", "Telemetry"])


@telemetry_router.get("/")
async def getTeleData(
    count : int = 100,
    offset : int = 0,
    filters: List[str] = []
    ):
        #telemetry_data = fetch the telemetry data here

        if filters:
            telemetry_data = []
        
        #o no i was scrolling thru the task board.  
        

        #TO DO: Check if tele data length>offset

        telemetry_data = telemetry_data[offset:]
        if count>0:
              telemetry_data = telemetry_data[:count]
        return {"data" : telemetry_data}

