from distutils.core import Extension, setup

module = Extension(
    "_obc_gs_pack_unpack",
    sources=[
        # SWIG-generated wrapper
        "obc_gs_pack_unpack_wrap.c",
        # Telemetry pack/unpack
        "../../telemetry/obc_gs_telemetry_pack.c",
        "../../telemetry/obc_gs_telemetry_unpack.c",
        # Command pack/unpack
        "../../commands/obc_gs_command_pack.c",
        "../../commands/obc_gs_command_unpack.c",
        # Command response pack/unpack
        "../../commands/obc_gs_commands_response_pack.c",
        "../../commands/obc_gs_commands_response_unpack.c",
        # Utils
        "../../../data_pack_unpack/data_pack_utils.c",
        "../../../data_pack_unpack/data_unpack_utils.c",
    ],
    include_dirs=[
        "../../common",  # includes "obc_gs_errors.h"
        "../../telemetry",  # includes telemetry headers
        "../../commands",  # includes command headers
        "../../../data_pack_unpack",  # includes data utils headers
    ],
    define_macros=[],
    undef_macros=[],
    library_dirs=[],
    libraries=[],
)

setup(name="obc_gs_pack_unpack", version="1.0", ext_modules=[module])
