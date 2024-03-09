import ctypes
import pathlib


def main() -> None:
    """Test for CRC function bindings"""
    lib_path = pathlib.Path("../../build_gs/interfaces/libobc-gs-interface.so")
    dll = ctypes.CDLL(str(lib_path.resolve()))
    dll.calculateCrc16Ccii.restype = ctypes.c_uint16
    dll.calculateCrc16Ccii.argstype = [ctypes.POINTER(ctypes.c_byte), ctypes.c_uint16]
    in_data1 = [1, 2, 3]
    in_data2 = 3
    out_data = dll.calculateCrc16Ccii(
        (ctypes.POINTER(ctypes.c_byte) * len(in_data1))(*in_data1), ctypes.c_uint16(in_data2)
    )
    print(out_data)


if __name__ == "__main__":
    main()
