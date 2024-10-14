import pytest
import interfaces.obc_gs_interface.python_wrappers.rs_encode_decode.obc_gs_fec as obc_gs_fec
import interfaces.obc_gs_interface.python_wrappers.error_codes.obc_gs_errors as obc_gs_errors

@pytest.fixture
def telem_data():
    """Returns a valid packed telemetry data bytearray."""
    return bytearray([i % 256 for i in range(223)])

@pytest.fixture
def rs_packet():
    """Returns a valid packed RS packet as a dictionary."""
    return {"data": bytearray(255)}

def test_init_rs():
    """Test if RS initialization works correctly."""
    obc_gs_fec.initRs()
    # Assuming initRs has no return value or errors, just ensure it runs without issues
    assert True  # If no errors, the test is successful

def test_destroy_rs():
    """Test if RS destruction works correctly."""
    obc_gs_fec.destroyRs()
    # Assuming destroyRs has no return value or errors, just ensure it runs without issues
    assert True  # If no errors, the test is successful

def test_rs_encode(telem_data, rs_packet):
    """Test encoding telemetry data into Reed-Solomon encoded data."""
    result = obc_gs_fec.rsEncode(telem_data, rs_packet)
    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "RS encoding failed with valid telemetry data"
    assert isinstance(rs_packet["data"], bytearray), "Encoded RS packet should be a bytearray"
    assert len(rs_packet["data"]) == 255, "Encoded RS packet should have 255 bytes"

@pytest.mark.parametrize("telem_data, rs_packet, expected_error", [
    (None, {"data": bytearray(255)}, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid telemetry data
    (bytearray([i % 256 for i in range(223)]), None, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid RS packet
])
def test_rs_encode_invalid_args(telem_data, rs_packet, expected_error):
    """Test RS encoding with invalid arguments."""
    result = obc_gs_fec.rsEncode(telem_data, rs_packet)
    assert result == expected_error, "RS encoding did not return the expected error"

def test_rs_decode(rs_packet, telem_data):
    """Test decoding Reed-Solomon encoded data back into telemetry data."""
    result = obc_gs_fec.rsDecode(rs_packet, telem_data, 223)
    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "RS decoding failed with valid RS packet"
    assert isinstance(telem_data, bytearray), "Decoded telemetry data should be a bytearray"
    assert len(telem_data) == 223, "Decoded telemetry data should have 223 bytes"

@pytest.mark.parametrize("rs_packet, decoded_data_len, expected_error", [
    (None, 223, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid RS packet
    ({"data": bytearray(255)}, 0, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid decoded data length
])
def test_rs_decode_invalid_args(rs_packet, decoded_data_len, expected_error):
    """Test RS decoding with invalid arguments."""
    decoded_data = bytearray(223)
    result = obc_gs_fec.rsDecode(rs_packet, decoded_data, decoded_data_len)
    assert result == expected_error, "RS decoding did not return the expected error"
