%module obc_gs_pack_unpack
%{
    /* pack functions */
    #include "../telemetry/obc_gs_telemetry_pack.h"
    #include "../commands/obc_gs_command_pack.h"
    #include "../commands/obc_gs_commands_response_pack.h"

	/* unpack functions */
    #include "../telemetry/obc_gs_telemetry_unpack.h"
    #include "../commands/obc_gs_command_unpack.h"
    #include "../commands/obc_gs_commands_response_unpack.h"
%}

%include <stdint.i>
%include <stddef.i>

%typemap(in) const uint8_t * {
    if (!PyBytes_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expecting a bytes object");
    }
    if (PyBytes_Size($input) != AES_KEY_SIZE) {  // Ensure the key is the correct size
        SWIG_exception_fail(SWIG_ValueError, "Key size must be exactly AES_KEY_SIZE");
    }
    const char *buffer = PyBytes_AsString($input);  // Get the buffer as const char *
    if (!buffer) {
        SWIG_exception_fail(SWIG_ValueError, "Invalid bytes object");
    }
    $1 = (const uint8_t *)buffer;  // Cast to const uint8_t *
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

%typemap(in) cmd_msg_t * {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (cmd_msg_t *)malloc(sizeof(cmd_msg_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_MemoryError, "Cannot allocate memory for cmd_msg_t");
        }

        PyObject *timestamp_obj = PyDict_GetItemString($input, "timestamp");
        PyObject *isTimeTagged_obj = PyDict_GetItemString($input, "isTimeTagged");
        PyObject *id_obj = PyDict_GetItemString($input, "id");
        PyObject *rtcSync_obj = PyDict_GetItemString($input, "rtcSync");
        PyObject *downlinkLogsNextPass_obj = PyDict_GetItemString($input, "downlinkLogsNextPass");

        if (!timestamp_obj || !isTimeTagged_obj || !id_obj || (!rtcSync_obj && !downlinkLogsNextPass_obj)) {
            free($1);
            SWIG_exception_fail(SWIG_TypeError,
            "Expected a dictionary with keys 'timestamp', 'isTimeTagged', 'id' and either 'rtcSync' or 'downlinkLogsNextPass'");
        }

        if (PyLong_Check(timestamp_obj)) {
            $1->timestamp = (uint32_t)PyLong_AsUnsignedLong(timestamp_obj);
            if (PyErr_Occurred()) {
                free($1);
                SWIG_exception_fail(SWIG_TypeError, "'timestamp' value too large or invalid for uint32_t");
            }
        } else {
            free($1);
            SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'timestamp'");
        }

        $1->isTimeTagged = (bool)PyObject_IsTrue(isTimeTagged_obj);
        if (PyErr_Occurred()) {
            free($1);
            SWIG_exception_fail(SWIG_TypeError, "Failed to convert 'isTimeTagged' to boolean");
        }

        if (PyLong_Check(id_obj)) {
            long id_value = PyLong_AsLong(id_obj);
            if (id_value >= CMD_END_OF_FRAME && id_value < NUM_CMD_CALLBACKS) {
                $1->id = (cmd_callback_id_t)id_value;
            } else {
                free($1);
                SWIG_exception_fail(SWIG_ValueError, "Invalid cmd_callback_id_t value");
            }
        } else {
            free($1);
            SWIG_exception_fail(SWIG_ValueError, "Invalid type for 'id_obj'");
        }

        if (rtcSync_obj) {
            PyObject *unixTime_obj = PyDict_GetItemString(rtcSync_obj, "unixTime");
            if (!unixTime_obj) {
                free($1);
                SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'unixTime' inside 'rtcSync'");
            }

            if (PyLong_Check(unixTime_obj)) {
                $1->rtcSync.unixTime = (uint32_t)PyLong_AsUnsignedLong(unixTime_obj);
                if (PyErr_Occurred()) {
                    free($1);
                    SWIG_exception_fail(SWIG_TypeError, "unixTime value too large or invalid for uint32_t");
                }
            } else {
                free($1);
                SWIG_exception_fail(SWIG_TypeError, "Invalid type for unixTime");
            }
        }

        if (downlinkLogsNextPass_obj) {
            PyObject *logLevel_obj = PyDict_GetItemString(downlinkLogsNextPass_obj, "logLevel");
            if (!logLevel_obj) {
                free($1);
                SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'logLevel' inside 'downlinkLogsNextPass'");
            }

            if (PyLong_Check(logLevel_obj)) {
                $1->downlinkLogsNextPass.logLevel = (uint8_t)PyLong_AsUnsignedLong(logLevel_obj);
                if (PyErr_Occurred()) {
                    free($1);
                    SWIG_exception_fail(SWIG_TypeError, "logLevel value too large or invalid for uint8_t");
                }
            } else {
                free($1);
                SWIG_exception_fail(SWIG_TypeError, "Invalid type for logLevel");
            }
        }
    }
}

%typemap(freearg) cmd_msg_t * {
    if ($1) {
        free($1);
    }
}





// Create wrappers for the following files
%include "../telemetry/obc_gs_telemetry_pack.h"
%include "../commands/obc_gs_command_pack.h"
%include "../commands/obc_gs_commands_response_pack.h"
%include "../telemetry/obc_gs_telemetry_unpack.h"
%include "../commands/obc_gs_command_unpack.h"
%include "../commands/obc_gs_commands_response_unpack.h"