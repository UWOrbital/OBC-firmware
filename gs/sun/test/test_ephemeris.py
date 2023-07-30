import pytest

import struct
import logging
import os

from sun import ephemeris
from sun.ephemeris import ErrorCode
from sun import ephemerisparser as ep


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
def suppress_logging(caplog, *, level=logging.ERROR):
    for handler in logging.getLogger().handlers:
        caplog.set_level(level, logger=handler.name)


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
    ("JD", "JD100", "1y", "output.bin", ErrorCode.INVALID_START_TIME),
    ("JD100", "JD", "1y", "output.bin", ErrorCode.INVALID_STOP_TIME),
    ("2023-07-20", "2023-07-25", "10w", "output.bin", ErrorCode.INVALID_STEP_SIZE),
    ("2023-07-20", "2023-07-25", "10d", "output.bin", ErrorCode.SUCCESS),
    ("2023-07-20", "2023-07-25", "10d", "output.txt", ErrorCode.INVALID_OUTPUT_FILE),
    ("JD1000", "JD1001", "1m", "output.txt", ErrorCode.INVALID_OUTPUT_FILE),
    ("JD1000", "JD1001", "1m", "output.bin", ErrorCode.SUCCESS),
])
def test_validate_input(start_time, stop_time, step_size, output, expected_result, caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog)

    result = ephemeris.validate_input(start_time, stop_time, step_size, output)
    assert result == expected_result


def test_allocate_header_false():
    file = 'test_allocate_header.bin'
    ephemeris.allocate_header(False, file)
    assert not os.path.exists(file)


def test_allocate_header_true():
    file = 'test_allocate_header.bin'

    ephemeris.allocate_header(True, file)
    assert os.path.exists(file)

    with open(file, 'rb') as f:
        value = f.read()
    os.remove(file)

    assert len(value) == ephemeris.SIZE_OF_HEADER  # Check if the file size is correct


def test_define_parser_default_step_size():
    parser = ephemeris.define_parser()
    assert parser.get_default('step_size') == ephemeris.DEFAULT_STEP_SIZE


# Check if the default value for target is set correctly
def test_define_parser_default_target():
    parser = ephemeris.define_parser()
    assert parser.get_default('target') == ephemeris.DEFAULT_TARGET


# Check if the default value for output is set correctly
def test_define_parser_default_output():
    parser = ephemeris.define_parser()
    assert parser.get_default('output') == ephemeris.DEFAULT_FILE_OUTPUT


def test_define_parser_argument_parsing():
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


def test_check_version_success(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.NOTSET)
    d = {'signature': {'version': ephemeris.SUPPORTED_VERSION}}

    # Check if the version is correct
    assert ephemeris.check_version(d) == ErrorCode.SUCCESS
    assert caplog.text == ''


def test_check_version_invalid_version(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {'signature': {'version': '0.0.0'}}

    # Check if the version is incorrect
    assert ephemeris.check_version(d) == ErrorCode.SUCCESS
    assert 'WARNING: UNSUPPORTED HORIZON API VERSION USED' in caplog.text


def test_check_version_no_version(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {'signature': 'invalid'}

    # Check if the version is incorrect
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert 'ERROR: INVALID SIGNATURE' in caplog.text


def test_check_version_no_signature(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {}

    # Check if the version is incorrect
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert 'ERROR: INVALID SIGNATURE' in caplog.text


def test_check_version_signature_none(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {'signature': None}

    # Check if the version is incorrect
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert 'ERROR: INVALID SIGNATURE' in caplog.text


def test_check_version_signature_not_dict(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {'signature': 'invalid'}

    # Check if the version is incorrect
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert 'ERROR: INVALID SIGNATURE' in caplog.text


def test_write_header():
    file = 'test_write_header.bin'
    ephemeris.write_header(file, 10, 15, 5)
    assert os.path.exists(file)

    with open(file, 'rb') as f:
        value = f.read()
    os.remove(file)

    assert len(value) == ephemeris.SIZE_OF_HEADER  # Check if the file size is correct
    assert struct.unpack(ephemeris.DATA_DOUBLE, value[0:8])[0] == 10
    assert struct.unpack(ephemeris.DATA_DOUBLE, value[8:16])[0] == 1.25  # (15 - 10) / (5 - 1)
    assert struct.unpack(ephemeris.DATA_UINT, value[16:20])[0] == 5


def test_write_header_no_file():
    file = 'test_write_header.bin'
    ephemeris.write_header(file, 10, 15, 5, write_to_file=False)
    assert not os.path.exists(file)


def test_find_number_of_data_points():
    # Test data taken from the Horizons API docs (https://ssd-api.jpl.nasa.gov/doc/horizons.html)
    lines = """
$$SOE
1998-Jan-01 10:00     20 55 41.20 -18 33 23.0    1.199   4.107  2.13799045474771   5.6049390
1998-Jan-01 11:00     20 55 49.17 -18 32 49.7    1.205   4.112  2.13812533287512   5.6044117
1998-Jan-01 12:00     20 55 57.13 -18 32 16.5    1.221   4.128  2.13826019831403   5.6038846
1998-Jan-01 13:00     20 56 05.09 -18 31 43.1    1.234   4.141  2.13839505107206   5.6033579
1998-Jan-01 14:00     20 56 13.05 -18 31 09.8    1.249   4.156  2.13852989115697   5.6028315
1998-Jan-01 15:00     20 56 21.01 -18 30 36.5    1.229   4.136  2.13866471857652   5.6023055
1998-Jan-01 16:00     20 56 28.97 -18 30 03.1    1.242   4.149  2.13879953333835   5.6017797
1998-Jan-01 17:00     20 56 36.93 -18 29 29.7    1.236   4.143  2.13893433545018   5.6012543
1998-Jan-01 18:00     20 56 44.89 -18 28 56.3    1.225   4.132  2.13906912492008   5.6007292
1998-Jan-01 19:00     20 56 52.85 -18 28 22.9    1.233   4.139  2.13920390175678   5.6002045
1998-Jan-01 20:00     20 57 00.81 -18 27 49.4    1.239   4.146  2.13933866596968   5.5996802
1998-Jan-01 21:00     20 57 08.76 -18 27 15.9    1.246   4.153  2.13947341756857   5.5991564
1998-Jan-01 22:00     20 57 16.72 -18 26 42.5    1.226   4.133  2.13960815656331   5.5986329
1998-Jan-01 23:00     20 57 24.67 -18 26 08.9    1.216   4.123  2.13974288296371   5.5981098
1998-Jan-02 00:00     20 57 32.63 -18 25 35.4    1.205   4.111  2.13987759677950   5.5975872
$$EOE
    """.split('\n')
    assert ephemeris.find_number_of_data_points(lines) == 15


def test_find_number_of_data_points_no_data():
    lines = """
$$SOE
$$EOE
    """.split('\n')
    assert ephemeris.find_number_of_data_points(lines) == 0


def test_find_number_of_data_points():
    # Test data modified from the Horizons API docs (https://ssd-api.jpl.nasa.gov/doc/horizons.html)
    # First 2 and last lines are ignored
    lines = """
1998-Jan-01 10:00     20 55 41.20 -18 33 23.0    1.199   4.107  2.13799045474771   5.6049390
1998-Jan-01 11:00     20 55 49.17 -18 32 49.7    1.205   4.112  2.13812533287512   5.6044117
$$SOE
1998-Jan-01 12:00     20 55 57.13 -18 32 16.5    1.221   4.128  2.13826019831403   5.6038846
1998-Jan-01 13:00     20 56 05.09 -18 31 43.1    1.234   4.141  2.13839505107206   5.6033579
1998-Jan-01 14:00     20 56 13.05 -18 31 09.8    1.249   4.156  2.13852989115697   5.6028315
1998-Jan-01 15:00     20 56 21.01 -18 30 36.5    1.229   4.136  2.13866471857652   5.6023055
1998-Jan-01 16:00     20 56 28.97 -18 30 03.1    1.242   4.149  2.13879953333835   5.6017797
1998-Jan-01 17:00     20 56 36.93 -18 29 29.7    1.236   4.143  2.13893433545018   5.6012543
1998-Jan-01 18:00     20 56 44.89 -18 28 56.3    1.225   4.132  2.13906912492008   5.6007292
1998-Jan-01 19:00     20 56 52.85 -18 28 22.9    1.233   4.139  2.13920390175678   5.6002045
1998-Jan-01 20:00     20 57 00.81 -18 27 49.4    1.239   4.146  2.13933866596968   5.5996802
1998-Jan-01 21:00     20 57 08.76 -18 27 15.9    1.246   4.153  2.13947341756857   5.5991564
1998-Jan-01 22:00     20 57 16.72 -18 26 42.5    1.226   4.133  2.13960815656331   5.5986329
1998-Jan-01 23:00     20 57 24.67 -18 26 08.9    1.216   4.123  2.13974288296371   5.5981098
$$EOE
1998-Jan-02 00:00     20 57 32.63 -18 25 35.4    1.205   4.111  2.13987759677950   5.5975872
    """.split('\n')
    assert ephemeris.find_number_of_data_points(lines) == 12

@pytest.mark.parametrize("error_code_object, error_code", [
    (ErrorCode.INVALID_START_TIME, 1),
    (ErrorCode.INVALID_STOP_TIME, 2),
    (ErrorCode.INVALID_STEP_SIZE, 3),
    (ErrorCode.INVALID_OUTPUT_FILE, 4),
    (ErrorCode.NO_SIGNATURE_FOUND, 5),
    (ErrorCode.INVALID_REQUEST400, 6),
    (ErrorCode.INVALID_REQUEST, 7),
])
def test_exit_program_on_error(error_code_object, error_code):
    with pytest.raises(SystemExit) as e:
        ephemeris.exit_program_on_error(error_code_object)

    assert e.type == SystemExit
    assert e.value.code == error_code


def test_exit_program_on_error_success():
    ephemeris.exit_program_on_error(ErrorCode.SUCCESS)
    assert True  # If we get here, we're good as the program didn't exit


# JD is not written to the file so set it to 0
@pytest.mark.parametrize("data_point", [
    (ephemeris.DataPoint(0, 10, 15, -1)),
    (ephemeris.DataPoint(0, 10, 15, 0)),
    (ephemeris.DataPoint(0, -5, 65.5, 1)),
    (ephemeris.DataPoint(0, 0, 0, 1)),
    (ephemeris.DataPoint(0, 0, 0, 0)),
    (ephemeris.DataPoint(0, 7, 7, 7))
])
def test_write_data(data_point):
    file = "test_write_data.bin"
    ephemeris.write_data(data_point, file)

    with open(file, "rb") as f:
        # get_single_data_point() is tested in test_ephermerisparser.py
        x = ep.get_single_data_point(f)
        y = ep.get_single_data_point(f)
        z = ep.get_single_data_point(f)

    os.remove(file)

    # JD is not written to file, so set it to 0
    assert ephemeris.DataPoint(0, x, y, z) == data_point
