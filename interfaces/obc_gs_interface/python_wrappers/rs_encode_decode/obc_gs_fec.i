%module obc_gs_fec

%{
    #include "../../fec/obc_gs_fec.h"
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
        temp = (uint32_t) value;
        $1 = &temp;
    }
}

// Return obc_gs_error_code_t by value
%typemap(out) obc_gs_error_code_t {
    $result = PyLong_FromLong((long)$1);
}

%typemap(in) packed_rs_packet_t * {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (packed_rs_packet_t *)malloc(sizeof(packed_rs_packet_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_MemoryError, "Cannot allocate memory for packed_rs_packet_t");
        }

        // Retrieve the 'data' field from the Python dictionary
        PyObject *data_obj = PyDict_GetItemString($input, "data");
        if (!data_obj) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'data'");
        }

        // Check if 'data' is a bytes-like object with correct length
        if (PyObject_CheckBuffer(data_obj)) {
            Py_buffer view;
            if (PyObject_GetBuffer(data_obj, &view, PyBUF_SIMPLE) != 0) {
                SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes-like object");
            }
            if (view.len != RS_ENCODED_SIZE) {
                PyBuffer_Release(&view);
                SWIG_exception_fail(SWIG_ValueError, "'data' must have a length of RS_ENCODED_SIZE");
            }

            // Copy the data from the buffer into the C struct
            memcpy($1->data, view.buf, RS_ENCODED_SIZE);
            PyBuffer_Release(&view);
        } else {
            SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes array");
        }
    }
}

%typemap(freearg) packed_rs_packet_t * {
    if ($1) {
        free($1);
        $1 = NULL;
    }
}

%typemap(argout) packed_rs_packet_t * {
    if ($1) {
        // Ensure 'data' field exists in the Python input dictionary
        PyObject *data_obj = PyDict_GetItemString($input, "data");
        if (!data_obj) {
            // If 'data' field doesn't exist, create a new bytearray with the correct size
            data_obj = PyByteArray_FromStringAndSize(NULL, RS_ENCODED_SIZE);
            if (!data_obj) {
                SWIG_exception_fail(SWIG_MemoryError, "Unable to allocate memory for 'data' bytearray");
            }
            PyDict_SetItemString($input, "data", data_obj);
            Py_DECREF(data_obj);  // Decrease reference after adding to the dictionary
        } else if (!PyByteArray_Check(data_obj)) {
            SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytearray");
        } else if (PyByteArray_Size(data_obj) != RS_ENCODED_SIZE) {
            SWIG_exception_fail(SWIG_ValueError, "'data' bytearray must have RS_ENCODED_SIZE elements");
        }
        memcpy(PyByteArray_AsString(data_obj), $1->data, RS_ENCODED_SIZE);
    }
}

%include "../../fec/obc_gs_fec.h"