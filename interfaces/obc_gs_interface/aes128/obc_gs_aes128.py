import tinyaes as aes


class AES128:
    """
    A class with decryption and encryption methods for the AES standard
    """

    def __init__(self, key: bytes) -> None:
        self.key = key

    def apply_aes128(self, data: bytes, iv: bytes) -> bytes:
        """
        A function that applys the aes128 standard using CTR

        :param data: Bytes of data that need to be encrypted
        :param iv: The initialization vector passed as bytes
        :return: If data was encrypted, then this function returns the decrypted data. If the data was decrypted, then
                 this function returns the data encrypted
        """
        aes_encode = aes.AES(self.key, iv)

        return aes_encode.CTR_xcrypt_buffer(data)


# Example
if __name__ == "__main__":
    codec = AES128(bytes.fromhex("11223344556677889900AABBCCDDEEFF"))
    data = codec.apply_aes128(b"Hallo", bytes.fromhex("11223344556677889900AABBCCDDEEFF"))
    print(codec.apply_aes128(data, bytes.fromhex("11223344556677889900AABBCCDDEEFF")))
