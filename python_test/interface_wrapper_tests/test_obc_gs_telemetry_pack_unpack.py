import pytest
from interfaces.obc_gs_interface.python_wrappers.pack_unpack.obc_gs_pack_unpack import packTelemetry, unpackTelemetry, TELEM_NONE, TELEM_OBC_TEMP, TELEM_CC1120_TEMP
from interfaces.obc_gs_interface.python_wrappers.error_codes import obc_gs_errors

@pytest.fixture
def valid_telemetry_data():
    return {
        "id": TELEM_OBC_TEMP,  # Example for TELEM_OBC_TEMP
        "timestamp": 1609459200,  # Example timestamp: 2021-01-01 00:00:00
        "data": {"obcTemp": 45.5}  # Example data for OBC temperature
    }

@pytest.fixture
def create_telemetry_buffer():
    def _create_telemetry_buffer(size=256):
        return bytearray(size)
    return _create_telemetry_buffer

# Test packing telemetry data with valid data
def test_pack_telemetry_valid_data(valid_telemetry_data, create_telemetry_buffer):
    telemetry_data = valid_telemetry_data
    buffer = create_telemetry_buffer()
    num_packed = 0

    result = packTelemetry(telemetry_data, buffer, len(buffer), num_packed)

    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "Packing telemetry data failed with valid data"

# Test packing telemetry data with invalid arguments
@pytest.mark.parametrize("telemetry_data, buffer, buffer_len, expected_error", [
    (None, bytearray(256), 256, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid telemetry_data,
    ({"id": TELEM_NONE, "timestamp": 1609459200}, bytearray(256), 256, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG), # ID is 0
    ({"id": TELEM_CC1120_TEMP, "timestamp": 1609459200}, bytearray(256), 256, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG), # No pack function
    ({"id": TELEM_OBC_TEMP, "timestamp": 1609459200}, None, 256, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid buffer
    ({"id": TELEM_OBC_TEMP, "timestamp": 1609459200, "data": {"obcTemp": 45.5}}, bytearray(256), 0, obc_gs_errors.OBC_GS_ERR_CODE_BUFF_TOO_SMALL),  # Buffer too small
])
def test_pack_telemetry_invalid_args(telemetry_data, buffer, buffer_len, expected_error):
    num_packed = 0
    result = packTelemetry(telemetry_data, buffer, buffer_len, num_packed)
    assert result == expected_error, "Incorrect error returned"

# Test unpacking telemetry data with valid buffer
def test_unpack_telemetry_valid_data(valid_telemetry_data, create_telemetry_buffer):
    telemetry_data = valid_telemetry_data
    buffer = create_telemetry_buffer()
    num_packed = 0

    packTelemetry(telemetry_data, buffer, len(buffer), num_packed)

    immutable_buffer = bytes(buffer)
    offset = 0

    # reset telemetry data to be overwritten
    telemetry_data = {"id": 0, "timestamp": 0}

    result = unpackTelemetry(immutable_buffer, offset, telemetry_data)

    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "Unpacking telemetry data failed with valid buffer"
    assert telemetry_data["id"] == valid_telemetry_data["id"]
    assert telemetry_data["timestamp"] == valid_telemetry_data["timestamp"]
    assert telemetry_data["data"]["obcTemp"] == valid_telemetry_data["data"]["obcTemp"]

# Test unpacking telemetry data with invalid arguments
@pytest.mark.parametrize("buffer, offset, telemetry_data, expected_error", [
    (None, 0, {"id": TELEM_OBC_TEMP, "timestamp": 1609459200}, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid buffer
    (bytes(256), None, {"id": TELEM_OBC_TEMP, "timestamp": 1609459200}, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid offset
    (bytes(256), 0, None, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid telemetry_data
])
def test_unpack_telemetry_invalid_args(buffer, offset, telemetry_data, expected_error):
    result = unpackTelemetry(buffer, offset, telemetry_data)
    assert result == expected_error, "Incorrect error returned for invalid arguments"