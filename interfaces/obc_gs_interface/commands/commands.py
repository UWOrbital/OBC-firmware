from ctypes import CDLL, POINTER, Structure, Union, c_bool, c_uint, c_uint8, c_uint32, pointer
from pathlib import Path

# The shared object file we are using the access the c functions via ctypes
path = (Path(__file__).parent / "../../../build_gs/interfaces/libobc-gs-interface.so").resolve()
pack_unpack = CDLL(str(path))


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
    """

    _anonymous_ = ("u",)
    _fields_ = [("u", _U), ("timestamp", c_uint32), ("isTimeTagged", c_bool), ("id", c_uint)]


pack_unpack.unpackCmdMsg.argtypes = (POINTER(c_uint8 * 16), POINTER(c_uint32), POINTER(CmdMsg))
pack_unpack.unpackCmdMsg.restype = c_uint

pack_unpack.packCmdMsg.argtypes = (POINTER(c_uint8 * 16), POINTER(c_uint32), POINTER(CmdMsg), POINTER(c_uint8))
pack_unpack.packCmdMsg.restype = c_uint

MAX_CMD_MSG_SIZE = 16

if __name__ == "__main__":
    buff = (c_uint8 * MAX_CMD_MSG_SIZE)(*([0] * 16))
    unpack_offset = c_uint32(0)
    cmd_msg = CmdMsg()
    cmd_msg.id = c_uint(0x04)
    cmd_msg.rtcSync.unixTime = c_uint32(0x12345678)
    cmd_msg_unpacked = CmdMsg()
    pack_offset = c_uint32(0)
    num_packed = c_uint8(0)

    print(pack_unpack.packCmdMsg(pointer(buff), pointer(pack_offset), pointer(cmd_msg), pointer(num_packed)))
    print([hex(element) for element in buff])
    pack_unpack.unpackCmdMsg(pointer(buff), pointer(unpack_offset), pointer(cmd_msg_unpacked))
    print(cmd_msg_unpacked.id)
    print([element for element in buff])

    print("Hallo")
