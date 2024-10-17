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

%typemap(in) u_frame_cmd_t * (u_frame_cmd_t temp) {
    if (PyLong_Check($input)) {
        long value = PyLong_AsLong($input);
        if (PyErr_Occurred()) {
            SWIG_fail;
        }

        temp = (u_frame_cmd_t) value;
        $1 = &temp;
    } else {
        SWIG_exception_fail(SWIG_TypeError, "Expected an integer value for u_frame_cmd_t");
    }
}

%typemap(in) ax25_addr_t * {
    if ($input == Py_None) {
        $1 = NULL;
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (ax25_addr_t *)malloc(sizeof(ax25_addr_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_TypeError, "Cannot allocate memory for ax25_addr_t");
        }

        PyObject *data_obj = PyDict_GetItemString($input, "data");
        if (!data_obj) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'data'");
        }

        if (PyObject_CheckBuffer(data_obj)) {
            Py_buffer view;
            if (PyObject_GetBuffer(data_obj, &view, PyBUF_SIMPLE) != 0) {
                SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes-like object");
            }
            if (view.len != AX25_DEST_ADDR_BYTES) {
                PyBuffer_Release(&view);
                SWIG_exception_fail(SWIG_ValueError, "'data' must have a length of AX25_DEST_ADDR_BYTES");
            }

            memcpy($1->data, view.buf, AX25_DEST_ADDR_BYTES);
            PyBuffer_Release(&view);
        } else {
            SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes array");
        }

        PyObject *length_obj = PyDict_GetItemString($input, "length");
        if (!length_obj || !PyLong_Check(length_obj)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'length' and an integer value");
        }

        $1->length = (uint8_t)PyLong_AsUnsignedLong(length_obj);
        if (PyErr_Occurred()) {
            SWIG_exception_fail(SWIG_OverflowError, "Error converting 'length' to uint8_t");
        }
    }
}

%typemap(freearg) ax25_addr_t * {
    if ($1) {
        free($1);
        $1 = NULL;
    }
}

// to get the Python object modified in place by the wrapped C function
%typemap(argout) ax25_addr_t * {
    if ($1) {
        PyObject *data_obj = PyBytes_FromStringAndSize((const char *)$1->data, AX25_DEST_ADDR_BYTES);
        PyDict_SetItemString($input, "data", data_obj);
        Py_DECREF(data_obj);

        PyObject *length_obj = PyLong_FromUnsignedLong($1->length);
        PyDict_SetItemString($input, "length", length_obj);
        Py_DECREF(length_obj);
    }
}

%typemap(in) unstuffed_ax25_i_frame_t * {
    if ($input == Py_None) {
        $1 = NULL;
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (unstuffed_ax25_i_frame_t *)malloc(sizeof(unstuffed_ax25_i_frame_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_MemoryError, "Cannot allocate memory for unstuffed_ax25_i_frame_t");
        }

        PyObject *data_obj = PyDict_GetItemString($input, "data");
        if (!data_obj) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'data'");
        }

        if (PyObject_CheckBuffer(data_obj)) {
            Py_buffer view;
            if (PyObject_GetBuffer(data_obj, &view, PyBUF_SIMPLE) != 0) {
                SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes-like object");
            }
            if (view.len != AX25_MINIMUM_I_FRAME_LEN) {
                PyBuffer_Release(&view);
                SWIG_exception_fail(SWIG_ValueError, "'data' must have a length of AX25_MINIMUM_I_FRAME_LEN");
            }

            memcpy($1->data, view.buf, AX25_MINIMUM_I_FRAME_LEN);
            PyBuffer_Release(&view);
        } else {
            SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes array");
        }

        PyObject *length_obj = PyDict_GetItemString($input, "length");
        if (!length_obj || !PyLong_Check(length_obj)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'length' and an integer value");
        }

        $1->length = (uint16_t)PyLong_AsUnsignedLong(length_obj);
        if (PyErr_Occurred()) {
            SWIG_exception_fail(SWIG_OverflowError, "Error converting 'length' to uint16_t");
        }
    }
}

%typemap(freearg) unstuffed_ax25_i_frame_t * {
    if ($1) {
        free($1);
        $1 = NULL;
    }
}

%typemap(in) packed_ax25_u_frame_t * {
    if ($input == Py_None) {
        $1 = NULL;
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (packed_ax25_u_frame_t *)malloc(sizeof(packed_ax25_u_frame_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_MemoryError, "Cannot allocate memory for packed_ax25_u_frame_t");
        }

        PyObject *data_obj = PyDict_GetItemString($input, "data");
        if (!data_obj) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'data'");
        }

        if (PyObject_CheckBuffer(data_obj)) {
            Py_buffer view;
            if (PyObject_GetBuffer(data_obj, &view, PyBUF_SIMPLE) != 0) {
                SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes-like object");
            }
            if (view.len != AX25_MAXIMUM_U_FRAME_CMD_LENGTH) {
                PyBuffer_Release(&view);
                SWIG_exception_fail(SWIG_ValueError, "'data' must have a length of AX25_MAXIMUM_U_FRAME_CMD_LENGTH");
            }

            memcpy($1->data, view.buf, AX25_MAXIMUM_U_FRAME_CMD_LENGTH);
            PyBuffer_Release(&view);
        } else {
            SWIG_exception_fail(SWIG_TypeError, "'data' must be a bytes array");
        }

        PyObject *length_obj = PyDict_GetItemString($input, "length");
        if (!length_obj || !PyLong_Check(length_obj)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'length' and an integer value");
        }

        $1->length = (uint8_t)PyLong_AsUnsignedLong(length_obj);
        if (PyErr_Occurred()) {
            SWIG_exception_fail(SWIG_OverflowError, "Error converting 'length' to uint8_t");
        }
    }
}

%typemap(freearg) packed_ax25_u_frame_t * {
    if ($1) {
        free($1);
        $1 = NULL;
    }
}

%include "../../ax25/obc_gs_ax25.h"
