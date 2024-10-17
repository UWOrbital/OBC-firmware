from distutils.core import Extension, setup

module = Extension(
    "_obc_gs_fec",
    sources=[
        "obc_gs_fec_wrap.c",
        "../../fec/obc_gs_fec.c",
        "../../../../gs/common/gs_heap.c",  # had to include a definition for sysMalloc
    ],
    include_dirs=[
        "../../fec",  # include headers
        "../../common",  # includes "obc_gs_errors.h"
        "../../../../libs/libcorrect/include",  # includes correct.h
        "../../aes128",  # AES headers
    ],
    extra_objects=[
        "../../libs/libcorrect/source/reed-solomon/decode.o",
        "../../libs/libcorrect/source/reed-solomon/encode.o",
        "../../libs/libcorrect/source/reed-solomon/polynomial.o",
        "../../libs/libcorrect/source/reed-solomon/reed-solomon.o",
    ],
    define_macros=[],
    undef_macros=[],
    library_dirs=[],
    libraries=[],
)

setup(name="obc_gs_fec", version="1.0", ext_modules=[module])
