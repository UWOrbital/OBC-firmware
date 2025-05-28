import time
from argparse import ArgumentParser, RawDescriptionHelpFormatter
from typing import Final

import serial
from ax25 import FrameType

from interfaces import OBC_UART_BAUD_RATE, RS_DECODED_DATA_SIZE, RS_ENCODED_DATA_SIZE
from interfaces.command_framing import command_frame
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    CmdMsg,
    create_cmd_ping,
)
from interfaces.obc_gs_interface.fec import FEC

_PADDING_REQUIRED: Final[int] = 300


def send_command(command: CmdMsg, com_port: str, print_data: bool = False) -> None:
    """
    A function to send a command up to the cube satellite and awaits a response

    :param command: A command as a CmdMsg structure to be passed into a function
    :param com_port: The port that the board is connected to (i.e. which port the program should use)
    """
    # Command frame takes in a list of commands to pack thus we create that list here.
    data = [command]

    # We pad the data to an amount that the OBC expects (See handleUplinkingState function in comms_manager.c)
    send_bytes = command_frame(data).ljust(_PADDING_REQUIRED, b"\x00")

    # Instantiate our ax25 class to get ready to create frame
    ax25_proto = AX25("ATLAS", "AKITO")
    # Instantiate the fec class for forward error correction
    fec_coder = FEC()

    # Print out the bytes we send
    if print_data:
        print("----- Send Frame Bytes -----")
        print([hex(byte) for byte in send_bytes])
        print("Length: " + str(len(send_bytes)))

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
        if print_data:
            print("Frame Sent")

        # Await a response (This is set to an arbitrary large amount as the logger and stats collector might
        # send through data)
        rcv_frame_bytes = ser.read(10000)
        start_index = rcv_frame_bytes.find(b"\x7e")
        end_index = rcv_frame_bytes.rfind(b"\x7e")

        # Print out any of the data send before and after
        if start_index != 0 and print_data:
            print("----- Data Received before and after Frame ----- ")
            print(rcv_frame_bytes[:start_index].decode("utf-8"))
            print(rcv_frame_bytes[end_index:].decode("utf-8"))

        # Isolate the frame
        read_bytes = rcv_frame_bytes[start_index : end_index + 1]

    frame_data_bytes = b""

    # Print out the frame received
    if print_data:
        print("----- Stuffed Frame -----")
        print([hex(byte) for byte in read_bytes])
        print("Length: " + str(len(read_bytes)))

    stuffed_frame = bytes(read_bytes)

    # Check if there actually was a frame received (i.e. the length is not 0) and then only unstuff
    if len(read_bytes) != 0:
        frame_data_bytes = ax25_proto.unstuff(stuffed_frame)

    # Print out the unstuffed frame
    if print_data:
        print("---- Unstuffed Frame -----")
        print([hex(byte) for byte in frame_data_bytes])
        print("Length: " + str(len(frame_data_bytes)))

    # Check if ther ewas a frame received and decode it
    if len(read_bytes) != 0:
        # If a frame is bigger than encoded data size it is an I frame else it is a U frame and we decode accordinlgy
        if len(read_bytes) > RS_ENCODED_DATA_SIZE:
            # NOTE: 17 (inclusive) to 272 (exclusive) is the range for info bytes that are needed for the decoding
            data_to_decode = fec_coder.decode(frame_data_bytes[17:272])
            # With the data decoded we need to add the rest of the data back to get a full frame
            decoded_data = bytes(frame_data_bytes[:17] + data_to_decode + frame_data_bytes[272:])
            # Now we can finally decode the frame and extract information
            rcv_frame = ax25_proto.decode_frame(decoded_data)
            frame_data = rcv_frame.data
            if print_data and frame_data is not None:
                print("---- Frame Data (First 223 Bytes) -----")
                print([hex(byte) for byte in frame_data[:RS_DECODED_DATA_SIZE]])
        else:
            rcv_frame = ax25_proto.decode_frame(frame_data_bytes)
            if print_data:
                print("---- Frame Data-----")
                print("Frame type: " + str(rcv_frame.control.frame_type))


def send_conn_request(com_port: str, print_data: bool = False) -> None:
    """
    Sends the initial connection request to the board
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

        if print_data:
            print("----- Send Frame Bytes -----")
            print([hex(byte) for byte in send_bytes])
            print("Length: " + str(len(send_bytes)))
            print("Frame Sent")
        rcv_frame_bytes = ser.read(10000)
        start_index = rcv_frame_bytes.find(b"\x7e")
        end_index = rcv_frame_bytes.rfind(b"\x7e")

        # Print out any of the data send before and after
        if start_index != 0 and print_data:
            print("----- Data Received before and after Frame ----- ")
            print(rcv_frame_bytes[:start_index].decode("utf-8"))
            print(rcv_frame_bytes[end_index:].decode("utf-8"))

        rcv_frame_bytes = rcv_frame_bytes[start_index : end_index + 1]
        rcv_frame_bytes = ax25_proto.unstuff(rcv_frame_bytes)
        rcv_frame = ax25_proto.decode_frame(rcv_frame_bytes)
        if print_data:
            print("---- Frame Data-----")
            print("Length: " + str(len(rcv_frame_bytes)))
            print("Frame Type: " + str(rcv_frame.control.frame_type))


def arg_parse() -> ArgumentParser:
    """
    Argument Parsing for the send and receive commands
    """
    # TODO: Add receive
    parser = ArgumentParser(
        prog="UW Orbital CLI",
        formatter_class=RawDescriptionHelpFormatter,
        epilog="Note: Before using this command, the com port must be configured and a connection request must be sent",
    )
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
    parser.add_argument(
        "-rtc",
        "--rtc_sync_time",
        required=False,
        dest="rtc_sync",
        type=int,
        help="The time that the should be used to sync",
    )
    parser.add_argument(
        "-lnp",
        "--log_next_pass",
        required=False,
        dest="log_level",
        type=int,
        help="The log level for when the logs are downlinked",
    )

    return parser


if __name__ == "__main__":
    send_conn_request("/dev/ttyACM0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_downlink_logs_next_pass(1)
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    cmd_to_send = create_cmd_ping()
    send_command(cmd_to_send, "/dev/ttyACM0")
    time.sleep(0.1)
    # cmd_to_send = create_cmd_rtc_sync(1748380111)
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_mirco_sd_format()
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_downlink_telem()
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # cmd_to_send = create_cmd_uplink_disc()
    # send_command(cmd_to_send, "/dev/ttyUSB0")
    # time.sleep(0.1)
    # send_command(cmd_ping, "/dev/ttyUSB0")
    fec_coder = FEC()
    fec_coder.destroy()
