from __future__ import annotations

# Standard library imports
from argparse import ArgumentParser
from collections.abc import Iterable
from dataclasses import dataclass
from datetime import date, datetime
from enum import Enum
from json import dumps, loads
from math import isclose
from os import remove
from os.path import exists
from re import compile
from struct import pack
from sys import exit, stderr
from typing import BinaryIO, Final

# 3rd party imports
from requests import Response, get
from skyfield.api import load

from gs.backend.common.logger import DEFAULT_LOG_FORMAT, logger

# Script constants
SUPPORTED_VERSION: Final[str] = "1.2"
NUMBER_OF_HEADER_DOUBLES: Final[int] = 2
RELATIVE_TOLERANCE: Final[float] = 1e-7
API_LIMIT: Final[int] = 90_000

# Print values
LOGGER_LEVELS = ["WARNING", "INFO", "DEBUG"]
DEFAULT_PRINT_WARNING: Final[int] = 0  # Prints the required output

# Data types
DATA_FLOAT: Final[str] = "f"
DATA_DOUBLE: Final[str] = "d"
DATA_UINT: Final[str] = "I"

# Default values
DEFAULT_STEP_SIZE: Final[str] = "5m"
DEFAULT_TARGET: Final[str] = "sun"
DEFAULT_FILE_OUTPUT: Final[str] = "output.bin"
DEFAULT_EXCLUDE: Final[str] = "last"

# Size constants
SIZE_OF_DOUBLE: Final[int] = 8
SIZE_OF_FLOAT: Final[int] = 4
SIZE_OF_INT: Final[int] = 4
SIZE_OF_HEADER: Final[int] = SIZE_OF_DOUBLE * NUMBER_OF_HEADER_DOUBLES + SIZE_OF_INT


class ErrorCode(Enum):
    """Error codes enumerator"""

    SUCCESS = 0
    INVALID_DATE_TIME = 1
    INVALID_STEP_SIZE = 2
    INVALID_OUTPUT_FILE = 3
    NO_SIGNATURE_FOUND = 4
    INVALID_REQUEST400 = 5
    INVALID_REQUEST = 6
    UNKNOWN = 7


@dataclass
class DataPoint:
    """Data class to store a position data point"""

    jd: float
    x: float
    y: float
    z: float

    def __eq__(self, other: object) -> bool:
        """
        Checks if the two data points are within a certain tolerance

        :param other: The other data point
        :return: True if the two data points are equal otherwise False
        """
        if not isinstance(other, DataPoint):
            return NotImplemented
        return (
            isclose(self.jd, other.jd, rel_tol=RELATIVE_TOLERANCE)
            and isclose(self.x, other.x, rel_tol=RELATIVE_TOLERANCE)
            and isclose(self.y, other.y, rel_tol=RELATIVE_TOLERANCE)
            and isclose(self.z, other.z, rel_tol=RELATIVE_TOLERANCE)
        )


def define_parser() -> ArgumentParser:
    """
    Defines the parser for the script

    :return: The parser
    """
    parser = ArgumentParser(description="Position Ephemeris Retriever")
    parser.add_argument(
        "start_time",
        type=str,
        help="Start time in the format YYYY-MM-DD or JD#. Must be of the same format as stop time",
    )
    parser.add_argument(
        "stop_time",
        type=str,
        help="Stop time in the format YYYY-MM-DD or JD#. Must be of the same format as start time",
    )
    parser.add_argument(
        "-s",
        "--step-size",
        type=str,
        default=DEFAULT_STEP_SIZE,
        help=f"Step size in the same format as the Horizons API (e.g. 1m, 1h, 1d, 100). "
        f"Default: {DEFAULT_STEP_SIZE}",
    )
    parser.add_argument(
        "-t",
        "--target",
        type=str,
        default=DEFAULT_TARGET,
        help=f"Target object (e.g. sun, moon, mars). Default: {DEFAULT_TARGET}",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        default=DEFAULT_FILE_OUTPUT,
        help=f"Output file name. Default: {DEFAULT_FILE_OUTPUT}",
    )
    parser.add_argument(
        "-p",
        "--print",
        type=int,
        choices=range(3),
        default=DEFAULT_PRINT_WARNING,
        help=f"Prints the output to the console used for debugging purposes. \
                        0 = Always, 1 = Basic debugging, 2 = All output. "
        f"Default: {DEFAULT_PRINT_WARNING}",
    )
    parser.add_argument(
        "-e",
        "--exclude",
        choices=["first", "last", "both", "none"],
        default=DEFAULT_EXCLUDE,
        help=f"Exclude the first, last, both or none of the values from the output file. "
        f"Default: {DEFAULT_EXCLUDE}",
    )
    parser.add_argument(
        "-l",
        "--log",
        type=str,
        default=None,
        help="Log file for debugging purposes. Default: None (standard output)",
    )

    return parser


def is_float(num: str) -> bool:
    """
    Checks if the parameter is a float

    :param num: The parameter to check
    :return: True if the parameter is a float otherwise False
    """
    try:
        float(num)
        return True
    except ValueError:
        return False


def is_valid_date(date_str: str) -> bool:
    """
    Checks if date is a valid date of the YYYY-MM-DD format

    :param date_str: The date to be checked
    :return: True if the date is a valid date, otherwise False
    """
    time_regex = compile(r"^[1-9]\d{3}-\d{2}-\d{2}$")
    if not time_regex.match(date_str):
        return False
    try:
        year, month, day = date_str.split("-")
        date(year=int(year), month=int(month), day=int(day))

        return True
    except ValueError:
        return False


def is_valid_julian_date(time: str) -> bool:
    """
    Checks if time is a valid julian date where time starts with JD and is followed by a positive number

    :param time: The parameter to check
    :return: True if the parameter is a valid time otherwise False
    """
    return time.startswith("JD") and is_float(time[2:]) and float(time[2:]) > 0


def convert_date_to_jd(time: str) -> float:
    """
    Converts the inputted time to a jd if it is a datetime of the format YYYY-MM-DD.
    If it is a jd#, then the number part is returned. This function doesnt not perform error
    checking.

    :param time: The time to convert to jd
    :return: jd of inputted time
    """
    if time.startswith("JD"):
        return float(time[2:])

    timescale = load.timescale()

    base_date = datetime.datetime.fromisoformat(time).replace(tzinfo=datetime.UTC)
    sky_date = timescale.from_datetime(base_date)

    return float(sky_date.ut1)


def validate_input(start_time: str, stop_time: str, step_size: str, output: str) -> ErrorCode:
    """
    Validates the input arguments created by the define_parser() function. If all the inputs are valid then it will
    do return a success code otherwise it return an error code

    :param start_time: Start time in the format YYYY-MM-DD or JD#
    :param stop_time: Stop time in the format YYYY-MM-DD or JD#
    :param step_size: Step size in the same format as the Horizons API (e.g. 1m, 1h, 1d, 100)
    :param output: Output file name in the format *.bin
    """
    # Checks if the start_time and stop_time have valid datetime formats that are the same format
    if not (is_valid_date(start_time) and is_valid_date(stop_time)) and not (
        is_valid_julian_date(start_time) and is_valid_julian_date(stop_time)
    ):
        logger.critical("Start time or stop time do not both have the same format of YYYY-MM-DD or JD#")
        return ErrorCode.INVALID_DATE_TIME

    # Checks if the step size is in the correct format
    if not ((step_size[-1] in ["m", "d", "h", "s"] and step_size[:-1].isnumeric()) or step_size.isnumeric()):
        logger.critical("Step size must be in the format #m, #d, #h, #s, or #. Where # is an integer")
        return ErrorCode.INVALID_STEP_SIZE

    # Checks if the output file is in the correct format
    if not output.endswith(".bin"):
        logger.critical("Output file must be in the format *.bin")
        return ErrorCode.INVALID_OUTPUT_FILE

    return ErrorCode.SUCCESS


def check_version(data: dict) -> ErrorCode:  # type: ignore
    """
    Prints out a warning if the version is different from the supported one

    :param data: response.txt
    :return: ErrorCode.SUCCESS if successful otherwise an error code
    """
    signature = data.get("signature")

    # Checks if the signature is valid
    if signature is None or not isinstance(signature, dict):
        logger.critical("ERROR: INVALID SIGNATURE")
        return ErrorCode.NO_SIGNATURE_FOUND

    # Checks if the version is supported
    if signature.get("version") != SUPPORTED_VERSION:
        logger.warning("WARNING: UNSUPPORTED HORIZON API VERSION USED")

    return ErrorCode.SUCCESS


# Not testable as we can't simulate a Response object
# Code taken from horizon API with slight modifications
def validate_response(response: Response) -> ErrorCode:
    """
    Validates the responses. It handles the 400 status error code specifically. It also makes sure that the status code
    is always 200 (success) for the rest of the script

    :param response: The response object
    :return: ErrorCode.SUCCESS if the response is valid otherwise ErrorCode.INVALID_REQUEST400
    or ErrorCode.INVALID_REQUEST
    """
    status_code = response.status_code
    if status_code == 400:
        data = loads(response.text)
        if "message" in data:
            logger.critical("Message: {message}", message=data["message"])
        else:
            logger.critical(dumps(data, indent=2))
        return ErrorCode.INVALID_REQUEST400

    if response.status_code != 200:
        logger.critical(f"{response.status_code = }")
        return ErrorCode.INVALID_REQUEST

    data = loads(response.text)
    if data.get("error") is not None:
        logger.critical(data.get("error"))
        return ErrorCode.UNKNOWN

    return ErrorCode.SUCCESS


# Not testable as it is a print statement used for debugging
def print_debug_header(reverse: bool = False) -> None:
    """
    Prints the header of the data printed, used for debugging purposes

    :param reverse: If True then reverses the order of the header and prints and extra seperator line
    """
    if reverse:
        logger.info("-" * 130)

    logger.info(("\t" * 3) + "JD:" + ("\t" * 4) + "X:" + ("\t" * 5) + "Y:" + ("\t" * 4) + "Z:")
    logger.info("-" * 130)


def write_data(data: DataPoint, file: BinaryIO) -> None:
    """
    Write the parameter data to the given file

    :param file_name: The output file name
    :param data: Data to be written
    """
    # Appends the data to the file and prints the expected data written if applicable
    logger.debug(f"\tData written: {data}", data=data)

    # Write the x value
    bx = pack(DATA_FLOAT, data.x)
    file.write(bytearray(bx))

    # Write the y value
    by = pack(DATA_FLOAT, data.y)
    file.write(bytearray(by))

    # Write the z value
    bz = pack(DATA_FLOAT, data.z)
    file.write(bytearray(bz))


def write_header(
    file_output: str,
    min_jd: float,
    step_size: float,
    count: int,
    exclude: str = "none",
    *,
    write_to_file: bool = True,
) -> None:
    """
    Writes the data header (min_jd, step_size, count) the output file

    :param count: The number of the data points
    :param max_jd: The maximum JD value used to calculate the step size
    :param min_jd: The minimum JD value
    :param file_output: The output file
    :param write_to_file: If True then the header is written to the file
    """
    if not write_to_file:
        return None

    # Create the file if it doesn't exist as it doesn't create one by default
    if not exists(file_output):
        open(file_output, "wb").close()

    data = [min_jd, step_size]

    if exclude != "none":
        count -= 1

    # Write the data to the file
    with open(file_output, "rb+") as file:
        logger.debug(f"Writing header to {file_output}")
        file.seek(0)

        # Write the data to the file that is of type double
        for i in data:
            logger.debug(f"\tData written: {i}", i=i)
            b = pack(DATA_DOUBLE, i)
            byte: bytearray = bytearray(b)
            file.write(byte)

        # Write the count to the file
        byte_count = pack(DATA_UINT, int(count))
        file.write(bytearray(byte_count))


def calculate_number_of_data_points(start_time: float, stop_time: float, step_size: str) -> int:
    """
    Calculates the number of data points. This function assumes all inputs are valid and performs
    no error checking.
    """
    difference = stop_time - start_time
    if step_size.isnumeric():
        return int(step_size) + 1
    elif step_size.endswith("d"):
        return int(difference / int(step_size[:-1])) + 1
    elif step_size.endswith("h"):
        return int(difference * 24 / int(step_size[:-1])) + 1
    return int(difference * 24 * 60 / int(step_size[:-1])) + 1


def calculate_step_size(min_jd: float, max_jd: float, number_of_data_points: int) -> float:
    """
    Calculates the step size of the data or raises an error if the parameters are invalid

    :param min_jd: The minimum JD
    :param max_jd: The maximum JD
    :param number_of_data_points: The number of data points
    :return: The step size
    """
    if max_jd < min_jd:
        raise ValueError("The maximum JD is less than the minimum JD")

    if min_jd == max_jd and number_of_data_points > 1:
        raise ValueError("The minimum JD is equal to the maximum JD but there is more than one data point")

    if number_of_data_points < 1:
        raise ValueError("The number of data points is less than 1")

    if number_of_data_points == 1:
        return 0

    return (max_jd - min_jd) / (number_of_data_points - 1)


def exit_program_on_error(error_code: ErrorCode) -> None:
    """
    Exits the program with the given error code if it is not a success

    :param error_code: The error code
    """
    if error_code != ErrorCode.SUCCESS:
        exit(error_code.value)


def extract_data_lines(lines: Iterable[str]) -> list[str]:
    """
    Finds the number of data points in the data

    :param lines: The lines of the data
    :return: The number of data points
    """
    output = []
    start = False
    for i in lines:
        # Start of data
        if i.startswith("$$SOE"):
            start = True
        # End of data
        if i.startswith("$$EOE"):
            start = False
        # Data point
        if start and not i.startswith("$$SOE"):
            output.append(i)

    return output


def get_lines_from_api(start_time: float, stop_time: float, step_size: int, target: str) -> list[str]:
    """
    Get the lines from the Horizons API

    :param start_time: Start time of data
    :param stop_time: Stop time of data
    :param step_size: Step size of data
    :param target: Target body
    :return: Lines of data
    """
    # Get the data from the API and validate it
    url = (
        f"https://ssd.jpl.nasa.gov/api/horizons.api?format=json&MAKE_EPHEM=YES&EPHEM_TYPE=VECTORS&COMMAND="
        f"{target}&OBJ_DATA=NO&STEP_SIZE={step_size}&START_TIME=JD{str(start_time)}&STOP_TIME="
        f"JD{str(stop_time)}&CSV_FORMAT=YES&CAL_FORMAT=JD&VEC_TABLE=1"
    )

    response = get(url)
    exit_program_on_error(validate_response(response))

    try:
        data = loads(response.text)
    except ValueError:
        logger.critical("Invalid JSON response")
        exit(-1)

    exit_program_on_error(check_version(data))

    # Start processing the data taken from API
    lines = data.get("result").split("\n")
    return extract_data_lines(lines)


def logger_setup(log_level: str, file_name: str | None) -> None:
    """
    Sets up the logger for the program
    """
    logger.remove()
    if file_name is not None:
        logger.add(file_name, level=log_level, format=DEFAULT_LOG_FORMAT, diagnose=True)
    else:
        logger.add(stderr, level=log_level, format=DEFAULT_LOG_FORMAT, colorize=True, diagnose=True)


def main(argsv: str | None = None) -> list[DataPoint]:
    """
    Main function of the program.
    :param argsv: The arguments to be parsed, similar to sys.argv
    """
    # Parse the arguments and validate them
    args = define_parser().parse_args(argsv.split() if isinstance(argsv, str) else None)
    exit_program_on_error(validate_input(args.start_time, args.stop_time, args.step_size, args.output))

    # Set up logger
    logger_setup(LOGGER_LEVELS[args.print], args.log)

    start_time = convert_date_to_jd(args.start_time)
    stop_time = convert_date_to_jd(args.stop_time)
    data_count = calculate_number_of_data_points(start_time, stop_time, args.step_size)
    try:
        step_size = calculate_step_size(start_time, stop_time, data_count)
    except ValueError as e:
        logger.critical(e)
        exit(-2)

    # Make requests to api
    lines = []
    request_count = data_count // API_LIMIT
    request_step = step_size * API_LIMIT
    for i in range(request_count):
        lines.extend(
            get_lines_from_api(
                start_time + i * request_step,
                start_time + (i + 1) * request_step,
                API_LIMIT,
                args.target,
            )
        )
        lines.pop()
    lines.extend(
        get_lines_from_api(
            start_time + request_count * request_step,
            stop_time,
            data_count - API_LIMIT * request_count - 1,
            args.target,
        )
    )

    # Delete file if it exists
    if exists(args.output):
        remove(args.output)

    # Write header
    write_header(args.output, start_time, step_size, data_count, args.exclude)

    data_points = []
    lines_written = 0
    print_debug_header()

    with open(args.output, "ab") as file:
        # Loop over response
        for count, line in enumerate(lines):
            # Depends on the exclude flag
            if not (
                (count == 0 and (args.exclude == "both" or args.exclude == "first"))
                or (count == data_count - 1)
                and (args.exclude == "both" or args.exclude == "last")
            ):
                # Parse the line of data
                logger.debug(f"Line being parsed: {line}", line=line)
                output = line[:-1].split(", ")

                # Parse, store and write the data point
                logger.info(f"Output written: {output}", output=output)
                # 1st element of parsed string is the date in YYYY-MM-DD format
                data_point = DataPoint(
                    float(output[0]),
                    float(output[2]),
                    float(output[3]),
                    float(output[4]),
                )
                data_points.append(data_point)
                write_data(data_point, file)
                lines_written = count

    # Write the header, print debug header
    print_debug_header(True)

    print(f"Lines written: {lines_written + 1}")
    logger.info("Lines written: {lines_written}", lines_written=lines_written + 1)

    return data_points


if __name__ == "__main__":
    main()
