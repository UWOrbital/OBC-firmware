%module obc_gs_ax25

%{
    #include "../../ax25/obc_gs_ax25.h"
%}

%include <stdint.i>

%typemap(in) const uint8_t * {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else if (!PyBytes_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expecting a bytes object");
    } else {
        const char *buffer = PyBytes_AsString($input);  // Get the buffer as const char *
        if (!buffer) {
            SWIG_exception_fail(SWIG_ValueError, "Invalid bytes object");
        }
        $1 = (const uint8_t *)buffer;  // Cast to const uint8_t *
    }
}

%typemap(in) uint8_t * {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else if (!PyByteArray_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expecting a bytearray object");
    } else {
        $1 = (uint8_t *)PyByteArray_AsString($input);
    }
}

%typemap(in) uint16_t * (uint16_t temp) {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else if (!PyLong_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expected a Python integer.");
    } else {
        unsigned long value = PyLong_AsUnsignedLong($input);
        if (PyErr_Occurred()) {
            SWIG_exception_fail(SWIG_TypeError, "Integer value too large or invalid for uint16_t.");
        }
        if (value > UINT16_MAX) {
            SWIG_exception_fail(SWIG_OverflowError, "Integer value too large for uint16_t.");
        }
        temp = (uint16_t) value;
        $1 = &temp;
    }
}

%typemap(in) uint32_t * (uint32_t temp) {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else if (!PyLong_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expected a Python integer.");
    } else {
        unsigned long value = PyLong_AsUnsignedLong($input);
        if (PyErr_Occurred()) {
            SWIG_exception_fail(SWIG_TypeError, "Integer value too large or invalid for uint32_t.");
        }
		if (value > UINT32_MAX) {
            SWIG_exception_fail(SWIG_OverflowError, "Integer value too large for uint32_t.");
        }
        temp = (uint32_t) value;
        $1 = &temp;
    }
}

// Return obc_gs_error_code_t by value
%typemap(out) obc_gs_error_code_t {
    $result = PyLong_FromLong((long)$1);
}
