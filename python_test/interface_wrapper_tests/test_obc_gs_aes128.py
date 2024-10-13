import pytest
from interfaces.obc_gs_interface.python_wrappers.encrypt.obc_gs_aes128 import aes128Decrypt, initializeAesCtx, AES_KEY_SIZE, AES_IV_SIZE
from interfaces.obc_gs_interface.python_wrappers.error_codes import obc_gs_errors

@pytest.fixture
def valid_aes_key():
    return b'\x00' * AES_KEY_SIZE

@pytest.fixture
def aes_ctx(valid_aes_key):
    result = initializeAesCtx(valid_aes_key)
    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "Failed to initialize AES context"
    return valid_aes_key

@pytest.fixture
def create_aes_data():
    def _create_aes_data(iv=None, ciphertext=None):
        if iv is None:
            iv = b'\x00' * AES_IV_SIZE  # Default IV
        if ciphertext is None:
            ciphertext = b'\x01\x02\x03\x04\x05\x06\x07\x08'  # Default ciphertext
        return {
            "iv": iv,                         # Initialization vector (bytes)
            "ciphertext": ciphertext,         # Ciphertext (bytes)
            "ciphertextLen": len(ciphertext)  # Length of ciphertext
        }
    return _create_aes_data

# Test invalid AES context initialization
def test_initialize_aes_ctx_invalid_key():
    with pytest.raises(TypeError):
        initializeAesCtx(None)

# Test AES decryption with valid data
def test_aes128_decrypt_valid_data(aes_ctx, create_aes_data):
    aes_data = create_aes_data()
    output_buffer_len = aes_data["ciphertextLen"]
    output = bytearray(output_buffer_len)

    result = aes128Decrypt(aes_data, output, output_buffer_len)

    assert result == obc_gs_errors.OBC_GS_ERR_CODE_SUCCESS, "AES decryption failed with valid data"
    assert len(output) == output_buffer_len, "Output buffer length mismatch"

# Test AES decryption with invalid arguments
@pytest.mark.parametrize("aes_data, output, output_buffer_len, expected_error", [
    (None, bytearray(8), 8, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid aes_data
    ({"iv": b'\x00'*AES_IV_SIZE, "ciphertext": b'\x01\x02\x03\x04', "ciphertextLen": 4}, None, 4, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Invalid output
    ({"iv": b'\x00'*AES_IV_SIZE, "ciphertext": b'\x01\x02\x03\x04', "ciphertextLen": 4}, bytearray(4), 10, obc_gs_errors.OBC_GS_ERR_CODE_INVALID_ARG),  # Mismatched outputBufferLen
])
def test_aes128_decrypt_invalid_args(aes_data, output, output_buffer_len, expected_error):
    result = aes128Decrypt(aes_data, output, output_buffer_len)
    assert result == expected_error, "Incorrect error returned"