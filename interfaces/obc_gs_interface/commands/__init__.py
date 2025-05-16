from ctypes import CDLL, POINTER, Structure, Union, c_bool, c_uint, c_uint8, c_uint32, pointer
from enum import IntEnum
from pathlib import Path

# The shared object file we are using the access the c functions via ctypes
path = (Path(__file__).parent / "../../../build_gs/interfaces/libobc-gs-interface.so").resolve()
pack_unpack = CDLL(str(path))


# Ctypes Declerations
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


# Class Implementation
class CommandPackUnpack:
    """
    A class that defines methods to pack and unpack commands. This class uses python wrappers for C functions to
    implement it's functionality
    """

    _MAX_CMD_MSG_SIZE = 16

    def __init__(self, pack_offset: int, unpack_offset: int, num_packed: int) -> None:
        """
        When initialized pass in the starting values you want for the offsets and number of commands packed

        :param pack_offset: The offset when packing (see C implementation)
        :param unpack_offset: The offset when unpacking (see C implementation)
        :param num_packed: The number of commands packed (see C implementation)
        """
        self._pack_offset = c_uint32(pack_offset)
        self._unpack_offset = c_uint32(unpack_offset)
        self._num_packed = c_uint8(num_packed)

    def pack(self, cmd_msg: CmdMsg) -> bytes:
        """
        This takes in a command message to be packed (see the C implementation for more on how that's exactly done)
        NOTE: When the class is initialized, it will use internal variables to keep a running count of the packOffset
        and numPacked parameters from the C implementation.

        :param cmd_msg: A c-style structure that hold the command message
        :return: Bytes of the packed message
        """
        buffer = (c_uint8 * self._MAX_CMD_MSG_SIZE)(*([0] * 16))
        res = pack_unpack.packCmdMsg(
            pointer(buffer), pointer(self._pack_offset), pointer(cmd_msg), pointer(self._num_packed)
        )

        if res != 0:
            raise ValueError("Could not unpack command. OBC Error Code: " + str(res))

        return bytes(buffer)

    def unpack(self, cmd_msg_packed: bytes) -> CmdMsg:
        """
        This takes in a command message to be packed (see the C implementation for more on how that's exactly done)
        NOTE: When the class is initialized, it will use internal variables to keep a running count of the unpackOffset
        parameter from the C implementation.

        :param cmd_msg_packed: Bytes of an already encoded message
        :return: An unpacked command message in the form of a structure
        """
        if len(cmd_msg_packed) > self._MAX_CMD_MSG_SIZE:
            raise ValueError("The encoded command data to unpack is too long")

        buffer_elements = list(cmd_msg_packed)
        buff = (c_uint8 * self._MAX_CMD_MSG_SIZE)(*buffer_elements)
        cmd_msg = CmdMsg()

        res = pack_unpack.unpackCmdMsg(pointer(buff), pointer(self._unpack_offset), pointer(cmd_msg))

        if res != 0:
            raise ValueError("Could not unpack command. OBC Error Code: " + str(res))

        return cmd_msg

    def set_pack_offset(self, num: c_uint32) -> None:
        """
        A method to set the pack offset of the class if needed

        :param num: The number to set the pack offset to
        :return: None
        """

        self._pack_offset = num

    def set_unpack_offset(self, num: c_uint32) -> None:
        """
        A method to set the unppack offset of the class if needed

        :param num: The number to set the unpack offset to
        :return: None
        """

        self._unpack_offset = num

    def set_num_packed(self, num: c_uint32) -> None:
        """
        A method to set the num_packed variable of the class if needed

        :param num: The number to set the num_packed variable to
        :return: None
        """

        self._num_packed = num

    class CmdMsgCode(IntEnum):
        """
        Enums corresponding to the C implementation
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


if __name__ == "__main__":
    packer = CommandPackUnpack(0, 0, 0)
    cmd_msg = CmdMsg()
    cmd_msg.id = packer.CmdMsgCode.CMD_RTC_SYNC
    cmd_msg.rtcSync.unixTime = c_uint32(0x12345678)
    cmd_msg_unpacked = CmdMsg()
    pack_offset = c_uint32(0)
    num_packed = c_uint8(0)

    packed_msg = packer.pack(cmd_msg)
    print([hex(element) for element in packed_msg])
    cmd_msg_unpacked = packer.unpack(packed_msg)
    print(cmd_msg_unpacked.id)
    print(cmd_msg_unpacked.rtcSync.unixTime)

    print("Hallo")
