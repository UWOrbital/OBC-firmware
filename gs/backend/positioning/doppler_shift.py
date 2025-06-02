from typing import Tuple
from skyfield.api import EarthSatellite, Topos, load 
# There might be a slightly simpler library but we're already using skyfield in ephemeris, so might as well use it here too

SPEED_OF_LIGHT = 299_792_458

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


def calculate_relative_velocity(satellite: EarthSatellite, observer_coords: Tuple[float, float, float]) -> float:
    """
    @brief Computes relative velocity between satellite and observer
    @param satellite: EarthSatellite object
    @param observer_coords: latitude_deg, longitude_deg, altitude_m
    @returns Relative radial velocity in m/s (positive is moving away)
    """
    latitude_deg, longitude_deg, altitude_m = observer_coords
    t = load.timescale().now()

    observer = Topos(latitude_degrees=latitude_deg, longitude_degrees=longitude_deg, elevation_m=altitude_m)
    difference = satellite - observer
    topocentric = difference.at(t)

    # Separate velocity and position vectors
    vel = topocentric.velocity.km_per_s
    pos = topocentric.position.km

    # Normalize position
    pos_mag = sum(p**2 for p in pos) ** 0.5
    los_unit = [p / pos_mag for p in pos]

    # Radial velocity = dot product of velocity vector with line-of-sight unit vector
    radial_velocity_km_s = sum(vel[i] * los_unit[i] for i in range(3))

    return radial_velocity_km_s * 1000  # km/s -> m/s


def compute_doppler_shift(frequency_hz: float, relative_velocity_m_s: float) -> float:
    """
    @brief Computes the Doppler-shift frequency
    @param frequency_hz: Transmission frequency in Hz
    @param relative_velocity_m_s: Relative radial velocity in m/s
    @returns Doppler-shift frequency in Hz
    """
    return frequency_hz * (((SPEED_OF_LIGHT + relative_velocity_m_s) / SPEED_OF_LIGHT) - 1)


def calculate_doppler(
    tle_line1: str,
    tle_line2: str,
    observer_coords: Tuple[float, float, float],
    transmission_frequency_hz: float = 433_920_000  # Default frequency, UW-Orbital's 433.920 MHz band
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
    rv = calculate_relative_velocity(sat, observer_coords)
    return compute_doppler_shift(transmission_frequency_hz, rv)
