from argparse import ArgumentParser
from typing import Final

import serial
from ax25 import FrameType

from interfaces.obc_gs_interface.aes128 import AES128
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    CmdMsg,
    create_cmd_ping,
    pack_command,
    unpack_command,
)
from interfaces.obc_gs_interface.fec import FEC

_MAX_DECODED_DATA_LEN: Final[int] = 223
_MAX_ENCODED_DATA_LEN: Final[int] = 255
_OBC_UART_BAUD_RATE: Final = 115200


def send_command(command: CmdMsg, com_port: str) -> None:
    """
    A function to send a command up to the cube satellite
    """
    # Instantiate our ax25 class to get ready to create frame
    ax25_proto = AX25("ATLAS", "AKITO")
    # Instantiate the fec class for forward error correction
    fec_coder = FEC()

    data = pack_command(command).ljust(223, b"\x00")

    # Instantaite the aes cipher with the same defaults from the c implementation
    aes_cipher = AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    # Encrypt data
    encrypted_data = aes_cipher.encrypt(data)
    # Encode data for error correction
    encode_data = fec_coder.encode(bytes(encrypted_data))
    # Create the frame
    send_frame = ax25_proto.encode_frame(encode_data, FrameType.I, 0)
    # Stuff the frame as per the standard
    send_frame_stuffed = ax25_proto.stuff(send_frame)

    with serial.Serial(
        com_port,
        baudrate=_OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=5,
    ) as ser:
        ser.write(send_frame_stuffed)
        print("Frame Sent")

        read_bytes = ser.read(300)

    print([hex(byte) for byte in read_bytes])
    stuffed_frame = bytes(read_bytes)
    data = ax25_proto.unstuff(stuffed_frame)
    print([hex(byte) for byte in data])
    print(len(data))
    # NOTE: 17 (inclusive) to 272 (exclusive) is the range for info bytes that are needed for the decoding
    data_to_decode = fec_coder.decode(data[17:272])
    # With the data decoded we need to add the rest of the data back to get a full frame
    decoded_data = bytes(data[:17] + data_to_decode + data[272:])

    # Now we can finally decode the frame and extract information
    rcv_frame = ax25_proto.decode_frame(decoded_data)
    frame_data = rcv_frame.data

    cmd_list = unpack_command(frame_data[:223])

    if len(cmd_list) != 0:
        print(cmd_list[0].id)


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
        baudrate=_OBC_UART_BAUD_RATE,
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
