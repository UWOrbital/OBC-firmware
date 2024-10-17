import interfaces.obc_gs_interface.python_wrappers.error_codes.obc_gs_errors as errors
import interfaces.obc_gs_interface.python_wrappers.rs_encode_decode.obc_gs_fec as fec
import pytest


@pytest.fixture
def telem_data():
    """Returns a valid packed telemetry data bytearray."""
    return bytearray([i % 256 for i in range(fec.RS_DECODED_SIZE)])


@pytest.fixture
def rs_packet():
    """Returns a valid packed RS packet as a dictionary."""
    return {"data": bytearray(fec.RS_ENCODED_SIZE)}


def test_init_destroy_rs():
    """Test if RS initialization works correctly."""
    fec.initRs()
    fec.destroyRs()
    # Assuming initRs has no return value or errors, just ensure it runs without issues
    assert True  # If no errors, the test is successful


@pytest.fixture(scope="module")
def rs_context():
    fec.initRs()
    yield
    fec.destroyRs()


def test_rs_encode(rs_context, telem_data, rs_packet):
    """Test encoding telemetry data into Reed-Solomon encoded data."""

    result = fec.rsEncode(telem_data, rs_packet)

    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "RS encoding failed with valid telemetry data"
    assert isinstance(rs_packet["data"], bytearray), "Encoded RS packet should be a bytearray"
    assert len(rs_packet["data"]) == fec.RS_ENCODED_SIZE, "Encoded RS packet should have 255 bytes"


@pytest.mark.parametrize(
    "telem_data, rs_packet, expected_error",
    [
        (None, {"data": bytearray(fec.RS_ENCODED_SIZE)}, errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid telemetry data
        (
            bytearray([i % 256 for i in range(fec.RS_DECODED_SIZE)]),
            None,
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid RS packet
    ],
)
def test_rs_encode_invalid_args(rs_context, telem_data, rs_packet, expected_error):
    """Test RS encoding with invalid arguments."""
    result = fec.rsEncode(telem_data, rs_packet)
    assert result == expected_error, "RS encoding did not return the expected error"


def test_rs_decode(rs_packet, telem_data):
    """Test decoding Reed-Solomon encoded data back into telemetry data."""
    result = fec.rsDecode(rs_packet, telem_data, fec.RS_DECODED_SIZE)
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "RS decoding failed with valid RS packet"
    assert isinstance(telem_data, bytearray), "Decoded telemetry data should be a bytearray"
    assert len(telem_data) == fec.RS_DECODED_SIZE, "Decoded telemetry data should have 223 bytes"


@pytest.mark.parametrize(
    "rs_packet, decoded_data_len, expected_error",
    [
        (None, fec.RS_DECODED_SIZE, errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid RS packet
        (
            {"data": bytearray(fec.RS_ENCODED_SIZE)},
            0,
            errors.OBC_GS_ERR_CODE_INVALID_ARG,
        ),  # Invalid decoded data length
    ],
)
def test_rs_decode_invalid_args(rs_packet, decoded_data_len, expected_error):
    """Test RS decoding with invalid arguments."""
    decoded_data = bytearray(fec.RS_DECODED_SIZE)
    result = fec.rsDecode(rs_packet, decoded_data, decoded_data_len)
    assert result == expected_error, "RS decoding did not return the expected error"


def test_encode_decode_zero_data(rs_context):
    encoded_data = {"data": bytearray(fec.RS_ENCODED_SIZE)}
    data = bytearray([0xFF] * fec.RS_DECODED_SIZE)

    result = fec.rsEncode(data, encoded_data)
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "RS encoding failed"

    # Flip some bits
    encoded_data["data"][0] ^= 0b10000001
    encoded_data["data"][222] ^= 0b10100011

    decoded_data = bytearray(fec.RS_DECODED_SIZE)
    result = fec.rsDecode(encoded_data, decoded_data, fec.RS_DECODED_SIZE)
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "RS decoding failed"

    assert decoded_data == data, "Decoded data does not match original data"


def test_encode_decode_non_zero(rs_context):
    encoded_data = {"data": bytearray(fec.RS_ENCODED_SIZE)}
    data = bytearray(fec.RS_DECODED_SIZE)

    # Generate pseudo-random data
    seed = 0x7A21C3
    for i in range(fec.RS_DECODED_SIZE):
        seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF
        data[i] = seed & 0xFF

    result = fec.rsEncode(data, encoded_data)
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "RS encoding failed"

    # Flip some bits
    encoded_data["data"][0] ^= 0b10000001
    encoded_data["data"][222] ^= 0b10100011

    decoded_data = bytearray(fec.RS_DECODED_SIZE)
    result = fec.rsDecode(encoded_data, decoded_data, fec.RS_DECODED_SIZE)
    assert result == errors.OBC_GS_ERR_CODE_SUCCESS, "RS decoding failed"

    assert decoded_data == data, "Decoded data does not match original data"
