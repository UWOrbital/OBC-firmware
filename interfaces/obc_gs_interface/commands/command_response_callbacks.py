from interfaces.obc_gs_interface.commands import CmdCallbackId, unpack_command_response
from interfaces.obc_gs_interface.commands.command_response_classes import CmdRes, CmdRtcSyncRes, CmdVerifyCrcRes


# Command specific parsing
def parse_cmd_with_no_data(cmd_response: CmdRes, data: bytes) -> CmdRes:
    """
    A function to parse the raw data from the response of CMD_EXEC_OBC_RESET

    :param cmd_response: Basic command response
    :param data: The raw bytes containing the data that needs to be parsed
    :return: CmdRes (i.e. A command response with no data for CMD_EXEC_OBC_RESET)
    """
    return cmd_response


def parse_cmd_rtc_sync(cmd_response: CmdRes, data: bytes) -> CmdRtcSyncRes:
    """
    A function to parse the raw data from the response of CMD_RTC_SYNC

    :param cmd_response: Basic command response
    :param data: The raw bytes containing the data that needs to be parsed
    :return: CmdRes (i.e. A command response with no data for CMD_RTC_SYNC)
    """
    # TODO: Implement this callback properly
    return CmdRtcSyncRes(cmd_response.cmd_id, cmd_response.error_code, 0x123478)


def parse_cmd_verify_crc(cmd_response: CmdRes, data: bytes) -> CmdVerifyCrcRes:
    """
    A function to parse the raw data from the response of CMD_VERIFY_CRC

    :param cmd_response: Basic command response
    :param data: The raw bytes containing the data that needs to be parsed
    :return: CmdVerifyCrcRes (i.e. The command response for CMD_VERIFY_CRC)
    """
    crc = int.from_bytes(data[:4], "little")
    return CmdVerifyCrcRes(cmd_response.cmd_id, cmd_response.error_code, crc)


# Function array where each index corresponds to the command enum value + 1
parse_func_list = [
    parse_cmd_with_no_data,
    parse_cmd_rtc_sync,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
    parse_cmd_verify_crc,
    parse_cmd_with_no_data,
    parse_cmd_with_no_data,
]


def parse_command_response(data: bytes) -> type[CmdRes] | CmdRes:
    """
    A function that unpacks a command response and returns a response structure

    :param data: The bytes of data the command is packed in
    """
    cmd_response_raw, data_bytes = unpack_command_response(data)
    cmd_response = CmdRes(CmdCallbackId(cmd_response_raw.cmdId), cmd_response_raw.errCode)
    cmd_parsed = parse_func_list[cmd_response.cmd_id.value - 1](cmd_response, data_bytes)

    return cmd_parsed


if __name__ == "__main__":
    data_bytes = b"\x0b\x01\x04\x78\x56\x34\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
    parse_command_response(data_bytes)
