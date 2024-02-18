import pytest

import struct
import logging
import os
import math

from gs.sun import ephemeris
from gs.sun.ephemeris import ErrorCode
from gs.sun.ephemeris import DataPoint
from gs.sun import ephemerisparser as ep


@pytest.mark.parametrize(
    "arg",
    [
        ("-1.0"),
        ("1"),
        ("1.0"),
        ("0"),
    ],
)
def test_is_float_true(arg):
    assert ephemeris.is_float(arg)


@pytest.mark.parametrize(
    "arg",
    [
        ("a"),
        ("1a"),
        ("a1"),
        ("1.0a"),
        ("a1.0"),
        ("1.a0"),
    ],
)
def test_is_float_false(arg):
    assert not ephemeris.is_float(arg)


@pytest.mark.parametrize(
    "arg",
    [
        ("JD2451545.0"),
        ("JD2451545"),
        ("JD1.1"),
    ],
)
def test_is_valid_julian_true(arg):
    assert ephemeris.is_valid_julian_date(arg)


@pytest.mark.parametrize("arg", [("2020-01-02 12:00:00"), ("JD0"), ("JD-1"), ("24")])
def test_is_valid_julian_false(arg):
    assert not ephemeris.is_valid_julian_date(arg)


@pytest.mark.parametrize(
    "arg",
    [
        ("2020-01-01"),
        ("2024-12-31"),
    ],
)
def test_is_valid_date_true(arg):
    assert ephemeris.is_valid_date(arg)


@pytest.mark.parametrize("arg", [("2020-01-02 12:00:00"), ("JD0"), ("JD-1"), ("24")])
def test_is_valid_date_false(arg):
    assert not ephemeris.is_valid_date(arg)


@pytest.mark.parametrize(
    "min_jd, max_jd, count, expected",
    [
        (1, 1, 1, 0),
        (1, 3, 2, 2),
        (1, 7, 4, 2),
        (10, 110, 11, 10),
    ],
)
def test_calculate_step_size_true(min_jd, max_jd, count, expected):
    assert ephemeris.calculate_step_size(min_jd, max_jd, count) == expected


@pytest.mark.parametrize(
    "min_jd, max_jd, count",
    [
        (1, 1, 0),
        (1, 1, -1),
        (1, 1, 2),
        (3, 1, 3),
    ],
)
def test_calculate_step_size_error(min_jd, max_jd, count):
    with pytest.raises(ValueError) as _:
        ephemeris.calculate_step_size(min_jd, max_jd, count)


# Helper function to suppress logging during tests
# Not tested as it is used in the tests itself
def suppress_logging(caplog, *, level=logging.ERROR):
    for handler in logging.getLogger().handlers:
        caplog.set_level(level, logger=handler.name)


# Test for suppress_logging function
def test_suppress_logging1(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.CRITICAL)

    # Test if logging is suppressed
    logging.error("This message should not be logged")
    assert len(caplog.records) == 0
    assert "" == caplog.text


@pytest.mark.parametrize(
    "level, count, msg, func, expected",
    [
        (logging.CRITICAL, 0, "This message should not be logged", logging.error, ""),
        (
            logging.ERROR,
            1,
            "This message should not be logged",
            logging.error,
            "This message should not be logged",
        ),
        (
            logging.WARNING,
            1,
            "This message should not be logged",
            logging.error,
            "This message should not be logged",
        ),
        (
            logging.INFO,
            1,
            "This message should not be logged",
            logging.error,
            "This message should not be logged",
        ),
        (
            logging.DEBUG,
            1,
            "This message should not be logged",
            logging.error,
            "This message should not be logged",
        ),
        (
            logging.CRITICAL,
            1,
            "This message should not be logged",
            logging.critical,
            "This message should not be logged",
        ),
        (
            logging.ERROR,
            1,
            "This message should not be logged",
            logging.critical,
            "This message should not be logged",
        ),
        (
            logging.WARNING,
            1,
            "This message should not be logged",
            logging.critical,
            "This message should not be logged",
        ),
        (
            logging.INFO,
            1,
            "This message should not be logged",
            logging.critical,
            "This message should not be logged",
        ),
        (
            logging.DEBUG,
            1,
            "This message should not be logged",
            logging.critical,
            "This message should not be logged",
        ),
        (logging.CRITICAL, 0, "This message should not be logged", logging.warning, ""),
        (logging.ERROR, 0, "This message should not be logged", logging.warning, ""),
        (
            logging.WARNING,
            1,
            "This message should not be logged",
            logging.warning,
            "This message should not be logged",
        ),
        (
            logging.INFO,
            1,
            "This message should not be logged",
            logging.warning,
            "This message should not be logged",
        ),
        (
            logging.DEBUG,
            1,
            "This message should not be logged",
            logging.warning,
            "This message should not be logged",
        ),
        (logging.CRITICAL, 0, "This message should not be logged", logging.info, ""),
        (logging.ERROR, 0, "This message should not be logged", logging.info, ""),
        (logging.WARNING, 0, "This message should not be logged", logging.info, ""),
        (
            logging.INFO,
            1,
            "This message should not be logged",
            logging.info,
            "This message should not be logged",
        ),
    ],
)
def test_suppress_logging(caplog, level, count, msg, func, expected):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=level)

    # Test if logging is suppressed
    func(msg)
    assert len(caplog.records) == count

    if expected:
        assert expected in caplog.text
    # Empty string case is handled separately
    else:
        assert expected == caplog.text


# Test cases for validate_input function
@pytest.mark.parametrize(
    "start_time, stop_time, step_size, output, expected_result",
    [
        ("2023-07-20", "2023-07-25", "1d", "output.bin", ErrorCode.SUCCESS),
        ("2023-07-20", "2023-07-25", "1w", "output.bin", ErrorCode.INVALID_STEP_SIZE),
        ("2023-07-20", "2023-07-25", "1m", "output.dat", ErrorCode.INVALID_OUTPUT_FILE),
        (
            "2023-07-20",
            "2023-07-25",
            "invalid",
            "output.bin",
            ErrorCode.INVALID_STEP_SIZE,
        ),
        ("2023-07-20", "2023-07-25", "3", "output.bin", ErrorCode.SUCCESS),
        ("2023-07-20", "2023-07-25", "1h", "output.bin", ErrorCode.SUCCESS),
        ("2023-07-20", "2023-07-25", "1", "output.bin", ErrorCode.SUCCESS),
        ("2023-07-20", "2023-07-25", "100", "output.bin", ErrorCode.SUCCESS),
        ("2023-07-20", "2023-07-25", "1y", "output.txt", ErrorCode.INVALID_STEP_SIZE),
        ("2023-07-20", "2023-07-25", "1d", "output.txt", ErrorCode.INVALID_OUTPUT_FILE),
        (
            "2023-07-20",
            "2023-07-25",
            "1d",
            "output.bin.txt",
            ErrorCode.INVALID_OUTPUT_FILE,
        ),
        ("2023-07-20", "2023-07-25", "1d", "output.bin.bin", ErrorCode.SUCCESS),
        ("JD", "JD100", "1d", "output.bin", ErrorCode.INVALID_DATE_TIME),
        ("JD100", "JD", "1d", "output.bin", ErrorCode.INVALID_DATE_TIME),
        ("2023-07-20", "2023-07-25", "10w", "output.bin", ErrorCode.INVALID_STEP_SIZE),
        ("2023-07-20", "2023-07-25", "10d", "output.bin", ErrorCode.SUCCESS),
        (
            "2023-07-20",
            "2023-07-25",
            "10d",
            "output.txt",
            ErrorCode.INVALID_OUTPUT_FILE,
        ),
        ("JD1000", "JD1001", "1m", "output.txt", ErrorCode.INVALID_OUTPUT_FILE),
        ("JD1000", "JD1001", "1m", "output.bin", ErrorCode.SUCCESS),
    ],
)
def test_validate_input(
    start_time, stop_time, step_size, output, expected_result, caplog
):
    # Suppress logging messages during the test
    suppress_logging(caplog)

    result = ephemeris.validate_input(start_time, stop_time, step_size, output)
    assert result == expected_result


def test_define_parser_default_step_size():
    parser = ephemeris.define_parser()
    assert parser.get_default("step_size") == ephemeris.DEFAULT_STEP_SIZE


# Check if the default value for target is set correctly
def test_define_parser_default_target():
    parser = ephemeris.define_parser()
    assert parser.get_default("target") == ephemeris.DEFAULT_TARGET


# Check if the default value for output is set correctly
def test_define_parser_default_output():
    parser = ephemeris.define_parser()
    assert parser.get_default("output") == ephemeris.DEFAULT_FILE_OUTPUT


def test_define_parser_argument_parsing():
    # Simulate command-line arguments
    args = [
        "2023-07-26",
        "2023-07-27",
        "-s",
        "1h",
        "-t",
        "sun",
        "-o",
        "output_file.bin",
        "-p",
        "2",
        "-e",
        "both",
        "-l",
        "debug.log",
    ]

    # Parse the arguments using the defined parser
    parser = ephemeris.define_parser()
    parsed_args = parser.parse_args(args)

    # Assert the values of the parsed arguments
    assert parsed_args.start_time == "2023-07-26"
    assert parsed_args.stop_time == "2023-07-27"
    assert parsed_args.step_size == "1h"
    assert parsed_args.target == "sun"
    assert parsed_args.output == "output_file.bin"
    assert parsed_args.print == 2
    assert parsed_args.exclude == "both"
    assert parsed_args.log == "debug.log"


def test_check_version_success(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.NOTSET)
    d = {"signature": {"version": ephemeris.SUPPORTED_VERSION}}

    # Check if the version is correct
    assert ephemeris.check_version(d) == ErrorCode.SUCCESS
    assert caplog.text == ""


def test_check_version_invalid_version(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {"signature": {"version": "0.0.0"}}

    # Check if the version is incorrect
    assert ephemeris.check_version(d) == ErrorCode.SUCCESS
    assert "WARNING: UNSUPPORTED HORIZON API VERSION USED" in caplog.text


def test_check_version_no_version(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {"signature": "invalid"}

    # Check if the signature is incorrect
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert "ERROR: INVALID SIGNATURE" in caplog.text


def test_check_version_no_signature(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {}

    # Check if the signature doesn't exist
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert "ERROR: INVALID SIGNATURE" in caplog.text


def test_check_version_signature_none(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {"signature": None}

    # Check if the signature is incorrect
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert "ERROR: INVALID SIGNATURE" in caplog.text


def test_check_version_signature_not_dict(caplog):
    # Suppress logging messages during the test
    suppress_logging(caplog, level=logging.WARNING)
    d = {"signature": "invalid"}

    # Check if the signature is incorrect
    assert ephemeris.check_version(d) == ErrorCode.NO_SIGNATURE_FOUND
    assert "ERROR: INVALID SIGNATURE" in caplog.text


def test_write_header():
    file = "test_write_header.bin"

    # Write the header to the file and check that it exists
    ephemeris.write_header(file, 10, 15, 5)
    assert os.path.exists(file)

    # Read and delete the file
    with open(file, "rb") as f:
        value = f.read()
    os.remove(file)

    assert len(value) == ephemeris.SIZE_OF_HEADER  # Check if the file size is correct
    assert struct.unpack(ephemeris.DATA_DOUBLE, value[0:8])[0] == 10
    assert struct.unpack(ephemeris.DATA_DOUBLE, value[8:16])[0] == 15
    assert struct.unpack(ephemeris.DATA_UINT, value[16:20])[0] == 5


def test_write_header_no_file():
    file = "test_write_header.bin"
    ephemeris.write_header(file, 10, 15, 5, write_to_file=False)
    assert not os.path.exists(file)


def test_extract_data_points1():
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
    """.split("\n")
    assert len(ephemeris.extract_data_lines(lines)) == 15


def test_extract_data_points_no_data():
    lines = """
$$SOE
$$EOE
    """.split("\n")
    assert len(ephemeris.extract_data_lines(lines)) == 0


def test_extract_data_points2():
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
    """.split("\n")
    assert len(ephemeris.extract_data_lines(lines)) == 12


def test_exit_program_on_error_success():
    ephemeris.exit_program_on_error(ErrorCode.SUCCESS)
    assert True  # If we get here, we're good as the program didn't exit


# JD is not written to the file so set it to 0
@pytest.mark.parametrize(
    "data_point",
    [
        (DataPoint(0, 10, 15, -1)),
        (DataPoint(0, 10, 15, 0)),
        (DataPoint(0, -5, 65.5, 1)),
        (DataPoint(0, 0, 0, 1)),
        (DataPoint(0, 0, 0, 0)),
        (DataPoint(0, 7, 7, 7)),
    ],
)
def test_write_data(data_point):
    file = "test_write_data.bin"
    with open(file, "ab") as f:
        ephemeris.write_data(data_point, f)

    # Read the data points from the file
    with open(file, "rb") as f:
        # get_single_data_point() is tested in test_ephermerisparser.py
        x = ep.get_single_data_point(f)
        y = ep.get_single_data_point(f)
        z = ep.get_single_data_point(f)

    os.remove(file)

    # JD is not written to file, so set it to 0
    assert ephemeris.DataPoint(0, x, y, z) == data_point


@pytest.mark.parametrize(
    "argsv, data_points_expected",
    [
        (
            # Default: exclude last
            "JD1 JD2 -s 3h -o test_main.bin",
            [
                DataPoint(
                    1,
                    x=1.384519786747137e08,
                    y=-5.472710939424842e07,
                    z=-1.276932755237378e06,
                ),
                DataPoint(
                    1.125000000,
                    x=1.385760653373899e08,
                    y=-5.442875513032935e07,
                    z=-1.272689795682322e06,
                ),
                DataPoint(
                    1.250000000,
                    x=1.386995049407168e08,
                    y=-5.413014269361465e07,
                    z=-1.268441062983371e06,
                ),
                DataPoint(
                    1.375000000,
                    x=1.388222969000409e08,
                    y=-5.383127342494688e07,
                    z=-1.264186567945778e06,
                ),
                DataPoint(
                    1.500000000,
                    x=1.389444406309174e08,
                    y=-5.353214866855511e07,
                    z=-1.259926321179278e06,
                ),
                DataPoint(
                    1.625000000,
                    x=1.390659355491845e08,
                    y=-5.323276977215502e07,
                    z=-1.255660333107308e06,
                ),
                DataPoint(
                    1.750000000,
                    x=1.391867810710398e08,
                    y=-5.293313808704702e07,
                    z=-1.251388613976542e06,
                ),
                DataPoint(
                    1.875000000,
                    x=1.393069766131224e08,
                    y=-5.263325496821213e07,
                    z=-1.247111173866812e06,
                ),
            ],
        ),
    ],
)
def test_main(argsv, data_points_expected):
    filename = "test_main.bin"
    data_points_returned = ephemeris.main(argsv)

    data_points_actual = ep.parse_file(filename)
    os.remove(filename)

    assert data_points_expected == data_points_returned
    assert data_points_actual == data_points_returned
    assert data_points_actual == data_points_expected


# Main thing is to test the logic behind the function, assume math.isclose works
@pytest.mark.parametrize(
    "data_point, tolerance",
    [
        (DataPoint(0, 0, 0, 0), 0),
        (DataPoint(1, 1, 1, 1), 1e-7),
        (DataPoint(1, 1, 1, 1), 1e-8),
        (DataPoint(1, 1, 1, 1), -1e-8),
        (DataPoint(1, 1, 1, 1), -1e-7),
    ],
)
def test_datapoint_equals(data_point, tolerance):
    new_data_point = DataPoint(
        data_point.jd * (1 + tolerance),
        data_point.x * (1 + tolerance),
        data_point.y * (1 + tolerance),
        data_point.z * (1 + tolerance),
    )

    assert data_point == new_data_point


# Main thing is to test the logic behind the function, assume math.isclose works
@pytest.mark.parametrize(
    "data_point, tolerance",
    [
        (DataPoint(1, 1, 1, 1), 1e-6),
        (DataPoint(1, 1, 1, 1), 1e-5),
        (DataPoint(1, 1, 1, 1), -1e-6),
        (DataPoint(1, 1, 1, 1), -1e-5),
    ],
)
def test_datapoint_not_equal_all(data_point, tolerance):
    new_data_point = DataPoint(
        data_point.jd * (1 + tolerance),
        data_point.x * (1 + tolerance),
        data_point.y * (1 + tolerance),
        data_point.z * (1 + tolerance),
    )

    assert data_point != new_data_point


# Main thing is to test the logic behind the function, assume math.isclose works
@pytest.mark.parametrize(
    "data_point1, data_point2",
    [
        (DataPoint(1, 1, 1, 1), DataPoint((1 + 1e-6), 1, 1, 1)),
        (DataPoint(1, 1, 1, 1), DataPoint(1, (1 + 1e-6), 1, 1)),
        (DataPoint(1, 1, 1, 1), DataPoint(1, 1, (1 + 1e-6), 1)),
        (DataPoint(1, 1, 1, 1), DataPoint(1, 1, 1, (1 + 1e-6))),
    ],
)
def test_datapoint_not_equal(data_point1, data_point2):
    assert data_point1 != data_point2


@pytest.mark.parametrize(
    "start_time, stop_time, step_size, expected",
    [
        (1, 110000, "1", 2),
        (1, 110000, "5", 6),
        (1, 110000, "0", 1),
        (1, 110000, "1d", 110_000),
        (1, 100, "1h", 24 * 99 + 1),
        (10, 19, "1m", 9 * 24 * 60 + 1),
        (1, 2, "3h", 9),
    ],
)
def test_calculate_number_of_data_points_true(
    start_time, stop_time, step_size, expected
):
    assert (
        ephemeris.calculate_number_of_data_points(start_time, stop_time, step_size)
        == expected
    )


@pytest.mark.parametrize(
    "time, expected",
    [
        ("JD1", 1),
        ("JD69.420", 69.420),
        ("2000-01-01", 2451544.500000000),
        (
            "2024-01-20",
            2451544.500000000 + 8785,
        ),  # Jan 1, 2024 is 8785 days since Jan 1, 2000
    ],
)
def test_convert_to_date(time, expected):
    assert math.isclose(ephemeris.convert_date_to_jd(time), expected)


def test_get_lines_from_api():
    lines = [
        "      1.000000000, B.C. 4713-Jan-02 12:00:00.0000,  1.384519786747137E+08, -5.472710939424842E+07, -1.276932755237378E+06,",
        "      1.125000000, B.C. 4713-Jan-02 15:00:00.0000,  1.385760653373899E+08, -5.442875513032935E+07, -1.272689795682322E+06,",
        "      1.250000000, B.C. 4713-Jan-02 18:00:00.0000,  1.386995049407168E+08, -5.413014269361465E+07, -1.268441062983371E+06,",
        "      1.375000000, B.C. 4713-Jan-02 21:00:00.0000,  1.388222969000409E+08, -5.383127342494688E+07, -1.264186567945778E+06,",
        "      1.500000000, B.C. 4713-Jan-03 00:00:00.0000,  1.389444406309174E+08, -5.353214866855511E+07, -1.259926321179278E+06,",
        "      1.625000000, B.C. 4713-Jan-03 03:00:00.0000,  1.390659355491845E+08, -5.323276977215502E+07, -1.255660333107308E+06,",
        "      1.750000000, B.C. 4713-Jan-03 06:00:00.0000,  1.391867810710398E+08, -5.293313808704702E+07, -1.251388613976542E+06,",
        "      1.875000000, B.C. 4713-Jan-03 09:00:00.0000,  1.393069766131224E+08, -5.263325496821213E+07, -1.247111173866812E+06,",
        "      2.000000000, B.C. 4713-Jan-03 12:00:00.0000,  1.394265215925976E+08, -5.233312177440570E+07, -1.242828022701215E+06,",
    ]
    assert ephemeris.get_lines_from_api(1, 2, 8, "sun") == lines


# @pytest.mark.skip(reason="Taking too long")
def test_100k_request():
    filename = "test_100k_request.bin"
    length = 110_000
    data_points_returned = ephemeris.main(f"JD1 JD110001 -s {length} -o {filename}")
    file_size = os.path.getsize(filename)
    assert (
        file_size == length * (3 * ephemeris.SIZE_OF_FLOAT) + ephemeris.SIZE_OF_HEADER
    )

    data_points_actual = ep.parse_file(filename)
    os.remove(filename)

    assert data_points_returned == data_points_actual
