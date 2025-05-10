import ctypes

# find the library. for osx this is .dylib, for linux .so, for windows .dll
# path should be an absolute path to where the library was compiled and installed
fec = ctypes.CDLL("../../../build_gs/interfaces/libobc-gs-interface.so")


# Let's define some of the structs from fec here as well...
class PackedRsPacket(ctypes.Structure):
    """ """

    _fields_ = [("data", ctypes.c_uint8 * 255)]


# Let's define the functions from our fec file that we want to use here...
# Arguments for rsEncode
fec.rsEncode.argtypes = [ctypes.POINTER(ctypes.c_uint8), ctypes.POINTER(PackedRsPacket)]
# Return for rsEncode
fec.rsEncode.restype = ctypes.c_uint


class Vars:
    """ """

    def __init__(self) -> None:
        self._struct_pointer = ctypes.pointer(PackedRsPacket((ctypes.c_uint8 * 255)()))
        self._int_pointer = ctypes.pointer(ctypes.c_uint8(2))

    def encode(self) -> None:
        """ """
        fec.rsEncode(self._int_pointer, self._struct_pointer)


if __name__ == "__main__":
    variables = Vars()
    print(variables.encode())
