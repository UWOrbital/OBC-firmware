from skyfield.api import EarthSatellite, Topos, load
from typing import Final

SPEED_OF_LIGHT_METERS_PER_SECOND: Final[int] = 299_792_458

"""
@brief Holds logic for calculating doppler shift from TLS and ground station coordinates
@details Since we're not actually getting TLE or interfacing with HackRF it's not hooked up to anything yet
"""


def load_satellite(tle_line1: str, tle_line2: str, name: str = "UW_SAT") -> EarthSatellite:
    """
    @brief Loads satellite from TLE lines
    @param tle_line1: First line of TLE
    @param tle_line2: Second line of TLE
    @param name: Optional satellite name
    @returns EarthSatellite object
    """
    return EarthSatellite(tle_line1, tle_line2, name, load.timescale())


def calculate_relative_velocity(
    satellite: EarthSatellite, observer_latitude_deg: float, observer_longitude_deg: float, observer_altitude_m: float
) -> float:
    """
    @brief Computes relative velocity between satellite and observer
    @param satellite: EarthSatellite object
    @param observer_latitude_degree: Latitude of observer in degrees
    @param observer_longitude_deg: Longitude of observer in degrees
    @param observer_altitude_m: Altitude of observer in meters
    @returns Relative radial velocity in m/s (positive is moving away)
    """
    time_current = load.timescale().now()

    observer = Topos(
        latitude_degrees=observer_latitude_deg,
        longitude_degrees=observer_longitude_deg,
        elevation_m=observer_altitude_m,
    )
    difference = satellite - observer
    topocentric = difference.at(time_current)

    # Separate velocity and position vectors
    vx, vy, vz = topocentric.velocity.km_per_s
    vel: tuple[float, float, float] = (vx, vy, vz)

    px, py, pz = topocentric.position.km
    pos: tuple[float, float, float] = (px, py, pz)

    # Normalize position
    pos_mag = sum(p**2 for p in pos) ** 0.5
    los_unit = [p / pos_mag for p in pos]

    # Radial velocity = dot product of velocity vector with line-of-sight unit vector
    radial_velocity_km_s = sum(vel[i] * los_unit[i] for i in range(3))

    return float(radial_velocity_km_s * 1000.0)  # km/s -> m/s


def compute_doppler_shift(frequency_hz: float, relative_velocity_m_s: float) -> float:
    """
    @brief Computes the Doppler-shift frequency
    @param frequency_hz: Transmission frequency in Hz
    @param relative_velocity_m_s: Relative radial velocity in m/s
    @returns Doppler-shift frequency in Hz
    """
    return frequency_hz * (
        ((SPEED_OF_LIGHT_METERS_PER_SECOND + relative_velocity_m_s) / SPEED_OF_LIGHT_METERS_PER_SECOND) - 1
    )


def calculate_doppler(
    tle_line1: str,
    tle_line2: str,
    observer_latitude_deg: float,
    observer_longitude_deg: float,
    observer_altitude_m: float,
    transmission_frequency_hz: float = 433_920_000,  # Default frequency, UW-Orbital's 433.920 MHz band
) -> float:
    """
    @brief High-level function to compute Doppler shift
    @param tle_line1: First line of TLE data
    @param tle_line2: Second line of TLE data
    @param observer_coords: Tuple of (latitude_deg, longitude_deg, altitude_m)
    @param transmission_frequency_hz: Frequency of the satellite transmission in Hz
    @returns Doppler-shifted frequency in Hz
    """
    sat = load_satellite(tle_line1, tle_line2)
    rv = calculate_relative_velocity(sat, observer_latitude_deg, observer_longitude_deg, observer_altitude_m)
    return compute_doppler_shift(transmission_frequency_hz, rv)
