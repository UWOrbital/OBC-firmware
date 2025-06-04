from ctypes import CDLL
from pathlib import Path

# The shared object file we are using the access the c functions via ctypes
path = (Path(__file__).parent / "../../build_gs/interfaces/libobc-gs-interface.so").resolve()
interface = CDLL(str(path))
