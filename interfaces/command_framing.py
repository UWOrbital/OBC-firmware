from ax25 import FrameType

from interfaces import RS_DECODED_DATA_SIZE
from interfaces.obc_gs_interface.aes128 import AES128
from interfaces.obc_gs_interface.ax25 import AX25
from interfaces.obc_gs_interface.commands import (
    CmdMsg,
    pack_command,
)
from interfaces.obc_gs_interface.fec import FEC


def command_frame(commands: list[CmdMsg]) -> bytes:
    """
    A function that frames the commands passed in with encryption and fec

    :param command: A list of commands to encrypt, fec and frame
    :return: The commands fully framed with fec and encryption. Note, these bytes are stuffed
    """
    # Instantiate our ax25 class to get ready to create frame
    ax25_proto = AX25("ATLAS", "AKITO")
    # Instantiate the fec class for forward error correction
    fec_coder = FEC()
    data = bytearray(b"")

    for command in commands:
        command_packed = bytearray(pack_command(command))
        # NOTE: We check its greater than the max size - 1 as we need the last byte to be 0x00
        if (len(data) + len(command_packed)) > RS_DECODED_DATA_SIZE - 1:
            break
        else:
            data += command_packed

    data_bytes = bytes(data)
    data_bytes = data_bytes.ljust(RS_DECODED_DATA_SIZE, b"\x00")

    # Instantaite the aes cipher with the same defaults from the c implementation
    aes_cipher = AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    # Encrypt data
    encrypted_data = aes_cipher.encrypt(data_bytes)
    # Encode data for error correction
    encode_data = fec_coder.encode(bytes(encrypted_data))
    # Create the frame
    send_frame = ax25_proto.encode_frame(encode_data, FrameType.I, 0)
    # Stuff the frame as per the standard
    send_frame_stuffed = ax25_proto.stuff(send_frame)

    return send_frame_stuffed
