from __future__ import annotations

# Standard library imports
import argparse
import dataclasses
import datetime
import enum
import json
import logging
import math
import os
import re
import struct
import sys
from collections.abc import Iterable
from typing import BinaryIO, Final

# 3rd party imports
import requests
from skyfield.api import load

# Script constants
SUPPORTED_VERSION: Final[str] = "1.2"
NUMBER_OF_HEADER_DOUBLES: Final[int] = 2
RELATIVE_TOLERANCE: Final[float] = 1e-7
API_LIMIT: Final[int] = 90_000

# Print values
LOGGING_LEVELS = [logging.WARNING, logging.INFO, logging.DEBUG]
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


class ErrorCode(enum.Enum):
    """Error codes enumerator"""

    SUCCESS = 0
    INVALID_DATE_TIME = 1
    INVALID_STEP_SIZE = 2
    INVALID_OUTPUT_FILE = 3
    NO_SIGNATURE_FOUND = 4
    INVALID_REQUEST400 = 5
    INVALID_REQUEST = 6
    UNKNOWN = 7


@dataclasses.dataclass
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
            math.isclose(self.jd, other.jd, rel_tol=RELATIVE_TOLERANCE)
            and math.isclose(self.x, other.x, rel_tol=RELATIVE_TOLERANCE)
            and math.isclose(self.y, other.y, rel_tol=RELATIVE_TOLERANCE)
            and math.isclose(self.z, other.z, rel_tol=RELATIVE_TOLERANCE)
        )


def define_parser() -> argparse.ArgumentParser:
    """
    Defines the parser for the script

    :return: The parser
    """
    parser = argparse.ArgumentParser(description="Position Ephemeris Retriever")
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


def is_valid_date(date: str) -> bool:
    """
    Checks if date is a valid date of the YYYY-MM-DD format

    :param date: The date to be checked
    :return: True if the date is a valid date, otherwise False
    """
    time_regex = re.compile(r"^[1-9]\d{3}-\d{2}-\d{2}$")
    if not time_regex.match(date):
        return False
    try:
        year, month, day = date.split("-")
        datetime.date(year=int(year), month=int(month), day=int(day))

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

    base_date = datetime.datetime.fromisoformat(time).replace(tzinfo=datetime.timezone.utc)
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
        logging.critical("Start time or stop time do not both have the same format of YYYY-MM-DD or JD#")
        return ErrorCode.INVALID_DATE_TIME

    # Checks if the step size is in the correct format
    if not ((step_size[-1] in ["m", "d", "h", "s"] and step_size[:-1].isnumeric()) or step_size.isnumeric()):
        logging.critical("Step size must be in the format #m, #d, #h, #s, or #. Where # is an integer")
        return ErrorCode.INVALID_STEP_SIZE

    # Checks if the output file is in the correct format
    if not output.endswith(".bin"):
        logging.critical("Output file must be in the format *.bin")
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
        logging.critical("ERROR: INVALID SIGNATURE")
        return ErrorCode.NO_SIGNATURE_FOUND

    # Checks if the version is supported
    if signature.get("version") != SUPPORTED_VERSION:
        logging.warning("WARNING: UNSUPPORTED HORIZON API VERSION USED")

    return ErrorCode.SUCCESS


# Not testable as we can't simulate a Response object
# Code taken from horizon API with slight modifications
def validate_response(response: requests.Response) -> ErrorCode:
    """
    Validates the responses. It handles the 400 status error code specifically. It also makes sure that the status code
    is always 200 (success) for the rest of the script

    :param response: The response object
    :return: ErrorCode.SUCCESS if the response is valid otherwise ErrorCode.INVALID_REQUEST400
    or ErrorCode.INVALID_REQUEST
    """
    if response.status_code == 400:
        data = json.loads(response.text)
        if "message" in data:
            logging.critical(f"Message: {data['message']}")
        else:
            logging.critical(json.dumps(data, indent=2))
        return ErrorCode.INVALID_REQUEST400

    if response.status_code != 200:
        logging.critical(f"{response.status_code = }")
        return ErrorCode.INVALID_REQUEST

    data = json.loads(response.text)
    if data.get("error") is not None:
        logging.critical(data.get("error"))
        return ErrorCode.UNKNOWN

    return ErrorCode.SUCCESS


# Not testable as it is a print statement used for debugging
def print_debug_header(reverse: bool = False) -> None:
    """
    Prints the header of the data printed, used for debugging purposes

    :param reverse: If True then reverses the order of the header and prints and extra seperator line
    """
    if reverse:
        logging.info("-" * 130)

    logging.info(("\t" * 3) + "JD:" + ("\t" * 4) + "X:" + ("\t" * 5) + "Y:" + ("\t" * 4) + "Z:")
    logging.info("-" * 130)


def write_data(data: DataPoint, file: BinaryIO) -> None:
    """
    Write the parameter data to the given file

    :param file_name: The output file name
    :param data: Data to be written
    """
    # Appends the data to the file and prints the expected data written if applicable
    logging.debug(f"\tData written: {data}")

    # Write the x value
    bx = struct.pack(DATA_FLOAT, data.x)
    file.write(bytearray(bx))

    # Write the y value
    by = struct.pack(DATA_FLOAT, data.y)
    file.write(bytearray(by))

    # Write the z value
    bz = struct.pack(DATA_FLOAT, data.z)
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
    if not os.path.exists(file_output):
        open(file_output, "wb").close()

    data = [min_jd, step_size]

    if exclude != "none":
        count -= 1

    # Write the data to the file
    with open(file_output, "rb+") as file:
        logging.debug(f"Writing header to {file_output}")
        file.seek(0)

        # Write the data to the file that is of type double
        for i in data:
            logging.debug(f"\tData written: {i}")
            b = struct.pack(DATA_DOUBLE, i)
            byte: bytearray = bytearray(b)
            file.write(byte)

        # Write the count to the file
        byte_count = struct.pack(DATA_UINT, int(count))
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
        sys.exit(error_code.value)


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

    response = requests.get(url)
    exit_program_on_error(validate_response(response))

    try:
        data = json.loads(response.text)
    except ValueError:
        logging.critical("Invalid JSON response")
        sys.exit(-1)

    exit_program_on_error(check_version(data))

    # Start processing the data taken from API
    lines = data.get("result").split("\n")
    return extract_data_lines(lines)


def main(argsv: str | None = None) -> list[DataPoint]:
    """
    Main function of the program.
    :param argsv: The arguments to be parsed, similar to sys.argv
    """
    # Parse the arguments and validate them
    args = define_parser().parse_args(argsv.split() if isinstance(argsv, str) else None)
    exit_program_on_error(validate_input(args.start_time, args.stop_time, args.step_size, args.output))

    # Set up logging
    # TODO: Setup proper logging
    logging.basicConfig(
        filename=args.log,
        level=LOGGING_LEVELS[args.print],
        encoding="utf-8",
        format="%(asctime)s %(levelname)s (Line: %(lineno)d):  %(message)s",
        datefmt="%m/%d/%Y %I:%M:%S %p",
    )

    start_time = convert_date_to_jd(args.start_time)
    stop_time = convert_date_to_jd(args.stop_time)
    data_count = calculate_number_of_data_points(start_time, stop_time, args.step_size)
    try:
        step_size = calculate_step_size(start_time, stop_time, data_count)
    except ValueError as e:
        logging.critical(e)
        sys.exit(-2)

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
    if os.path.exists(args.output):
        os.remove(args.output)

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
                logging.debug(f"Line being parsed: {line}")
                output = line[:-1].split(", ")

                # Parse, store and write the data point
                logging.info(f"Output written: {output}")
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
    return data_points


if __name__ == "__main__":
    main()
