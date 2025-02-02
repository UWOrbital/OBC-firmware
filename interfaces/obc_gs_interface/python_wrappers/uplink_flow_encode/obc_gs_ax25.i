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

%typemap(argout) uint8_t * {
    if ($1) {
        // Convert C uint8_t* back to Python bytearray
        PyObject *result = PyByteArray_FromStringAndSize((char *)$1, sizeof($1));
        if (!result) {
            SWIG_exception_fail(SWIG_MemoryError, "Failed to create Python bytearray from C uint8_t*.");
        }
        $input = result;
    } else {
        $input = Py_None;
        Py_INCREF(Py_None);
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

%typemap(argout) uint16_t * {
    if ($1) {
        // Convert C uint16_t* back to Python integer
        PyObject *result = PyLong_FromUnsignedLong((unsigned long)(*$1));
        if (!result) {
            SWIG_exception_fail(SWIG_MemoryError, "Failed to create Python integer from C uint16_t*.");
        }
        $input = result;
    } else {
        $input = Py_None;
        Py_INCREF(Py_None);
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

// Create Python objects from the structs

%extend unstuffed_ax25_i_frame_t {
    void set_data(PyObject *bytes_obj) {
        if (!PyByteArray_Check(bytes_obj)) {
            PyErr_SetString(PyExc_TypeError, "Expected bytearray");
            return;
        }
        if (PyByteArray_Size(bytes_obj) != AX25_MINIMUM_I_FRAME_LEN) {
            PyErr_SetString(PyExc_ValueError, "Bytearray must be exactly AX25_MINIMUM_I_FRAME_LEN bytes");
            return;
        }
        memcpy(self->data, PyByteArray_AsString(bytes_obj), AX25_MINIMUM_I_FRAME_LEN);
    }

    PyObject* get_data() {
        return PyByteArray_FromStringAndSize((char *)self->data, AX25_MINIMUM_I_FRAME_LEN);
    }
}

%extend packed_ax25_i_frame_t {
    void set_data(PyObject *bytes_obj) {
        if (!PyByteArray_Check(bytes_obj)) {
            PyErr_SetString(PyExc_TypeError, "Expected bytearray");
            return;
        }
        if (PyByteArray_Size(bytes_obj) != AX25_MAXIMUM_PKT_LEN) {
            PyErr_SetString(PyExc_ValueError, "Bytearray must be exactly AX25_MAXIMUM_PKT_LEN bytes");
            return;
        }
        memcpy(self->data, PyByteArray_AsString(bytes_obj), AX25_MAXIMUM_PKT_LEN);
    }

    PyObject* get_data() {
        return PyByteArray_FromStringAndSize((char *)self->data, AX25_MAXIMUM_PKT_LEN);
    }
}

%extend packed_ax25_u_frame_t {
    void set_data(PyObject *bytes_obj) {
        if (!PyByteArray_Check(bytes_obj)) {
            PyErr_SetString(PyExc_TypeError, "Expected bytearray");
            return;
        }
        if (PyByteArray_Size(bytes_obj) != AX25_MAXIMUM_U_FRAME_CMD_LENGTH) {
            PyErr_SetString(PyExc_ValueError, "Bytearray must be exactly AX25_MAXIMUM_U_FRAME_CMD_LENGTH bytes");
            return;
        }
        memcpy(self->data, PyByteArray_AsString(bytes_obj), AX25_MAXIMUM_U_FRAME_CMD_LENGTH);
    }

    PyObject* get_data() {
        return PyByteArray_FromStringAndSize((char *)self->data, AX25_MAXIMUM_U_FRAME_CMD_LENGTH);
    }
}

%extend ax25_addr_t {
    void set_data(PyObject *bytes_obj) {
        if (!PyByteArray_Check(bytes_obj)) {
            PyErr_SetString(PyExc_TypeError, "Expected bytearray");
            return;
        }
        if (PyByteArray_Size(bytes_obj) != AX25_DEST_ADDR_BYTES) {
            PyErr_SetString(PyExc_ValueError, "Bytearray must be exactly AX25_DEST_ADDR_BYTES bytes");
            return;
        }
        memcpy(self->data, PyByteArray_AsString(bytes_obj), AX25_DEST_ADDR_BYTES);
    }

    PyObject* get_data() {
        return PyByteArray_FromStringAndSize((char *)self->data, AX25_DEST_ADDR_BYTES);
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

%typemap(argout) u_frame_cmd_t * {
    if ($1) {
        $input = PyLong_FromLong((long)(*$1));
    } else {
        $input = Py_None;
        Py_INCREF(Py_None);
    }
}

%include "../../ax25/obc_gs_ax25.h"
