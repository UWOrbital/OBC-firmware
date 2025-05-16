from ctypes import CDLL, POINTER, Structure, Union, c_bool, c_float, c_uint, c_uint8, c_uint32, pointer
from enum import IntEnum
from pathlib import Path
from typing import Final

# The shared object file we are using the access the c functions via ctypes
path = (Path(__file__).parent / "../../../build_gs/interfaces/libobc-gs-interface.so").resolve()
pack_unpack = CDLL(str(path))


## Ctypes Declerations for CommandPackUnpack


class RtcSyncCmdData(Structure):
    """
    The python equivalent class for the rtc_sync_cmd_data_t structure in the C implementation
    """

    _fields_ = [("unixTime", c_uint32)]


class DownlinkLogsNextPassCmdData(Structure):
    """
    The python equivalent class for the downlink_logs_next_pass_cmd_data_t structure in the C implementation
    """

    _fields_ = [("logLevel", c_uint8)]


class _U(Union):
    """
    Union class needed to create the CmdMsgType Class
    """

    _fields_ = [("rtcSync", RtcSyncCmdData), ("downlinkLogsNextPass", DownlinkLogsNextPassCmdData)]


class CmdMsg(Structure):
    """
    The python equivalent class for the cmd_msg_t structure in the C implementation
    NOTE: This class has a union so initialize accordingly
    """

    _anonymous_ = ("u",)
    _fields_ = [("u", _U), ("timestamp", c_uint32), ("isTimeTagged", c_bool), ("id", c_uint)]


pack_unpack.unpackCmdMsg.argtypes = (POINTER(c_uint8 * 16), POINTER(c_uint32), POINTER(CmdMsg))
pack_unpack.unpackCmdMsg.restype = c_uint

pack_unpack.packCmdMsg.argtypes = (POINTER(c_uint8 * 16), POINTER(c_uint32), POINTER(CmdMsg), POINTER(c_uint8))
pack_unpack.packCmdMsg.restype = c_uint


## Ctypes Declerations for CommandResponsePackUnpack


# NOTE: Just like the C implementation, this is a sample implementation. Add implemetnations as command responses are
# made
class ObcCmdResetResponse(Structure):
    """
    The python equivalent class for the obc_cmd_reset_response_t structure in the C implementation
    NOTE: This is a sample
    NOTE: This class uses floats which means it has floating point precision. This should not be a problem in most cases
    """

    _fields_ = [("data1", c_float), ("data2", c_uint32)]


# NOTE: This only has the sample response so add more response structures as they get implemented to this union
class _UR(Union):
    """
    The union needed to create the cmd_unpacked_response_t type in python
    NOTE: Add response structures as they get implemented to this union
    """

    _fields_ = [("obcResetResponse", ObcCmdResetResponse)]


# NOTE: No modifications to this class are necessary when adding new responses
class CmdUnpackedReponse(Structure):
    """
    The python equivalent class for the cmd_unpacked_response_t structure in the C implementation
    """

    _anonymous_ = ("u",)
    _fields_ = [("errCode", c_uint), ("cmdId", c_uint), ("u", _UR)]


pack_unpack.packCommandResponse.argtypes = (POINTER(CmdUnpackedReponse), POINTER(c_uint8 * 16))
pack_unpack.packCommandResponse.restype = c_uint

pack_unpack.unpackCommandResponse.argtypes = (POINTER(c_uint8 * 16), POINTER(CmdUnpackedReponse))
pack_unpack.unpackCommandResponse.restype = c_uint


## Enum Declerations


class CmdCallbackId(IntEnum):
    """
    Enums corresponding to the C implementation of cmd_callback_id_t
    """

    CMD_END_OF_FRAME = 0
    CMD_EXEC_OBC_RESET = 1
    CMD_RTC_SYNC = 2
    CMD_DOWNLINK_LOGS_NEXT_PASS = 3
    CMD_MICRO_SD_FORMAT = 4
    CMD_PING = 5
    CMD_DOWNLINK_TELEM = 6
    CMD_UPLINK_DISC = 7
    NUM_CMD_CALLBACKS = 8


class CmdResponseErrorCode(IntEnum):
    """
    Enums corresponding to the C implementation of the cmd_response_error_code_t
    """

    CMD_RESPONSE_SUCCESS = 0
    CMD_RESPONSE_ERROR = 1


## Class Implementation for CommandPackUnpack


_MAX_CMD_MSG_SIZE: Final[int] = 16
_PACK_OFFSET_INITIAL: Final[int] = 0
_UNPACK_OFFSET_INITIAL: Final[int] = 0
_NUM_PACKED_INITIAL: Final[int] = 0


def pack_command(cmd_msg: CmdMsg) -> bytes:
    """
    This takes in a command message to be packed (see the C implementation for more on how that's exactly done)
    NOTE: When the class is initialized, it will use internal variables to keep a running count of the packOffset
    and numPacked parameters from the C implementation.

    :param cmd_msg: A c-style structure that hold the command message
    :return: Bytes of the packed message
    """
    buffer = (c_uint8 * _MAX_CMD_MSG_SIZE)(*([0] * 16))
    res = pack_unpack.packCmdMsg(
        pointer(buffer),
        pointer(c_uint32(_PACK_OFFSET_INITIAL)),
        pointer(cmd_msg),
        pointer(c_uint8(_NUM_PACKED_INITIAL)),
    )

    if res != 0:
        raise ValueError("Could not pack command. OBC Error Code: " + str(res))

    return bytes(buffer)


def unpack_command(cmd_msg_packed: bytes) -> CmdMsg:
    """
    This takes in a data bytes to be unpacked into a command message (see the C implementation for more on how
    that's exactly done)
    NOTE: When the class is initialized, it will use internal variables to keep a running count of the unpackOffset
    parameter from the C implementation.

    :param cmd_msg_packed: Bytes of an already encoded message
    :return: An unpacked command message in the form of a structure
    """
    if len(cmd_msg_packed) > _MAX_CMD_MSG_SIZE:
        raise ValueError("The encoded command data to unpack is too long")

    buffer_elements = list(cmd_msg_packed)
    buff = (c_uint8 * _MAX_CMD_MSG_SIZE)(*buffer_elements)
    cmd_msg = CmdMsg()

    res = pack_unpack.unpackCmdMsg(pointer(buff), pointer(c_uint32(_UNPACK_OFFSET_INITIAL)), pointer(cmd_msg))

    if res != 0:
        raise ValueError("Could not unpack command. OBC Error Code: " + str(res))

    return cmd_msg


## Class implementation for CommandReponsePackUnpack


_MAX_REPONSE_PACKED_SIZE: Final[int] = 16


def pack_command_response(cmd_msg_response: CmdUnpackedReponse) -> bytes:
    """
    This takes a command message reponse to pack it (see the C implementation for more on how that's exactly done)

    :param cmd_msg_response: A c-style structure that hold the unpacked command message response
    :return: Bytes of the packed commmand response
    """
    buffer = (c_uint8 * _MAX_REPONSE_PACKED_SIZE)(*([0] * 16))
    res = pack_unpack.packCommandResponse(pointer(cmd_msg_response), pointer(buffer))

    if res != 0:
        raise ValueError("Could not pack command response. OBC Error Code: " + str(res))

    return bytes(buffer)


def unpack_command_response(cmd_msg_packed: bytes) -> CmdUnpackedReponse:
    """
    This takes in a bytes of data to be unpacked into a command response (see the C implementation for more on how
    that's exactly done)

    :param cmd_msg_packed: Bytes of an already encoded message
    :return: An unpacked command message in the form of a structure
    """
    if len(cmd_msg_packed) > _MAX_REPONSE_PACKED_SIZE:
        raise ValueError("The encoded command reponse data to unpack is too long")

    buffer_elements = list(cmd_msg_packed)
    buff = (c_uint8 * _MAX_REPONSE_PACKED_SIZE)(*buffer_elements)
    cmd_msg_response = CmdUnpackedReponse()

    res = pack_unpack.unpackCommandResponse(pointer(buff), pointer(cmd_msg_response))

    if res != 0:
        raise ValueError("Could not unpack command response. OBC Error Code: " + str(res))

    return cmd_msg_response


if __name__ == "__main__":
    cmd_msg = CmdMsg()
    cmd_msg.id = CmdCallbackId.CMD_RTC_SYNC
    cmd_msg.rtcSync.unixTime = c_uint32(0x12345678)
    cmd_msg_unpacked = CmdMsg()

    packed_msg = pack_command(cmd_msg)
    print([hex(element) for element in packed_msg])
    cmd_msg_unpacked = unpack_command(packed_msg)
    print(cmd_msg_unpacked.id)
    print(cmd_msg_unpacked.rtcSync.unixTime)

    cmd_response = CmdUnpackedReponse()
    cmd_response.errCode = 1
    cmd_response.cmdId = 1
    cmd_response.obcResetResponse = ObcCmdResetResponse(0.02, 2)
    buffer = (c_uint8 * 16)()

    packed_response = pack_command_response(cmd_response)
    print([hex(element) for element in packed_response])
    cmd_response_unpack = unpack_command_response(packed_response)
    print(cmd_response_unpack.cmdId)
    print(cmd_response_unpack.errCode)
    print(cmd_response_unpack.obcResetResponse.data1)
