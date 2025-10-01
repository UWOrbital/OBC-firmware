import asyncio
import dataclasses
import logging
import struct
from argparse import ArgumentParser
from pathlib import Path
from typing import Final

import serial

OBC_UART_BAUD_RATE: Final = 115200

logging.basicConfig(level=logging.INFO, format="%(asctime)s - %(message)s")
bin_header_logger = logging.getLogger("bin_header_logger")
log_reader = logging.getLogger("log_reader")


@dataclasses.dataclass
class BootloaderHeader:
    """Header for the application binary"""

    version: int
    bin_size: int

    HEADER_FMT: Final[str] = "<II"

    def serialize(self) -> bytes:
        """Returns the serialized version of the object"""
        return struct.pack(BootloaderHeader.HEADER_FMT, self.version, self.bin_size)

    @staticmethod
    def get_header_size() -> int:
        """Returns the size of the header in bytes"""
        return struct.calcsize(BootloaderHeader.HEADER_FMT)

    def __str__(self) -> str:
        """Returns a string representation of the object"""
        return f"Header Size: {BootloaderHeader.get_header_size()}, Version: {self.version}, Bin Size: {self.bin_size}"


def create_bin(input_path: str, input_version: int) -> str:
    """
    Appends custom header data to a .bin file

    :param input_path: Path to .bin file to be modified and sent
    :param input_version: Header version
    :return: The output file path as a string
    """
    program_bin = Path(input_path).read_bytes()
    program_size_bytes = len(program_bin)

    header = BootloaderHeader(version=input_version, bin_size=program_size_bytes)
    header_bytes = header.serialize()

    bin_header_logger.info(header)

    output_path = input_path.replace(".bin", "_formatted.bin")
    output_obj = Path(output_path)
    output_obj.write_bytes(header_bytes + program_bin)

    return output_path


async def send_bin(file_path: str, com_port: str) -> None:
    """
    Sends .bin file over UART serial port

    :param file_path: Path to .bin file to be sent
    :param com_port: Com port for UART communication
    """

    file_obj = Path(file_path)
    data = file_obj.read_bytes()
    if len(data) < BootloaderHeader.get_header_size():
        print("File too small to contain header. Exiting...")
        return

    # Open serial port and write binary to device via UART
    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        # TODO: Improve transfer protocol

        # Start program download
        ser.write("d".encode("ascii"))
        await asyncio.sleep(0.1)

        # Send header
        ser.write(data[0 : BootloaderHeader.get_header_size()])
        await asyncio.sleep(0.1)

        # Wait for user to initiate transfer
        while input("Enter 1 to start program transfer: ") != "1":
            pass

        # Bootloader expects a 'D' to be sent before the app
        ser.write("D".encode("ascii"))
        await asyncio.sleep(0.1)

        # Send app in chunks of 128 bytes
        total_bytes_to_write = len(data) - BootloaderHeader.get_header_size()
        num_bytes_written = 0
        while num_bytes_written < total_bytes_to_write:
            chunk_size = 128

            if total_bytes_to_write - num_bytes_written >= chunk_size:
                ser.write(data[8 + num_bytes_written : 8 + num_bytes_written + chunk_size])
                num_bytes_written += chunk_size
                await asyncio.sleep(0.1)
            else:
                ser.write(data[num_bytes_written + 8 :])
                num_bytes_written += total_bytes_to_write - num_bytes_written

            print(f"{num_bytes_written}/{total_bytes_to_write} bytes sent")

        print("Done writing app")


async def read_log(com_port: str) -> None:
    """
    Reads logs from a specific port

    :param com_port: Com port for UART communication
    """

    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        try:
            while True:
                await asyncio.sleep(0.1)
                log_line = ser.readline().decode("utf-8")
                if log_line:
                    log_reader.info(log_line)
        except serial.SerialException as e:
            log_reader.exception(e)


def arg_parse() -> ArgumentParser:
    """
    Returns the argument parser

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
    parser.add_argument("-p", required=True, dest="port", type=str, help="Serial port number")

    parser.add_argument(
        "-v",
        dest="version",
        type=int,
        default=0,
        help="Version of the application. Default is 0",
    )

    return parser


async def main() -> None:
    """Entry point to script"""
    arg_parser = arg_parse()
    args = arg_parser.parse_args()
    output_file = create_bin(args.input_path, args.version)
    await asyncio.gather(
        send_bin(output_file, args.port),
        read_log(args.port),
    )


if __name__ == "__main__":
    asyncio.run(main())
