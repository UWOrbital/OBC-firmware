import time
from argparse import ArgumentParser
from typing import Final

import serial
from ax25 import FrameType

from interfaces.obc_gs_interface.aes128 import AES128
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.commands import (
    CmdCallbackId,
    CmdMsg,
    CmdUnpackedReponse,
    pack_command,
    unpack_command_response,
)
from interfaces.obc_gs_interface.fec import FEC

_MAX_DECODED_DATA_LEN: Final[int] = 223
_MAX_ENCODED_DATA_LEN: Final[int] = 255
_OBC_UART_BAUD_RATE: Final = 115200


def send_command(command: CmdMsg, com_port: str) -> None:
    """
    A function to send a command up to the cube satellite
    """
    # Pack and pad command to the decoded data length
    packed_command = pack_command(command).ljust(_MAX_DECODED_DATA_LEN, b"\x00")

    # Instantiate our ax25 class to get ready to create frame
    ax25_proto = AX25("ATLAS", "AKITO")
    # Instantiate the fec class for forward error correction
    fec_coder = FEC()
    # Instantaite the aes cipher with the same defaults from the c implementation
    aes_cipher = AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )

    # Encrypt command
    encrypted_command = aes_cipher.encrypt(packed_command)
    # Encode command for error correction
    encode_data = fec_coder.encode(bytes(encrypted_command))
    # Create the frame for the command
    send_frame = ax25_proto.encode_frame(encode_data, FrameType.I, 0)
    # Stuff the frame as per the standard
    send_frame_stuffed = ax25_proto.stuff(send_frame)

    with serial.Serial(
        com_port,
        baudrate=_OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=1,
    ) as ser:
        ser.write(send_frame_stuffed)
        time.sleep(0.1)
        print("Frame Sent")


def receive_reponse(wait_time: int, com_port: str) -> CmdUnpackedReponse:
    """
    A function to wait for a response given a timeout
    """
    with serial.Serial(
        com_port,
        baudrate=_OBC_UART_BAUD_RATE,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_TWO,
        timeout=wait_time,
    ) as ser:
        read_bytes = bytearray(b"")
        start_end_flag = bytearray(bytes.fromhex("7E"))
        while read_bytes[0] != start_end_flag:
            if ser.read() == start_end_flag:
                read_bytes += start_end_flag

        while read_bytes[-1] != start_end_flag:
            read_bytes += ser.read()

        read_bytes += start_end_flag

    stuffed_frame = bytes(read_bytes)

    # Instantiate the ax25 class to unstuff
    ax25_proto = AX25("ATLAS", "AKITO")
    data = ax25_proto.unstuff(stuffed_frame)

    # Instantiate FEC class to error correct
    fec_coder = FEC()
    # NOTE: 17 (inclusive) to 272 (exclusive) is the range for info bytes that are needed for the decoding
    data_to_decode = fec_coder.decode(data[17:272])
    # With the data decoded we need to add the rest of the data back to get a full frame
    decoded_data = bytes(data[:17] + data_to_decode + data[272:])

    # Now we can finally decode the frame and extract information
    rcv_frame = ax25_proto.decode_frame(decoded_data)
    frame_data = rcv_frame.data

    # Instantiate the aes class with defaults from the c implementation and decrypt the data
    aes_cipher = AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    decrypted_data = aes_cipher.decrypt(frame_data)[:16]

    command = unpack_command_response(decrypted_data)

    return command


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
    arg_parser = arg_parse()
    args = arg_parser.parse_args()
