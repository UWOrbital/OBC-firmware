# Parser for tests and will be used as the basis for the parser on the OBC
# Test harness for test_ephemeris.py
from __future__ import annotations

from dataclasses import dataclass
from struct import unpack

# Standard library imports
from typing import BinaryIO

# Local application imports
from . import ephemeris
from .ephemeris import DataPoint


@dataclass
class Header:
    """
    Data class to store the header information
    """

    start_time: float
    step_size: float
    num_data_points: int


def parse_header(file: str) -> Header:
    """
    Tests the header file to ensure that the data was read correctly

    :param file: The file to read from
    """
    with open(file, "rb") as f:
        f.seek(0)
        # Read 2 double values
        start_time = get_single_data_point(f, False)
        step_size = get_single_data_point(f, False)

        # Read 1 uint value
        num_data_points = int(unpack(ephemeris.DATA_UINT, f.read(ephemeris.SIZE_OF_INT))[0])
        return Header(start_time, step_size, num_data_points)


def get_single_data_point(file: BinaryIO, is_float: bool = True) -> float:
    """
    Tests the output file to ensure that the data was written correctly

    :param is_float: If true, then will parse as float, otherwise will parse as double
    :param file: The file to read from
    """
    if is_float:
        read_type = ephemeris.DATA_FLOAT
        read_size = ephemeris.SIZE_OF_FLOAT
    else:
        read_type = ephemeris.DATA_DOUBLE
        read_size = ephemeris.SIZE_OF_DOUBLE

    # Read and parse 1 float/double value
    byte_str = file.read(read_size)
    float_val = unpack(read_type, byte_str)[0]
    return float(float_val)


def parse_file(file: str) -> list[DataPoint]:
    """
    Tests the output file to ensure that the data was written correctly

    :param file: The file to read from
    """
    output = []
    header = parse_header(file)

    with open(file, "rb") as f:
        f.seek(ephemeris.SIZE_OF_HEADER)

        # Read and calculate the data points
        for i in range(header.num_data_points):
            jd = header.start_time + (i * header.step_size)
            data_point = DataPoint(
                jd,
                get_single_data_point(f),
                get_single_data_point(f),
                get_single_data_point(f),
            )
            output.append(data_point)

    return output
