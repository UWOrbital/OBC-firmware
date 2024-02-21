import os
import struct

import pytest

from gs.sun import ephemeris
from gs.sun import ephemeris_parser as ep


@pytest.mark.parametrize(
    "ephermeris_data_type, is_float, expected",
    [
        (ephemeris.DATA_FLOAT, True, 2451545.0),
        (ephemeris.DATA_DOUBLE, False, 2451545.0),
        (ephemeris.DATA_FLOAT, True, 20.0),
        (ephemeris.DATA_DOUBLE, False, 20.0),
    ],
)
def test_get_single_data_point(ephermeris_data_type, is_float, expected):
    filename = "test_get_single_data_point_float.bin"

    # Write a float or double to the file
    with open(filename, "wb") as f:
        f.write(struct.pack(ephermeris_data_type, expected))

    # Read the float or double from the file
    with open(filename, "rb") as f:
        f.seek(0)
        value = ep.get_single_data_point(f, is_float)

    os.remove(filename)
    assert value == expected


@pytest.mark.parametrize("expected", [(ep.Header(2451545.0, 20.0, 1000)), (ep.Header(5.0, 20.0, 4))])
def test_parse_header(expected):
    filename = "test_parse_header.bin"

    # Write header to file
    with open(filename, "wb") as f:
        f.write(struct.pack(ephemeris.DATA_DOUBLE, expected.start_time))
        f.write(struct.pack(ephemeris.DATA_DOUBLE, expected.step_size))
        f.write(struct.pack(ephemeris.DATA_UINT, expected.num_data_points))

    # Read the header from the file and delete the file
    actual = ep.parse_header(filename)
    os.remove(filename)

    assert actual == expected


def test_parse_file():
    file = "test_parse_file.bin"

    # Write header to file
    # Tested in test_write_header()
    ephemeris.write_header(file, 0, 1, 3)

    # Data points to be written to the file
    data_points_actual = [
        ephemeris.DataPoint(0, 1, 2, 3),
        ephemeris.DataPoint(1, 4, 5, 6),
        ephemeris.DataPoint(2, 7, 8, 9),
    ]

    with open(file, "ab") as f:
        for data_point in data_points_actual:
            # Tested in test_write_data()
            ephemeris.write_data(data_point, f)

    # Read the data points from the file and delete the file
    data_points_expected = ep.parse_file(file)
    os.remove(file)

    assert data_points_actual == data_points_expected
