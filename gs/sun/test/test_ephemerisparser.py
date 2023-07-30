import pytest

import os
import struct

from sun import ephemerisparser as ep
from sun import ephemeris


@pytest.mark.parametrize("ephermeris_data_type, is_float, expected", [
    (ephemeris.DATA_FLOAT, True, 2451545.0),
    (ephemeris.DATA_DOUBLE, False, 2451545.0),
    (ephemeris.DATA_FLOAT, True, 20.0),
    (ephemeris.DATA_DOUBLE, False, 20.0)
])
def test_get_single_data_point(ephermeris_data_type, is_float, expected):
    filename = 'test_get_single_data_point_float.bin'

    # Write a float to the file
    with open(filename, 'wb') as f:
        f.write(struct.pack(ephermeris_data_type, expected))

    # Read the float from the file
    with open(filename, 'rb') as f:
        f.seek(0)
        value = ep.get_single_data_point(f, is_float)

    os.remove(filename)
    assert value == expected

@pytest.mark.parametrize("expected", [
    (ep.Header(2451545.0, 20.0, 1000)),
    (ep.Header(5.0, 20.0, 4))
])
def test_parse_header(expected):
    filename = 'test_parse_header.bin'

    with open(filename, 'wb') as f:
        f.write(struct.pack(ephemeris.DATA_DOUBLE, expected.start_time))
        f.write(struct.pack(ephemeris.DATA_DOUBLE, expected.step_size))
        f.write(struct.pack(ephemeris.DATA_UINT, expected.num_data_points))

    actual = ep.parse_header(filename)
    os.remove(filename)

    assert actual == expected


def test_parse_file():
    file = "test_parse_file.bin"

    # Write header to file
    # Test in test_write_header()
    ephemeris.write_header(file, 0, 2, 3)

    # Write data points to file
    data_points_actual = [ephemeris.DataPoint(0, 1, 2, 3),
                   ephemeris.DataPoint(1, 4, 5, 6),
                   ephemeris.DataPoint(2, 7, 8, 9)]

    for data_point in data_points_actual:
        # Test in test_write_data()
        ephemeris.write_data(data_point, file)

    data_points_expected = ep.parse_file(file)
    os.remove(file)

    assert data_points_actual == data_points_expected
