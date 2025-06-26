from math import ceil
from pathlib import Path
from sys import argv
from time import sleep
from typing import Final

from serial import PARITY_NONE, STOPBITS_TWO, Serial, SerialException

from interfaces import OBC_UART_BAUD_RATE, RS_DECODED_DATA_SIZE
from interfaces.obc_gs_interface.commands import (
    ProgrammingSession,
    create_cmd_download_data,
    create_cmd_erase_app,
    pack_command,
)

# Refer to the bl_command_callbacks.c for the number
COMMAND_DATA_SIZE: Final[int] = 208

# Refer to bl_config.h for the start address
APP_STARTING_ADDRESS: Final[int] = 0x00040000


def create_app_packet(packet_number: int, app_bin: bytes, is_last_packet: bool = False) -> bytes:
    """
    A helper function that creates an app packet to send to the bootloader

    :param packet_number: The current packet number being sent (starts from 0)
    :param app_bin: Bytes representing the application to be written to the bootloader
    :param is_last_packet: If the packet is the last one to be sent (i.e. it probably won't occupy the full 208 bytes of
                           length), set this to true
    :return: bytes that represent and app packet that the bootloader can receive
    """
    if is_last_packet:
        command_bytes = pack_command(
            create_cmd_download_data(
                ProgrammingSession.APPLICATION,
                len(app_bin[packet_number * COMMAND_DATA_SIZE :]),
                APP_STARTING_ADDRESS + packet_number * COMMAND_DATA_SIZE,
            )
        )
        print(len(app_bin) - packet_number * COMMAND_DATA_SIZE)
        return (command_bytes + app_bin[packet_number * COMMAND_DATA_SIZE :]).ljust(RS_DECODED_DATA_SIZE, b"\x00")
    else:
        command_bytes = pack_command(
            create_cmd_download_data(
                ProgrammingSession.APPLICATION,
                COMMAND_DATA_SIZE,
                APP_STARTING_ADDRESS + packet_number * COMMAND_DATA_SIZE,
            )
        )
        return command_bytes + app_bin[
            packet_number * COMMAND_DATA_SIZE : (packet_number + 1) * COMMAND_DATA_SIZE
        ].ljust(RS_DECODED_DATA_SIZE, b"\x00")


def send_bin(file_path: str, com_port: str) -> None:
    """
    Sends .bin file over UART serial port

    :param file_path: Path to .bin file to be sent
    :param com_port: Com port for UART communication
    """

    file_obj = Path(file_path)
    app_bin = file_obj.read_bytes()

    # Open serial port and write binary to device via UART
    with Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=PARITY_NONE,
        stopbits=STOPBITS_TWO,
        timeout=15,
    ) as ser:
        erase_command = pack_command(create_cmd_erase_app())
        ser.write(erase_command.ljust(RS_DECODED_DATA_SIZE, b"\x00"))
        ser.read(len("Erase success\r\n"))
        print("Erased App")
        sleep(2)

        commands_needed = ceil(len(app_bin) / 208)
        print(commands_needed)

        for i in range(commands_needed - 1):
            app_packet = create_app_packet(i, app_bin)
            print([hex(byte) for byte in app_packet])
            ser.write(app_packet)
            print(ser.read(len("Received packet\r\nWrite success\r\n")))
            ser.reset_output_buffer()
            ser.reset_input_buffer()

        app_packet = create_app_packet(commands_needed - 1, app_bin, True)
        ser.write(app_packet)
        print(ser.read(len("Received packet\r\nWrite success\r\n")))
        print("App Successfully Written")
        print(ser.read(100))


def main() -> None:
    """
    A function that initializes the com port and path to update the app
    """
    if len(argv) != 3:
        print("Two arguments needed: Com Port and Application File Path")
        return

    try:
        com_port = str(argv[1])
        ser = Serial(com_port)
        print("Comm port set to: " + str(ser.name))
        ser.close()
        path = Path(argv[2]).resolve()
        if not path.is_file() and path.suffix != ".bin":
            print("Invalid file path")
            return

        print("Sending app...")
        # file_obj = Path(path)
        # app_bin = file_obj.read_bytes()
        # print(app_bin)
        send_bin(str(path), com_port)
    except SerialException:
        print("Invalid port entered")


if __name__ == "__main__":
    main()
