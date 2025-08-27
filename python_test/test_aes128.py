from hypothesis import given
from hypothesis.strategies import binary
from interfaces.obc_gs_interface.aes128 import AES128


@given(binary())
def test_encrypt_decrypt(data_bytes: bytes):
    aes_cipher = AES128(
        b"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f",
        b"\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",
    )
    encrypted_data = aes_cipher.encrypt(data_bytes)
    decrypted_data = aes_cipher.decrypt(encrypted_data)
    assert decrypted_data == data_bytes
