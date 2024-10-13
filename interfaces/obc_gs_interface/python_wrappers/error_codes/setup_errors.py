import os
from distutils.core import setup, Extension

module = Extension(
    '_obc_gs_errors',
    sources=[
        'obc_gs_errors_wrap.c',
    ],
    include_dirs=[
        '../../common',                         # includes "obc_gs_errors.h"
    ],
    define_macros=[],
    undef_macros=[],
    library_dirs=[],
    libraries=[]
)

setup(
    name='obc_gs_errors',
    version='1.0',
    ext_modules=[module]
)
