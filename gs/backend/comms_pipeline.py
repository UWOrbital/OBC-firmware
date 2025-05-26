from argparse import ArgumentParser

import serial
from ax25 import FrameType

from interfaces import OBC_UART_BAUD_RATE, RS_DECODED_DATA_SIZE
from interfaces.command_framing import command_frame
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    CmdMsg,
    create_cmd_ping,
)
from interfaces.obc_gs_interface.fec import FEC


def send_command(command: CmdMsg, com_port: str) -> None:
    """
    A function to send a command up to the cube satellite
    """
    data = [command]
    send_bytes = command_frame(data)

    # Instantiate our ax25 class to get ready to create frame
    ax25_proto = AX25("ATLAS", "AKITO")
    # Instantiate the fec class for forward error correction
    fec_coder = FEC()

    with serial.Serial(
        com_port,
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        ser.write(send_bytes)
        print("Frame Sent")

        read_bytes = ser.read(300)

    frame_data_bytes = b""
    print("---- Stuffed Frame -----")
    print([hex(byte) for byte in read_bytes])
    stuffed_frame = bytes(read_bytes)
    frame_data_bytes = ax25_proto.unstuff(stuffed_frame)
    print("---- Unstuffed Frame -----")
    print([hex(byte) for byte in frame_data_bytes])
    print(len(frame_data_bytes))
    # NOTE: 17 (inclusive) to 272 (exclusive) is the range for info bytes that are needed for the decoding
    data_to_decode = fec_coder.decode(frame_data_bytes[17:272])
    # With the data decoded we need to add the rest of the data back to get a full frame
    decoded_data = bytes(frame_data_bytes[:17] + data_to_decode + frame_data_bytes[272:])

    # Now we can finally decode the frame and extract information
    rcv_frame = ax25_proto.decode_frame(decoded_data)
    frame_data = rcv_frame.data
    print("---- Frame Data (First 223 Bytes) -----")
    print([hex(byte) for byte in frame_data[:RS_DECODED_DATA_SIZE]])


def arg_parse() -> ArgumentParser:
    """
    Argument Parsing for the send and receive commands
    """
    # TODO: Add receive
    parser = ArgumentParser(
        prog="UW Orbital CLI",
        description="Sending and Receiving from the satellite using the command line",
        epilog="Help needs implementation",
    )
    parser.add_argument(
        "-c", "--command", required=True, dest="command", type=str, choices=[x.name for x in CmdCallbackId]
    )
    parser.add_argument("-t", "--timestamp", required=False, dest="timestamp", type=int)
    parser.add_argument("-rtc", "--rtc_sync_time", required=False, dest="rtc_sync", type=int)
    parser.add_argument("-lnp", "--log_next_pass", required=False, dest="log_level", type=int)

    return parser


if __name__ == "__main__":
    with serial.Serial(
        "/dev/ttyUSB0",
        baudrate=OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        ax25_proto = AX25("ATLAS", "AKITO")
        send_bytes = ax25_proto.encode_frame(None, FrameType.SABM, 0, True)
        send_bytes = ax25_proto.stuff(send_bytes)
        print([hex(byte) for byte in send_bytes])
        print(hex(send_bytes[0]))
        print([hex(byte) for byte in send_bytes[1:]])
        ser.write(send_bytes)
        print("Frame Sent")
        rcv_frame_bytes = ser.read(300)
        print([hex(byte) for byte in rcv_frame_bytes])
        rcv_frame_bytes = ax25_proto.unstuff(rcv_frame_bytes)
        rcv_frame = ax25_proto.decode_frame(rcv_frame_bytes)
        print(rcv_frame.control.frame_type)

    cmd_ping = create_cmd_ping()
    send_command(cmd_ping, "/dev/ttyUSB0")
