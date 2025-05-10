import tinyaes as aes


class AES128:
    """
    A class with decryption and encryption methods for the AES standard
    """

    def __init__(self, key: bytes) -> None:
        self.key = key

    def encrypt(self, data: bytes, iv: bytes) -> bytes:
        """
        A function that encrypts based on the aes128 standard using CTR

        :param data: Bytes of data that need to be encrypted
        :param iv: The initialization vector passed as bytes
        :return: Encrypted data
        """
        aes_encode = aes.AES(self.key, iv)

        return aes_encode.CTR_xcrypt_buffer(data)

    def decrypt(self, data: bytes, iv: bytes) -> bytes:
        """
        A function that decrypts based on the aes128 standard using CTR

        :param data: Bytes of data that need to be encrypted
        :param iv: The initialization vector passed as bytes
        :return: Decrypted data
        """
        aes_encode = aes.AES(self.key, iv)

        return aes_encode.CTR_xcrypt_buffer(data)


# Example
if __name__ == "__main__":
    codec = AES128(bytes.fromhex("11223344556677889900AABBCCDDEEFF"))
    data = codec.encrypt(b"Hallo", bytes.fromhex("11223344556677889900AABBCCDDEEFF"))
    print(codec.decrypt(data, bytes.fromhex("11223344556677889900AABBCCDDEEFF")))
