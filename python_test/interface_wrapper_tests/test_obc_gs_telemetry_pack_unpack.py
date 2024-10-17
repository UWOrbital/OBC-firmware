import interfaces.obc_gs_interface.python_wrappers.error_codes.obc_gs_errors as errors
import interfaces.obc_gs_interface.python_wrappers.pack_unpack.obc_gs_pack_unpack as pack_unpack
import pytest


@pytest.fixture
def valid_telemetry_data():
    return {
        "id": pack_unpack.TELEM_OBC_TEMP,
        "timestamp": 1609459200,  # Example timestamp: 2021-01-01 00:00:00
        "data": {"obcTemp": 45.5},  # Example data for OBC temperature
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

    result = pack_unpack.packTelemetry(telemetry_data, buffer, len(buffer), num_packed)

    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "Packing telemetry data failed with valid data"


# Test packing telemetry data with invalid arguments
@pytest.mark.parametrize(
    "telemetry_data, buffer, buffer_len, expected_error",
    [
        (None, bytearray(256), 256, errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid telemetry_data,
        (
            {"id": pack_unpack.TELEM_NONE, "timestamp": 1609459200},
            bytearray(256),
            256,
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # ID is 0
        (
            {"id": pack_unpack.TELEM_CC1120_TEMP, "timestamp": 1609459200},
            bytearray(256),
            256,
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # No pack function
        (
            {"id": pack_unpack.TELEM_OBC_TEMP, "timestamp": 1609459200},
            None,
            256,
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid buffer
        (
            {"id": pack_unpack.TELEM_OBC_TEMP, "timestamp": 1609459200, "data": {"obcTemp": 45.5}},
            bytearray(256),
            0,
            errors.OBC_GS_ERR_CODE_BUFF_TOO_SMALL,
        ),  # Buffer too small
    ],
)
def test_pack_telemetry_invalid_args(telemetry_data, buffer, buffer_len, expected_error):
    num_packed = 0
    result = pack_unpack.packTelemetry(telemetry_data, buffer, buffer_len, num_packed)
    assert result == expected_error, "Incorrect error returned"


# Test unpacking telemetry data with valid buffer
def test_unpack_telemetry_valid_data(valid_telemetry_data, create_telemetry_buffer):
    telemetry_data = valid_telemetry_data
    buffer = create_telemetry_buffer()
    num_packed = 0

    pack_unpack.packTelemetry(telemetry_data, buffer, len(buffer), num_packed)

    immutable_buffer = bytes(buffer)
    offset = 0

    # reset telemetry data to be overwritten
    unpacked_telemetry_data = {"id": 0, "timestamp": 0}

    result = pack_unpack.unpackTelemetry(immutable_buffer, offset, unpacked_telemetry_data)

    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "Unpacking telemetry data failed with valid buffer"
    assert unpacked_telemetry_data["id"] == telemetry_data["id"]
    assert unpacked_telemetry_data["timestamp"] == telemetry_data["timestamp"]
    assert unpacked_telemetry_data["data"]["obcTemp"] == telemetry_data["data"]["obcTemp"]


# Test unpacking telemetry data with invalid arguments
@pytest.mark.parametrize(
    "buffer, offset, telemetry_data, expected_error",
    [
        (
            None,
            0,
            {"id": pack_unpack.TELEM_OBC_TEMP, "timestamp": 1609459200},
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid buffer
        (
            bytes(256),
            None,
            {"id": pack_unpack.TELEM_OBC_TEMP, "timestamp": 1609459200},
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid offset
        (bytes(256), 0, None, errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid telemetry_data
    ],
)
def test_unpack_telemetry_invalid_args(buffer, offset, telemetry_data, expected_error):
    result = pack_unpack.unpackTelemetry(buffer, offset, telemetry_data)
    assert result == expected_error, "Incorrect error returned for invalid arguments"
