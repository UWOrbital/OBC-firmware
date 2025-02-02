import interfaces.obc_gs_interface.python_wrappers.error_codes.obc_gs_errors as obc_gs_errors
import interfaces.obc_gs_interface.python_wrappers.pack_unpack.obc_gs_pack_unpack as obc_gs_pack_unpack
import pytest


@pytest.fixture
def valid_command_data():
    return {
        "id": obc_gs_pack_unpack.CMD_RTC_SYNC,
        "timestamp": 1609459200,
        "isTimeTagged": False,
        "rtcSync": {"unixTime": 1609459200},  # Example rtcSync data
    }


@pytest.fixture
def create_command_buffer():
    def _create_command_buffer(size=256):
        return bytearray(size)

    return _create_command_buffer


def test_pack_cmd_valid_data(valid_command_data, create_command_buffer):
    cmd_data = valid_command_data
    buffer = create_command_buffer()
    offset = 0
    num_packed = bytearray(0)

    result = obc_gs_pack_unpack.packCmdMsg(buffer, offset, cmd_data, num_packed)

    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "Packing command data failed with valid data"


@pytest.mark.parametrize(
    "cmd_data, buffer, expected_error",
    [
        (None, bytearray(256), obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid cmd_data
        (
            {"id": obc_gs_pack_unpack.CMD_END_OF_FRAME, "timestamp": 1609459200, "isTimeTagged": True},
            bytearray(256),
            obc_gs_errors.OBC_GS_ERR_CODE_UNSUPPORTED_CMD,
        ),  # Unsupported command ID
        (
            {"id": obc_gs_pack_unpack.CMD_END_OF_FRAME, "timestamp": 1609459200, "isTimeTagged": True},
            None,
            obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid buffer
    ],
)
def test_pack_cmd_invalid_args(cmd_data, buffer, expected_error):
    offset = 0
    num_packed = bytearray(0)
    result = obc_gs_pack_unpack.packCmdMsg(buffer, offset, cmd_data, num_packed)
    assert result == expected_error, "Incorrect error returned"


def test_unpack_cmd_valid_data(valid_command_data, create_command_buffer):
    cmd_data = valid_command_data
    buffer = create_command_buffer()

    offset = 0
    num_packed = bytearray(0)

    obc_gs_pack_unpack.packCmdMsg(buffer, offset, cmd_data, num_packed)
    immutable_buffer = bytes(buffer)

    # reset cmd_data to be overwritten
    unpacked_cmd_data = {"id": obc_gs_pack_unpack.CMD_END_OF_FRAME, "timestamp": 0, "isTimeTagged": False}

    result = obc_gs_pack_unpack.unpackCmdMsg(immutable_buffer, offset, unpacked_cmd_data)

    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "Unpacking command data failed with valid buffer"
    assert unpacked_cmd_data["id"] == cmd_data["id"]
    assert unpacked_cmd_data["timestamp"] == cmd_data["timestamp"]
    assert unpacked_cmd_data["isTimeTagged"] == cmd_data["isTimeTagged"]
    assert unpacked_cmd_data["rtcSync"]["unixTime"] == cmd_data["rtcSync"]["unixTime"]


@pytest.mark.parametrize(
    "buffer, offset, cmd_data, expected_error",
    [
        (
            None,
            0,
            {"id": 2, "timestamp": 1609459200, "isTimeTagged": True},
            obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid buffer
        (
            bytes(256),
            None,
            {"id": 2, "timestamp": 1609459200, "isTimeTagged": True},
            obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid offset
        (bytes(256), 0, None, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid cmd_data
    ],
)
def test_unpack_cmd_invalid_args(buffer, offset, cmd_data, expected_error):
    result = obc_gs_pack_unpack.unpackCmdMsg(buffer, offset, cmd_data)
    assert result == expected_error, "Incorrect error returned"
