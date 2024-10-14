import os
from distutils.core import setup, Extension

module = Extension(
    '_obc_gs_fec',
    sources=[
        'obc_gs_fec_wrap.c',
        '../../fec/obc_gs_fec.c'
    ],
    include_dirs=[
        '../../fec',                            # include headers
        '../../common',                         # includes "obc_gs_errors.h"
        '../../../../libs/libcorrect/include',  # includes correct.h
        '../../aes128',                         # AES headers
    ],
    define_macros=[],
    undef_macros=[],
    library_dirs=['../../../../libs/libcorrect/build'],
    libraries=['lib-correct']
)

setup(
    name='obc_gs_fec',
    version='1.0',
    ext_modules=[module]
)
