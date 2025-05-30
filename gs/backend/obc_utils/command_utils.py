from argparse import ArgumentError, ArgumentParser
from collections.abc import Callable
from typing import Final

import serial
from ax25 import Frame, FrameType

from gs.backend.obc_utils.encode_decode import decode, encode
from interfaces import (
    OBC_UART_BAUD_RATE,
)
from interfaces.command_framing import command_multi_pack
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    CmdMsg,
    create_cmd_downlink_logs_next_pass,
    create_cmd_downlink_telem,
    create_cmd_end_of_frame,
    create_cmd_exec_obc_reset,
    create_cmd_mirco_sd_format,
    create_cmd_ping,
    create_cmd_rtc_sync,
    create_cmd_uplink_disc,
)

_PADDING_REQUIRED: Final[int] = 300


def send_command(args: str, com_port: str) -> Frame | None:
    """
    A function to send a command up to the cube satellite and awaits a response

    :param command: A command as a CmdMsg structure to be passed into a function
    :param com_port: The port that the board is connected to (i.e. which port the program should use)
    :param print_data: Whether the function should print to console or not
    """
    command = generate_command(args)

    if command is None:
        return None

    # command_multi_pack takes in a list of commands to pack thus we create that list here.
    data = [command]

    # We pad the data to an amount that the OBC expects (See handleUplinkingState function in comms_manager.c)
    send_bytes = encode(command_multi_pack(data)).ljust(_PADDING_REQUIRED, b"\x00")

    # Initialize pyserial
    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        # Reset the output buffer just in case
        ser.reset_output_buffer()

        # Send the frames to the board
        ser.write(send_bytes)
        print("Frame Sent")

        # Await a response (This is set to an arbitrary large amount as the logger and stats collector might
        # send through data)
        read_bytes = ser.read(10000)
        start_index = read_bytes.find(b"\x7e")
        end_index = read_bytes.rfind(b"\x7e")

        with open("obc_utils/logs.txt", "a") as file:
            file.write(str(read_bytes[:start_index].decode("utf-8")))
            file.write(str(read_bytes[end_index + 1 :].decode("utf-8")))

        # Isolate the frame
        rcv_frame_bytes = read_bytes[start_index : end_index + 1]

        # Check if the frame is an I frame
        if len(rcv_frame_bytes) > 255:
            rcv_frame = decode(rcv_frame_bytes)
        else:
            ax25 = AX25("ATLAS", "AKITO")
            rcv_frame = ax25.decode_frame(rcv_frame_bytes)

        return rcv_frame


def send_conn_request(com_port: str) -> Frame:
    """
    Sends the initial connection request to the board

    :param com_port: The port which the function should use to send and receive on
    """
    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        ax25_proto = AX25("ATLAS", "AKITO")
        send_bytes = ax25_proto.encode_frame(None, FrameType.SABM, 0, True)
        send_bytes = ax25_proto.stuff(send_bytes)
        ser.write(send_bytes.ljust(30, b"\x00"))

        rcv_frame_bytes = ser.read(10000)
        start_index = rcv_frame_bytes.find(b"\x7e")
        end_index = rcv_frame_bytes.rfind(b"\x7e")

        with open("obc_utils/logs.txt", "a") as file:
            file.write(str(rcv_frame_bytes[:start_index].decode("utf-8")))
            file.write(str(rcv_frame_bytes[end_index + 1 :].decode("utf-8")))

        rcv_frame_bytes = rcv_frame_bytes[start_index : end_index + 1]
        rcv_frame_bytes = ax25_proto.unstuff(rcv_frame_bytes)
        rcv_frame = ax25_proto.decode_frame(rcv_frame_bytes)
        return rcv_frame


def arg_parse() -> ArgumentParser:
    """
    Argument Parsing for the send and receive commands
    """
    parser = ArgumentParser(add_help=False, exit_on_error=False)
    parser.add_argument(
        "-c",
        "--command",
        required=True,
        dest="command",
        type=str,
        choices=[x.name for x in CmdCallbackId],
        help="The command to send to the board",
    )
    parser.add_argument(
        "-t",
        "--timestamp",
        required=False,
        dest="timestamp",
        type=int,
        help="The time stamp for when the command should execute",
    )

    return parser


# The following are specific command parsers with one argument
# NOTE: Updated these and always set the destinations of variables to arg1, arg2, arg3 and make the arguments required
# Additionally, keep the same arguments when initiailizing the ArgumentParser Class
def parse_cmd_rtc_time_sync() -> ArgumentParser:
    """
    A function to parse the argument for the rtc_time_sync command

    :param args: Arguments passed in (rtc_time_sync: unixTime)
    """
    parent_parser = arg_parse()
    parser = ArgumentParser(parents=[parent_parser], exit_on_error=False)
    parser.add_argument(
        "-rtc",
        "--rtc_sync_time",
        required=True,
        dest="arg1",
        type=int,
        help="The time that the should be used to sync",
    )
    return parser


def parse_cmd_downlink_logs_next_pass() -> ArgumentParser:
    """
    A function to parse the argument for the downlink_logs_next_pass command

    :param args: Arguments passed in (log_level: int)
    """
    parent_parser = arg_parse()
    parser = ArgumentParser(parents=[parent_parser], exit_on_error=False)
    parser.add_argument(
        "-lnp",
        "--log_next_pass",
        required=True,
        dest="arg1",
        type=int,
        help="The log level for when the logs are downlinked",
    )
    return parser


# End of specific command parsers


def generate_command(args: str) -> CmdMsg | None:
    """
    A function that parsed command arguments and returns the corresponding command frame

    :param args: The arguments to parse to create the command
    """
    arguments = args.split()
    command = CmdMsg()

    # These are a list of parsers for commands that require additional arguments
    # NOTE: Update this list when another command with a specific parser is required
    child_parsers = [parse_cmd_downlink_logs_next_pass, parse_cmd_rtc_time_sync]

    # A list of Command factories for all commands
    # NOTE: Update these when a command is added and make sure to keep them in the order that the commands are described
    # in the CmdCallbackId Enum
    commmand_factories: list[Callable[..., CmdMsg]] = [
        create_cmd_end_of_frame,
        create_cmd_exec_obc_reset,
        create_cmd_rtc_sync,
        create_cmd_downlink_logs_next_pass,
        create_cmd_mirco_sd_format,
        create_cmd_ping,
        create_cmd_downlink_telem,
        create_cmd_uplink_disc,
    ]

    for func in child_parsers:
        try:
            parser = func()
            command_args = parser.parse_args(arguments)
        except ArgumentError:
            continue
        else:
            try:
                command_enum = CmdCallbackId[command_args.command]
            except KeyError:
                print("Invalid Command")
                return None
            else:
                # We check how many arguments have been parsed and call functions accordingly
                if hasattr(command_args, "arg3"):
                    command = commmand_factories[command_enum.value](
                        command_args.arg1, command_args.arg2, command_args.arg3, command_args.timestamp
                    )
                elif hasattr(command_args, "arg2"):
                    command = commmand_factories[command_enum.value](
                        command_args.arg1, command_args.arg2, command_args.timestamp
                    )
                elif hasattr(command_args, "arg1"):
                    command = commmand_factories[command_enum.value](command_args.arg1, command_args.timestamp)
                return command

    parser = arg_parse()
    try:
        command_args = parser.parse_args(arguments)
    except ArgumentError:
        print("Invalid Commands")
        return None
    else:
        try:
            command_enum = CmdCallbackId[command_args.command]
        except KeyError:
            print("Invalid Command")
            return None
        else:
            command = commmand_factories[command_enum.value](command_args.timestamp)
            return command


def poll(com_port: str, print_console: bool = False) -> None:
    """
    A function that is supposed to run in the background to keep receiving logs from the board
    """
    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        while True:
            with open("obc_utils/logs.txt", "a") as file:
                data = ser.read(10000).decode("utf-8")
                file.write(data)
                if print_console and len(data) != 0:
                    print(data)
