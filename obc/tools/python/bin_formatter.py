import struct
from argparse import ArgumentParser
from dataclasses import dataclass
from pathlib import Path
from typing import Final

import serial

OBC_UART_BAUD_RATE: Final = 115200


# Define header class. This is the header that will be appended to the .bin file
@dataclass
class BootloaderHeader:
    version: int
    bin_size: int

    def serialize(self) -> bytes:
        """Returns the serialized version of the object"""
        return struct.pack("<II", self.version, self.bin_size)


def create_bin(input_path: str, input_version: int) -> str:
    """
    Appends custom header data to a .bin file

    :param input_path: Path to .bin file to be modified and sent
    :param input_version: Header version
    :return: The output file path as a string
    """

    input_obj = Path(input_path)

    input_bin_data = input_obj.read_bytes()
    bin_len = len(input_bin_data)
    header = BootloaderHeader(
        version=input_version, bin_size=bin_len
    )  # Create header struct
    header_bytes = header.serialize()

    output_path = input_path.replace(".bin", "_formatted.bin")
    output_obj = Path(output_path)
    output_obj.write_bytes(header_bytes + input_bin_data)

    return output_path


def send_bin(file_path: str, com_port: str) -> None:
    """
    Sends .bin file over UART serial port

    :param file_path: Path to .bin file to be sent
    :param com_port: Com port for UART communication
    """

    file_obj = Path(file_path)

    # Open serial port and write binary to device via UART
    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        data = file_obj.read_bytes()
        ser.write(data)


def arg_parse() -> ArgumentParser:
    """Returns the argument parser

    :return: Parser object
    """
    parser = ArgumentParser(description="Append custom data to .bin and send")

    # Add arguments
    parser.add_argument(
        "-i",
        required=True,
        dest="input_path",
        type=str,
        help="Path to the input .bin file.",
    )
    parser.add_argument(
        "-p", required=True, dest="port", type=str, help="Serial port number"
    )
    parser.add_argument(
        "-v",
        dest="version",
        type=int,
        default=0,
        help="Version of the application. Default is 0",
    )

    return parser


def main() -> None:
    arg_parser = arg_parse()
    args = arg_parser.parse_args()

    output_file = create_bin(args.input_path, args.version)
    send_bin(output_file, args.port)


if __name__ == "__main__":
    main()
