from ctypes import POINTER, Structure, c_uint, c_uint8, pointer

from interfaces import RS_DECODED_DATA_SIZE, RS_ENCODED_DATA_SIZE
from interfaces.obc_gs_interface import interface


# Let's define the packed_rs_packet_t structure here so that we can use it as a parameter in functions
class PackedRsPacket(Structure):
    """
    The python equivalent class for the packed_rs_packet_t structure in the C implementation
    """

    _fields_ = [("data", c_uint8 * RS_ENCODED_DATA_SIZE)]


# Below are the ctype definitions from all the functions needed for fec
# initRs()
interface.initRs.argtypes = ()
interface.initRs.restype = None

# rsEncode()
interface.rsEncode.argtypes = [POINTER(c_uint8 * RS_DECODED_DATA_SIZE), POINTER(PackedRsPacket)]
interface.rsEncode.restype = c_uint

# rsDecode()
interface.rsDecode.argtypes = [POINTER(PackedRsPacket), POINTER(c_uint8 * RS_DECODED_DATA_SIZE), c_uint8]
interface.rsDecode.restype = c_uint

# destroyRs()
interface.destroyRs.argtypes = ()
interface.destroyRs.restype = None


class FEC:
    """
    Class for forward error correction using the reed solomon algorithm
    """

    _MAX_DECODED_DATA_LEN = RS_DECODED_DATA_SIZE
    _MAX_ENCODED_DATA_LEN = RS_ENCODED_DATA_SIZE

    def __init__(self) -> None:
        """
        Constructor
        """
        interface.initRs()

    def encode(self, data_to_encode: bytes) -> bytes:
        """
        A function that decodes data via reed solomon for forward error correction

        :param telem_data: Telemetry data of type c_uint8 to pass in (must be a 223 bytes in size to avoid issues)
        :return: 0 for success or a number representing the obc_gs error code
        """
        if len(data_to_encode) > self._MAX_DECODED_DATA_LEN:
            raise ValueError("Data to Encode is too long")

        uint_list = []
        for byte in data_to_encode:
            uint_list.append(c_uint8(byte))
        encode_data = pointer((c_uint8 * self._MAX_DECODED_DATA_LEN)(*uint_list))
        rs_data = pointer(PackedRsPacket((c_uint8 * self._MAX_ENCODED_DATA_LEN)()))
        result = interface.rsEncode(encode_data, rs_data)

        if result != 0:
            raise ValueError("Could not encode object. OBC GS Error Code: " + str(result))

        return bytes(rs_data.contents.data)

    def decode(self, data_to_decode: bytes) -> bytes:
        """
        A function that decodes data via reed solomon for forward error correction

        :param data_to_decode: An array of type c_uint8 with the 255 of encoded information (must be a 255 in size to
                               avoid issues)
        :return: 0 for success or a number representing the obc_gs error code
        """
        if len(data_to_decode) > self._MAX_ENCODED_DATA_LEN:
            raise ValueError("Data to Decode is too long")

        rs_info = data_to_decode[-32:]
        uint_list = []
        for byte in data_to_decode:
            uint_list.append(c_uint8(byte))
        rs_data = pointer(PackedRsPacket((c_uint8 * self._MAX_ENCODED_DATA_LEN)(*uint_list)))
        decoded_data = pointer((c_uint8 * self._MAX_DECODED_DATA_LEN)())
        result = interface.rsDecode(rs_data, decoded_data, c_uint8(self._MAX_DECODED_DATA_LEN))

        if result != 0:
            raise ValueError("Could not decode object. OBC GS Error Code: " + str(result))

        return bytes(decoded_data.contents) + rs_info

    def destroy(self) -> None:
        """
        Destructor
        """
        interface.destroyRs()


if __name__ == "__main__":
    fec_code = FEC()

    # Telem data from the C test case
    telem_data = (c_uint8 * RS_DECODED_DATA_SIZE)(
        64,
        121,
        190,
        31,
        108,
        53,
        202,
        59,
        88,
        177,
        150,
        23,
        4,
        237,
        34,
        179,
        112,
        233,
        110,
        15,
        156,
        165,
        122,
        43,
        136,
        33,
        70,
        7,
        52,
        93,
        210,
        163,
        160,
        89,
        30,
        255,
        204,
        21,
        42,
        27,
        184,
        145,
        246,
        247,
        100,
        205,
        130,
        147,
        208,
        201,
        206,
        239,
        252,
        133,
        218,
        11,
        232,
        1,
        166,
        231,
        148,
        61,
        50,
        131,
        0,
        57,
        126,
        223,
        44,
        245,
        138,
        251,
        24,
        113,
        86,
        215,
        196,
        173,
        226,
        115,
        48,
        169,
        46,
        207,
        92,
        101,
        58,
        235,
        72,
        225,
        6,
        199,
        244,
        29,
        146,
        99,
        96,
        25,
        222,
        191,
        140,
        213,
        234,
        219,
        120,
        81,
        182,
        183,
        36,
        141,
        66,
        83,
        144,
        137,
        142,
        175,
        188,
        69,
        154,
        203,
        168,
        193,
        102,
        167,
        84,
        253,
        242,
        67,
        192,
        249,
        62,
        159,
        236,
        181,
        74,
        187,
        216,
        49,
        22,
        151,
        132,
        109,
        162,
        51,
        240,
        105,
        238,
        143,
        28,
        37,
        250,
        171,
        8,
        161,
        198,
        135,
        180,
        221,
        82,
        35,
        32,
        217,
        158,
        127,
        76,
        149,
        170,
        155,
        56,
        17,
        118,
        119,
        228,
        77,
        2,
        19,
        80,
        73,
        78,
        111,
        124,
        5,
        90,
        139,
        104,
        129,
        38,
        103,
        20,
        189,
        178,
        3,
        128,
        185,
        254,
        95,
        172,
        117,
        10,
        123,
        152,
        241,
        214,
        87,
        68,
        45,
        98,
        243,
        176,
        41,
        174,
        79,
        220,
        229,
        186,
        107,
        200,
        97,
        134,
        71,
        116,
        157,
        18,
    )

    encode_result = bytearray(fec_code.encode(bytes(telem_data)))
    for i in range(len(encode_result)):
        print(encode_result[i], end=" ")
    print("--------------")

    # Flip some bits
    encode_result[0] += 1
    encode_result[222] += 1

    decode_result = fec_code.decode(bytes(encode_result))
    for i in range(len(decode_result)):
        print(decode_result[i], end=" ")
    print("--------------")

    # Check if the decoded data matches the original bits
    matches = True
    for i in range(len(telem_data)):
        if decode_result[i] != telem_data[i]:
            matches = False
            break

    print(matches)
