from distutils.core import Extension, setup

module = Extension(
    "_obc_gs_ax25",
    sources=["obc_gs_ax25_wrap.c", "../../ax25/obc_gs_ax25.c", "../../common/obc_gs_crc.c"],
    include_dirs=[
        "../../ax25",  # include headers
        "../../common",  # includes "obc_gs_errors.h" and "obc_gs_crc.h"
    ],
    define_macros=[],
    undef_macros=[],
    library_dirs=[],
    libraries=[],
)

setup(name="obc_gs_ax25", version="1.0", ext_modules=[module])
