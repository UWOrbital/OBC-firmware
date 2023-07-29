# Parser for tests and will be used as the basis for the parser on the OBC
import os.path
from typing import List, BinaryIO
import struct
from dataclasses import dataclass
import sys

import ephemeris
from ephemeris import DataPoint


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
    with open(file, "rb") as file:
        file.seek(0)
        start_time = get_single_data_point(file, False)
        step_size = get_single_data_point(file, False)
        num_data_points = int(struct.unpack(ephemeris.DATA_UINT, file.read(ephemeris.SIZE_OF_INT))[0])
        return Header(start_time, step_size, num_data_points)


def get_single_data_point(file: BinaryIO, is_float=True) -> float:
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

    byte_str = file.read(read_size)
    float_val = struct.unpack(read_type, byte_str)[0]
    return float(float_val)


def parse_file(file: str) -> List[DataPoint]:
    """
    Tests the output file to ensure that the data was written correctly

    :param file: The file to read from
    """
    output = []
    header = parse_header(file)

    with open(file, "rb") as file:
        file.seek(ephemeris.SIZE_OF_HEADER)

        for i in range(header.num_data_points):
            jd = header.start_time + (i * header.step_size)
            data_point = DataPoint(jd, get_single_data_point(file),
                                   get_single_data_point(file), get_single_data_point(file))
            output.append(data_point)

    return output


def main():
    file_path = os.path.join('..', 'test_sun', 'test1.bin')
    expected = ephemeris.main(f'2020-01-01 2020-01-02 -s 5m -o {file_path}')
    actual = parse_file('test1.bin')
    print(f'{len(expected) = }')
    print(f'{len(actual) = }')
    assert len(expected) == len(actual)
    for i in range(len(expected)):
        print(f'{expected[i] = }')
        print(f'{actual[i] = }')
        print()
    assert expected == actual


def _command_line():
    """
    Parses the command line arguments and runs the program
    """
    if len(sys.argv) != 2:
        print("Usage: python parser.py <file>")
        exit(1)
    print(parse_file(sys.argv[1]))


if __name__ == '__main__':
    # _command_line()
    main()
