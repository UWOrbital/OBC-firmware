import ctypes as c
import pathlib

lib_path = pathlib.Path("../../build_gs/interfaces/libobc-gs-interface.so")
dll = c.CDLL(str(lib_path.resolve()))
dll.calculateCrc16Ccitt.restype = c.c_uint16
dll.calculateCrc16Ccitt.argstype = [c.POINTER(c.c_byte), c.c_uint16]


def calculateCrc16Ccitt(arr: [bytes]) -> int:
    """Wrapper for obc gs calculateCrc16Ccitt function"""
    length = len(arr)
    in_data1 = (c.c_byte * length)(*[c.c_byte(x) for x in arr])
    return dll.calculateCrc16Ccitt(in_data1, length)


def main() -> None:
    """Test for CRC function bindings"""
    in_data1 = [1, 2, 3]
    out_data = calculateCrc16Ccitt(in_data1)
    print(out_data)


if __name__ == "__main__":
    main()
