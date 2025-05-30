from interfaces import RS_DECODED_DATA_SIZE
from interfaces.obc_gs_interface.commands import (
    CmdMsg,
    pack_command,
)


def command_multi_pack(commands: list[CmdMsg]) -> bytes:
    """
    A function that generates bytes based on the commands passed in

    :param command: A list of commands to convert to bytes
    :return: The commands fully packed in sequence and ready to be encoded. Note, this will always be padded with 0x00
             to 223 bytes
    """
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

    return data_bytes
