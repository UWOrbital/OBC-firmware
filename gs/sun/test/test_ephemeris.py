import pytest

import logging
from os import remove
import os

from sun import ephemeris
from sun.ephemeris import ErrorCode


def test_is_float0():
    assert ephemeris.is_float('-1.0')


def test_is_float1():
    assert ephemeris.is_float('1')


def test_is_float2():
    assert ephemeris.is_float("1.0")


def test_is_float3():
    assert ephemeris.is_float("1")


def test_is_float4():
    assert not ephemeris.is_float("a")


def test_is_float5():
    assert not ephemeris.is_float("1a")


def test_is_float6():
    assert not ephemeris.is_float("a1")


def test_is_float7():
    assert not ephemeris.is_float("1.0a")


def test_is_float8():
    assert not ephemeris.is_float("a1.0")


def test_is_float9():
    assert not ephemeris.is_float("1.a0")


def test_is_valid_time0():
    assert ephemeris.is_valid_time("JD2451545.0")


def test_is_valid_time1():
    assert ephemeris.is_valid_time("JD2451545")


def test_is_valid_time2():
    assert ephemeris.is_valid_time("2020-01-02")


def test_is_valid_time3():
    assert not ephemeris.is_valid_time("2020-01-02 12:00:00")


def test_calculate_step_size0():
    assert ephemeris.calculate_step_size(1, 1, 1) == 0


def test_calculate_step_size1():
    assert ephemeris.calculate_step_size(1, 3, 2) == 2


def test_calculate_step_size2():
    assert ephemeris.calculate_step_size(1, 7, 4) == 2


def test_calculate_step_size3():
    assert ephemeris.calculate_step_size(10, 110, 11) == 10


def test_calculate_step_size4():
    with pytest.raises(ValueError) as _:
        ephemeris.calculate_step_size(1, 1, 0)


def test_calculate_step_size5():
    with pytest.raises(ValueError) as _:
        ephemeris.calculate_step_size(1, 1, -1)


def test_calculate_step_size6():
    with pytest.raises(ValueError) as _:
        ephemeris.calculate_step_size(1, 1, 2)


def test_calculate_step_size7():
    with pytest.raises(ValueError) as _:
        ephemeris.calculate_step_size(3, 1, 3)


# Helper function to suppress logging during tests
def suppress_logging(caplog):
    for handler in logging.getLogger().handlers:
        caplog.set_level(logging.ERROR, logger=handler.name)


# Test cases for validate_input function
@pytest.mark.parametrize("start_time, stop_time, step_size, output, expected_result", [
    ("2023-07-20", "2023-07-25", "1d", "output.bin", ErrorCode.SUCCESS),
    ("2023-07-20", "2023-07-25", "1w", "output.bin", ErrorCode.INVALID_STEP_SIZE),
    ("2023-07-20", "2023-07-25", "1m", "output.dat", ErrorCode.INVALID_OUTPUT_FILE),
    ("2023-07-20", "2023-07-25", "1y", "output.bin", ErrorCode.SUCCESS),
    ("2023-07-20", "2023-07-25", "invalid", "output.bin", ErrorCode.INVALID_STEP_SIZE),
    ("2023-07-20", "2023-07-25", "3", "output.bin", ErrorCode.SUCCESS),
    ("2023-07-20", "2023-07-25", "1h", "output.bin", ErrorCode.SUCCESS),
    ("2023-07-20", "2023-07-25", "1", "output.bin", ErrorCode.SUCCESS),
    ("2023-07-20", "2023-07-25", "100", "output.bin", ErrorCode.SUCCESS),
    ("2023-07-20", "2023-07-25", "1y", "output.txt", ErrorCode.INVALID_OUTPUT_FILE),
    ("2023-07-20", "2023-07-25", "1y", "output.bin.txt", ErrorCode.INVALID_OUTPUT_FILE),
    ("2023-07-20", "2023-07-25", "1y", "output.bin.bin", ErrorCode.SUCCESS),
])
def test_validate_input(start_time, stop_time, step_size, output, expected_result, caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog)

    result = ephemeris.validate_input(start_time, stop_time, step_size, output)
    assert result == expected_result


# Additional test cases to check if error codes are returned correctly
def test_validate_input_invalid_start_time(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog)

    start_time = "invalid_start_time"
    stop_time = "2023-07-25"
    step_size = "1d"
    output = "output.bin"

    result = ephemeris.validate_input(start_time, stop_time, step_size, output)
    assert result == ErrorCode.INVALID_START_TIME


def test_validate_input_invalid_stop_time(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog)

    start_time = "2023-07-20"
    stop_time = "invalid_stop_time"
    step_size = "1d"
    output = "output.bin"

    result = ephemeris.validate_input(start_time, stop_time, step_size, output)
    assert result == ErrorCode.INVALID_STOP_TIME


def test_validate_input_invalid_output_file(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog)

    start_time = "2023-07-20"
    stop_time = "2023-07-25"
    step_size = "1d"
    output = "output.txt"

    result = ephemeris.validate_input(start_time, stop_time, step_size, output)
    assert result == ErrorCode.INVALID_OUTPUT_FILE


def test_allocate_header_false():
    file = 'test_allocate_header.bin'
    ephemeris.allocate_header(False, file)
    assert not os.path.exists(file)


def test_allocate_header_true():
    file = 'test_allocate_header.bin'
    ephemeris.allocate_header(True, file)
    assert os.path.exists(file)

    try:
        with open(file, 'rb') as f:
            assert f.read() == b'\x00' * ephemeris.SIZE_OF_HEADER
    finally:
        os.remove(file)


def test_default_step_size():
    parser = ephemeris.define_parser()
    assert parser.get_default('step_size') == ephemeris.DEFAULT_STEP_SIZE


# Check if the default value for target is set correctly
def test_default_target():
    parser = ephemeris.define_parser()
    assert parser.get_default('target') == ephemeris.DEFAULT_TARGET


# Check if the default value for output is set correctly
def test_default_output():
    parser = ephemeris.define_parser()
    assert parser.get_default('output') == ephemeris.DEFAULT_FILE_OUTPUT


def test_argument_parsing():
    # Simulate command-line arguments
    args = ['2023-07-26', '2023-07-27', '-s', '1h', '-t', 'sun', '-o', 'output_file.bin',
            '-p', '2', '-e', 'both', '-v', '-l', 'debug.log']

    # Parse the arguments using the defined parser
    parser = ephemeris.define_parser()
    parsed_args = parser.parse_args(args)

    # Assert the values of the parsed arguments
    assert parsed_args.start_time == '2023-07-26'
    assert parsed_args.stop_time == '2023-07-27'
    assert parsed_args.step_size == '1h'
    assert parsed_args.target == 'sun'
    assert parsed_args.output == 'output_file.bin'
    assert parsed_args.print == 2
    assert parsed_args.exclude == 'both'
    assert parsed_args.verbose is True
    assert parsed_args.log == 'debug.log'
