import sgp4 as _sgp4
import sgp4.api as _sgp4API
from datetime import datetime
from sgp4.api import Satrec, jday, WGS72, SGP4_ERRORS
from backend.positioning.tle import TLEData
import dataclasses
from typing import Optional


@dataclasses.dataclass
class SGP4Data:
   position_km: tuple[float, float, float] 
   velocity_km_sec: tuple[float, float, float]
   error : Optional[str] = None #should I set this as optional to default as none or is the redundancy not needed



def setup_spg4(tle: TLEData) -> Satrec:
    sat = _sgp4API.Satrec()
    sat.sgp4init(
        WGS72,                # gravity model
        'i',                  # 'a' = old AFSPC mode, 'i' = improved mode
        tle.satellite_number,                # satnum: Satellite number
        tle.to_JT(),       # epoch: days since 1949 December 31 00:00 UT
        3.8792e-05,           # bstar: drag coefficient (1/earth radii)
        0.0,                  # ndot: ballistic coefficient (radians/minute^2)
        0.0,                  # nddot: mean motion 2nd derivative (radians/minute^3)
        tle.eccentricity,            # ecco: eccentricity
        tle.argument_of_perigee,   # argpo: argument of perigee (radians 0..2pi)
        tle.inclination,   # inclo: inclination (radians 0..pi)
        tle.mean_anomaly,   # mo: mean anomaly (radians 0..2pi)
        tle.mean_motion,  # no_kozai: mean motion (radians/minute)
        tle.right_ascension,    # nodeo: R.A. of ascending node (radians 0..2pi)
    )
    return sat

def get_sat_position(tle: TLEData, dt: datetime) -> SGP4Data:
    sat = setup_spg4(tle)
    jd, fr = jday(datetime.year, datetime.month, datetime.day, datetime.hour, datetime.minute, datetime.second)
    error_code, position, velocity = sat.sgp4(jd, fr)
    error_message = SGP4_ERRORS.get(error_code, None)
    return SGP4Data(
        position_km=tuple(position),
        velocity_km_sec = tuple(velocity),
        error = error_message
    )




