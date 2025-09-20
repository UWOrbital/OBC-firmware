from math import ceil
from pathlib import Path
from sys import argv
from time import sleep
from typing import Final

from serial import PARITY_NONE, STOPBITS_TWO, Serial, SerialException
from tqdm import tqdm

from gs.backend.obc_utils.command_utils import send_conn_request
from gs.backend.obc_utils.encode_decode import CommsPipeline
from interfaces import OBC_UART_BAUD_RATE, RS_DECODED_DATA_SIZE
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    CmdResponseErrorCode,
    ProgrammingSession,
    create_cmd_download_data,
    create_cmd_erase_app,
    create_cmd_ping,
    create_cmd_verify_crc,
    pack_command,
)
from interfaces.obc_gs_interface.commands.command_response_callbacks import parse_command_response
from interfaces.obc_gs_interface.commands.command_response_classes import CmdRes, FirmwareType

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


def write_command(
    ser: Serial,
    command: CmdCallbackId,
    app_data: bytes | None = None,
    iteration: int | None = None,
    is_last_packet: bool | None = None,
    firmware_type: FirmwareType | None = None,
) -> tuple[bool, CmdRes]:
    """
    Generates and writes command as well as handling command responses

    :param ser: The Serial object to communicate over UART with
    :param command: The command that needs to be send over UART
    :param app_data: The app binary in bytes for the CMD_DOWNLOAD_DATA
    :param iteration: The packet of app being written for CMD_DOWNLOAD_DATA
    """
    packed_command = b""

    cmd_print_response = False

    # TODO: Remove: Temporary variables till the sci bug during flash write gets sorted
    bytes_to_read = RS_DECODED_DATA_SIZE
    cmd_res_cutoff = 0

    match command:
        case CmdCallbackId.CMD_PING:
            packed_command = pack_command(create_cmd_ping()).ljust(RS_DECODED_DATA_SIZE, b"\x00")
        case CmdCallbackId.CMD_ERASE_APP:
            packed_command = pack_command(create_cmd_erase_app()).ljust(RS_DECODED_DATA_SIZE, b"\x00")
        case CmdCallbackId.CMD_DOWNLOAD_DATA:
            if app_data is not None and iteration is not None and is_last_packet is not None:
                packed_command = create_app_packet(iteration, app_data, is_last_packet)
                bytes_to_read = RS_DECODED_DATA_SIZE + 1
                cmd_res_cutoff = 1
            else:
                raise ValueError("Data and iteration need to be specified for the write command")
        case CmdCallbackId.CMD_VERIFY_CRC:
            packed_command = pack_command(create_cmd_verify_crc()).ljust(RS_DECODED_DATA_SIZE, b"\x00")
            cmd_print_response = True
        case _:
            raise ValueError("Command not supported")

    comms = CommsPipeline()

    if firmware_type == FirmwareType.APP:
        ser.write(comms.encode_frame(packed_command))

        if command != CmdCallbackId.CMD_EXEC_OBC_RESET:
            cmd_response_bytes = ser.read(bytes_to_read)
            print(cmd_response_bytes)
            frame = comms.decode_frame(cmd_response_bytes)

        if frame is not None:
            cmd_response = parse_command_response(frame.data[cmd_res_cutoff:])
        else:
            raise ValueError("Decoded frame is None")

    elif firmware_type == FirmwareType.BOOTLOADER:
        ser.write(packed_command)
        if command != CmdCallbackId.CMD_EXEC_OBC_RESET:
            cmd_response_bytes = ser.read(bytes_to_read)
            cmd_response = parse_command_response(cmd_response_bytes[cmd_res_cutoff:])

    else:
        raise ValueError("Invalid firmware type argument (Recieved no response)")

    if cmd_response.error_code != CmdResponseErrorCode.CMD_RESPONSE_SUCCESS or cmd_print_response:
        print(cmd_response)
        return (False, cmd_response)

    return (True, cmd_response)


def send_bin(
    file_path: str,
    com_port: str,
) -> None:
    """
    Sends .bin file over UART serial port

    :param file_path: Path to .bin file to be sent
    :param com_port: Com port for UART communication
    """

    file_obj = Path(file_path)
    app_bin = file_obj.read_bytes()

    commands_needed = ceil(len(app_bin) / COMMAND_DATA_SIZE)

    # Open serial port and write binary to device via UART
    with Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=PARITY_NONE,
        stopbits=STOPBITS_TWO,
        timeout=15,
    ) as ser:
        ping_response = None
        loaded_firmware = None

        send_conn_request(com_port)

        while True:
            ping_response = write_command(ser, CmdCallbackId.CMD_PING, firmware_type=FirmwareType.APP)
            if ping_response[0]:
                loaded_firmware = FirmwareType.APP
                break

            ping_response = write_command(ser, CmdCallbackId.CMD_PING, firmware_type=FirmwareType.BOOTLOADER)
            if ping_response[0]:
                loaded_firmware = FirmwareType.BOOTLOADER
                break

        if ping_response[1].error_code == 0x01:
            print("Bootloader loaded, continuing...")
        elif ping_response[1].error_code == 0x02:
            print("App loaded, resetting OBC to load bootloader...")
            if write_command(ser, CmdCallbackId.CMD_EXEC_OBC_RESET, firmware_type=loaded_firmware)[0]:
                print("Successful, continuing...")
        else:
            return

        if write_command(ser, CmdCallbackId.CMD_ERASE_APP, firmware_type=loaded_firmware)[0]:
            print("Erased App")
        else:
            return

        # We create a progress bar with the tqdm library
        progress_bar = tqdm(desc="Packets Written: ", total=commands_needed, dynamic_ncols=True)
        for i in range(commands_needed - 1):
            if write_command(ser, CmdCallbackId.CMD_DOWNLOAD_DATA, app_bin, i, False, firmware_type=loaded_firmware)[0]:
                progress_bar.update(1)
                ser.reset_output_buffer()
                ser.reset_input_buffer()
            else:
                return

        if write_command(
            ser, CmdCallbackId.CMD_DOWNLOAD_DATA, app_bin, commands_needed - 1, True, firmware_type=loaded_firmware
        )[0]:
            progress_bar.update(1)
            progress_bar.close()
        else:
            return

        if not write_command(ser, CmdCallbackId.CMD_VERIFY_CRC, firmware_type=loaded_firmware)[0]:
            return


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

        print("Starting Flashing Procedure...")
        send_bin(str(path), com_port)
        sleep(5)

    except SerialException:
        print("Invalid port entered")


if __name__ == "__main__":
    main()
