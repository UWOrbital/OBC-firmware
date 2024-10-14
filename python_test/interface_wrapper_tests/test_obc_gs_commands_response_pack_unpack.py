import interfaces.obc_gs_interface.python_wrappers.error_codes.obc_gs_errors as errors
import interfaces.obc_gs_interface.python_wrappers.pack_unpack.obc_gs_pack_unpack as pack_unpack
import pytest


@pytest.fixture
def valid_response_data():
    return {
        "errCode": pack_unpack.CMD_RESPONSE_SUCCESS,
        "cmdId": pack_unpack.CMD_EXEC_OBC_RESET,
        "obcResetResponse": {"data1": 25.0, "data2": 12345},  # Example response data
    }


@pytest.fixture
def create_response_buffer():
    def _create_response_buffer(size=256):
        return bytearray(size)

    return _create_response_buffer


# Test packing with valid response data
def test_pack_command_response_valid_data(valid_response_data, create_response_buffer):
    response_data = valid_response_data
    buffer = create_response_buffer()

    result = pack_unpack.packCommandResponse(response_data, buffer)

    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "Packing command response failed with valid data"


# Parametrized test for invalid arguments in packing
@pytest.mark.parametrize(
    "response_data, buffer, expected_error",
    [
        (None, bytearray(256), errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid response data
        (
            {"errCode": pack_unpack.CMD_RESPONSE_SUCCESS, "cmdId": pack_unpack.CMD_EXEC_OBC_RESET},
            None,
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid buffer
    ],
)
def test_pack_command_response_invalid_args(response_data, buffer, expected_error):
    result = pack_unpack.packCommandResponse(response_data, buffer)
    assert result == expected_error, "Incorrect error returned for invalid pack arguments"


# Test unpacking with valid buffer
def test_unpack_command_response_valid_data(valid_response_data, create_response_buffer):
    response_data = valid_response_data
    buffer = create_response_buffer()

    # Pack the data first
    pack_unpack.packCommandResponse(response_data, buffer)

    # Create a new, empty response to unpack into
    unpacked_response = {"errCode": 0, "cmdId": 0, "obcResetResponse": {"data1": 0.0, "data2": 0}}

    # Unpack the data from the buffer
    result = pack_unpack.unpackCommandResponse(bytearray(buffer), unpacked_response)

    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "Unpacking command response failed with valid data"
    assert unpacked_response["cmdId"] == response_data["cmdId"]
    assert unpacked_response["errCode"] == response_data["errCode"]
    assert unpacked_response["obcResetResponse"]["data1"] == response_data["obcResetResponse"]["data1"]
    assert unpacked_response["obcResetResponse"]["data2"] == response_data["obcResetResponse"]["data2"]


# Parametrized test for invalid arguments in unpacking
@pytest.mark.parametrize(
    "buffer, response_data, expected_error",
    [
        (None, {"errCode": 0, "cmdId": 0}, errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid buffer
        (bytearray(256), None, errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid response data
    ],
)
def test_unpack_command_response_invalid_args(buffer, response_data, expected_error):
    result = pack_unpack.unpackCommandResponse(buffer, response_data)
    assert result == expected_error, "Incorrect error returned for invalid unpack arguments"
