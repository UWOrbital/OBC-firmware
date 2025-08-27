from tinyaes import AES


class AES128:
    """
    A class with decryption and encryption methods for the AES standard
    """

    def __init__(self, key: bytes, iv: bytes) -> None:
        self._key = key
        self._iv = iv

    def encrypt(self, data: bytes) -> bytes:
        """
        A function that encrypts based on the aes128 standard using CTR

        :param data: Bytes of data that need to be encrypted
        :param iv: The initialization vector passed as bytes
        :return: Encrypted data
        """

        aes_encode = AES(self._key, self._iv)

        return bytes(aes_encode.CTR_xcrypt_buffer(data))

    def decrypt(self, data: bytes) -> bytes:
        """
        A function that decrypts based on the aes128 standard using CTR

        :param data: Bytes of data that need to be encrypted
        :param iv: The initialization vector passed as bytes
        :return: Decrypted data
        """
        aes_encode = AES(self._key, self._iv)

        return bytes(aes_encode.CTR_xcrypt_buffer(data))


# Example
if __name__ == "__main__":
    codec = AES128(bytes.fromhex("11223344556677889900AABBCCDDEEFF"), bytes.fromhex("11223344556677889900AABBCCDDEEFF"))
    data = codec.encrypt(b"UW Orbital")
    print(codec.decrypt(data))
