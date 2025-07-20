from dataclasses import dataclass
from interfaces.obc_gs_interface.commands import CmdCallbackId, CmdResponseErrorCode

@dataclass
class CmdRes:
    """
    A parent class for all response

    :param cmd_id: The id of the command that was sent
    :type cmd_id: CmdCallbackId
    :param error_code: Stores whether the command was successfully executed or if there was an error 
    :type error_code: CmdResponseErrorCode
    """
    cmd_id: CmdCallbackId
    error_code: CmdResponseErrorCode

@dataclass
class CmdRtcSyncRes(CmdRes):
    """
    Child class for storing the response to the CMD_RTC_SYNC

    :param board_unixtime: The time on the board when the sync command was sent
    :type board_unixtime: int
    """
    board_unixtime: int

@dataclass
class CmdVerifyCrcRes(CmdRes):
    """
    Class for storing the response to CMD_VERIFY_CRC

    :param crc: The crc of the application stored on the board
    :type crc: int 
    """
    crc: int 


