import os
import struct

import pytest
from gs.backend.positioning.tle import calculate_checksum, convert_decimal_point_assumed, parse_tle_data

@pytest.mark.parametrize(
    "tle_line, expected_checksum",
    [
        ("1 25544U 98067A 23058.54791667 .00016717 00000+0 10270-3 0 9991", 0),
    ],
)

def test_calculate_checksum(tle_line, expected_checksum):
    assert calculate_checksum(tle_line) == expected_checksum

@pytest.mark.parametrize(
    "input_value, expected_result",
    [
        ("123456-3", 0.00123456),
        ("678901+2", 678.901),
        ("-456789-5", -0.0000456789),
    ],
)

def test_convert_decimal_point_assumed(input_value, expected_result):
    assert convert_decimal_point_assumed(input_value)==expected_result


def test_parse_tle_data():
    tle_lines = [
        "1 25544U 98067A   23058.54791667  .00016717  00000+0  10270-3 0  9991",
        "2 25544  51.6435 143.0464 0004767 278.8055  81.2436 15.49815308274058"
    ]

    parsed_data = parse_tle_data(tle_lines)

    assert "satellite_number" in parsed_data
    assert parsed_data["satellite_number"] == 25544
    assert "inclination" in parsed_data
    assert parsed_data["inclination"] == pytest.approx(51.6435,rel=1e-4)
    assert "right_ascension" in parsed_data
    assert parsed_data["right_ascension"] == 143.0464

