%module obc_gs_pack_unpack
%{
    /* type definitions */
    #include "../../commands/obc_gs_command_data.h"
    #include "../../commands/obc_gs_command_id.h"
    #include "../../telemetry/obc_gs_telemetry_data.h"
    #include "../../telemetry/obc_gs_telemetry_id.h"

    /* pack functions */
    #include "../../telemetry/obc_gs_telemetry_pack.h"
    #include "../../commands/obc_gs_command_pack.h"
    #include "../../commands/obc_gs_commands_response_pack.h"

	/* unpack functions */
    #include "../../telemetry/obc_gs_telemetry_unpack.h"
    #include "../../commands/obc_gs_command_unpack.h"
    #include "../../commands/obc_gs_commands_response_unpack.h"
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
            // free($1);
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with keys 'timestamp', 'isTimeTagged', 'id' and either 'rtcSync' or 'downlinkLogsNextPass'");
        }

        // Check if the timestamp value is within range for uint32_t and not negative
        if (PyLong_Check(timestamp_obj)) {
            long long_val = PyLong_AsLong(timestamp_obj);
            if (long_val < 0 || (unsigned long)long_val > UINT32_MAX) {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "'timestamp' value out of range for uint32_t");
            }
            $1->timestamp = (uint32_t)long_val;
        } else {
            // free($1);
            SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'timestamp'");
        }

        $1->isTimeTagged = (bool)PyObject_IsTrue(isTimeTagged_obj);
        if (PyErr_Occurred()) {
            // free($1);
            SWIG_exception_fail(SWIG_TypeError, "Failed to convert 'isTimeTagged' to boolean");
        }

        // Validate id and check the range for cmd_callback_id_t
        if (PyLong_Check(id_obj)) {
            long id_value = PyLong_AsLong(id_obj);
            if (id_value >= CMD_END_OF_FRAME && id_value < NUM_CMD_CALLBACKS) {
                $1->id = (cmd_callback_id_t)id_value;
            } else {
                // free($1);
                SWIG_exception_fail(SWIG_ValueError, "Invalid cmd_callback_id_t value");
            }
        } else {
            // free($1);
            SWIG_exception_fail(SWIG_ValueError, "Invalid type for 'id'");
        }

        // Process rtcSync if provided
        if (rtcSync_obj) {
            PyObject *unixTime_obj = PyDict_GetItemString(rtcSync_obj, "unixTime");
            if (!unixTime_obj) {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'unixTime' inside 'rtcSync'");
            }

            // Check if unixTime is within range for uint32_t and not negative
            if (PyLong_Check(unixTime_obj)) {
                long long_val = PyLong_AsLong(unixTime_obj);
                if (long_val < 0 || (unsigned long)long_val > UINT32_MAX) {
                    // free($1);
                    SWIG_exception_fail(SWIG_TypeError, "'unixTime' value out of range for uint32_t");
                }
                $1->rtcSync.unixTime = (uint32_t)long_val;
            } else {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'unixTime'");
            }
        }

        // Process downlinkLogsNextPass if provided
        if (downlinkLogsNextPass_obj) {
            PyObject *logLevel_obj = PyDict_GetItemString(downlinkLogsNextPass_obj, "logLevel");
            if (!logLevel_obj) {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with key 'logLevel' inside 'downlinkLogsNextPass'");
            }

            // Check if logLevel is within range for uint8_t and not negative
            if (PyLong_Check(logLevel_obj)) {
                long long_val = PyLong_AsLong(logLevel_obj);
                if (long_val < 0 || (unsigned long)long_val > UINT8_MAX) {
                    // free($1);
                    SWIG_exception_fail(SWIG_TypeError, "'logLevel' value out of range for uint8_t");
                }
                $1->downlinkLogsNextPass.logLevel = (uint8_t)long_val;
            } else {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'logLevel'");
            }
        }
    }
}

%typemap(freearg) cmd_msg_t * {
    if ($1) {
        free($1);
    }
}

%typemap(in) cmd_unpacked_response_t * {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (cmd_unpacked_response_t *)malloc(sizeof(cmd_unpacked_response_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_MemoryError, "Cannot allocate memory for cmd_unpacked_response_t");
        }

        PyObject *errCode_obj = PyDict_GetItemString($input, "errCode");
        PyObject *cmdId_obj = PyDict_GetItemString($input, "cmdId");
        PyObject *obcResetResponse_obj = PyDict_GetItemString($input, "obcResetResponse");

        if (!errCode_obj || !cmdId_obj) {
            // free($1);
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with keys 'errCode' and 'cmdId'");
        }

        // Validate and set errCode
        if (PyLong_Check(errCode_obj)) {
            long errCode_value = PyLong_AsLong(errCode_obj);
            if (errCode_value >= CMD_RESPONSE_SUCCESS && errCode_value <= CMD_RESPONSE_ERROR) {
                $1->errCode = (cmd_response_error_code_t)errCode_value;
            } else {
                // free($1);
                SWIG_exception_fail(SWIG_ValueError, "Invalid cmd_response_error_code_t value");
            }
        } else {
            // free($1);
            SWIG_exception_fail(SWIG_ValueError, "Invalid type for 'errCode'");
        }

        // Validate and set cmdId
        if (PyLong_Check(cmdId_obj)) {
            long cmdId_value = PyLong_AsLong(cmdId_obj);
            if (cmdId_value >= CMD_END_OF_FRAME && cmdId_value < NUM_CMD_CALLBACKS) {
                $1->cmdId = (cmd_callback_id_t)cmdId_value;
            } else {
                // free($1);
                SWIG_exception_fail(SWIG_ValueError, "Invalid cmd_callback_id_t value");
            }
        } else {
            // free($1);
            SWIG_exception_fail(SWIG_ValueError, "Invalid type for 'cmdId'");
        }

        // Process obcResetResponse if provided
        if (obcResetResponse_obj) {
            PyObject *data1_obj = PyDict_GetItemString(obcResetResponse_obj, "data1");
            PyObject *data2_obj = PyDict_GetItemString(obcResetResponse_obj, "data2");

            if (!data1_obj || !data2_obj) {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with keys 'data1' and 'data2' inside 'obcResetResponse'");
            }

            // Set data1 (float)
            if (PyFloat_Check(data1_obj)) {
                $1->obcResetResponse.data1 = (float)PyFloat_AsDouble(data1_obj);
            } else {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'data1'");
            }

            // Set data2 (uint32_t)
            if (PyLong_Check(data2_obj)) {
                long data2_val = PyLong_AsLong(data2_obj);
                if (data2_val < 0 || (unsigned long)data2_val > UINT32_MAX) {
                    // free($1);
                    SWIG_exception_fail(SWIG_TypeError, "'data2' value out of range for uint32_t");
                }
                $1->obcResetResponse.data2 = (uint32_t)data2_val;
            } else {
                // free($1);
                SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'data2'");
            }
        }
    }
}

%typemap(freearg) cmd_unpacked_response_t * {
    if ($1) {
        free($1);
    }
}

%typemap(in) telemetry_data_t * {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (telemetry_data_t *)malloc(sizeof(telemetry_data_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_MemoryError, "Cannot allocate memory for telemetry_data_t");
        }

        PyObject *id_obj = PyDict_GetItemString($input, "id");
        PyObject *timestamp_obj = PyDict_GetItemString($input, "timestamp");
        PyObject *data_obj = PyDict_GetItemString($input, "data");

        if (!id_obj || !timestamp_obj) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with keys 'id' and 'timestamp'");
        }

        // Validate and set id (telemetry_data_id_t)
        if (PyLong_Check(id_obj)) {
            long id_value = PyLong_AsLong(id_obj);
            if (id_value >= TELEM_NONE && id_value <= TELEM_PONG) {
                $1->id = (telemetry_data_id_t)id_value;
            } else {
                SWIG_exception_fail(SWIG_ValueError, "Invalid telemetry_data_id_t value");
            }
        } else {
            SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'id'");
        }

        // Set timestamp (uint32_t)
        if (PyLong_Check(timestamp_obj)) {
            long timestamp_value = PyLong_AsLong(timestamp_obj);
            if (timestamp_value < 0 || (unsigned long)timestamp_value > UINT32_MAX) {
                SWIG_exception_fail(SWIG_TypeError, "'timestamp' value out of range for uint32_t");
            }
            $1->timestamp = (uint32_t)timestamp_value;
        } else {
            SWIG_exception_fail(SWIG_TypeError, "Invalid type for 'timestamp'");
        }

        // Convert telemetry id to camelCase key for accessing data
        char *camelCaseKey = NULL;
        switch ($1->id) {
            case TELEM_CC1120_TEMP: camelCaseKey = "cc1120Temp"; break;
            case TELEM_COMMS_CUSTOM_TRANSCEIVER_TEMP: camelCaseKey = "commsCustomTransceiverTemp"; break;
            case TELEM_OBC_TEMP: camelCaseKey = "obcTemp"; break;
            case TELEM_ADCS_MAG_BOARD_TEMP: camelCaseKey = "adcsMagBoardTemp"; break;
            case TELEM_ADCS_SENSOR_BOARD_TEMP: camelCaseKey = "adcsSensorBoardTemp"; break;
            case TELEM_EPS_BOARD_TEMP: camelCaseKey = "epsBoardTemp"; break;
            case TELEM_SOLAR_PANEL_1_TEMP: camelCaseKey = "solarPanel1Temp"; break;
            case TELEM_SOLAR_PANEL_2_TEMP: camelCaseKey = "solarPanel2Temp"; break;
            case TELEM_SOLAR_PANEL_3_TEMP: camelCaseKey = "solarPanel3Temp"; break;
            case TELEM_SOLAR_PANEL_4_TEMP: camelCaseKey = "solarPanel4Temp"; break;
            case TELEM_EPS_COMMS_5V_CURRENT: camelCaseKey = "epsComms5vCurrent"; break;
            case TELEM_EPS_COMMS_3V3_CURRENT: camelCaseKey = "epsComms3v3Current"; break;
            case TELEM_EPS_MAGNETORQUER_8V_CURRENT: camelCaseKey = "epsMagnetorquer8vCurrent"; break;
            case TELEM_EPS_ADCS_5V_CURRENT: camelCaseKey = "epsAdcs5vCurrent"; break;
            case TELEM_EPS_ADCS_3V3_CURRENT: camelCaseKey = "epsAdcs3v3Current"; break;
            case TELEM_EPS_OBC_3V3_CURRENT: camelCaseKey = "epsObc3v3Current"; break;
            case TELEM_EPS_COMMS_5V_VOLTAGE: camelCaseKey = "epsComms5vVoltage"; break;
            case TELEM_EPS_COMMS_3V3_VOLTAGE: camelCaseKey = "epsComms3v3Voltage"; break;
            case TELEM_EPS_MAGNETORQUER_8V_VOLTAGE: camelCaseKey = "epsMagnetorquer8vVoltage"; break;
            case TELEM_EPS_ADCS_5V_VOLTAGE: camelCaseKey = "epsAdcs5vVoltage"; break;
            case TELEM_EPS_ADCS_3V3_VOLTAGE: camelCaseKey = "epsAdcs3v3Voltage"; break;
            case TELEM_EPS_OBC_3V3_VOLTAGE: camelCaseKey = "epsObc3v3Voltage"; break;
            case TELEM_OBC_STATE: camelCaseKey = "obcState"; break;
            case TELEM_EPS_STATE: camelCaseKey = "epsState"; break;
            case TELEM_NUM_CSP_PACKETS_RCVD: camelCaseKey = "numCspPacketsRcvd"; break;
            default: camelCaseKey = NULL; break;
        }

        if (camelCaseKey && data_obj) {
            PyObject *value_obj = PyDict_GetItemString(data_obj, camelCaseKey);
            if (!value_obj) {
                SWIG_exception_fail(SWIG_TypeError, "No matching key for telemetry data in 'data'");
            }

            // Convert based on field type
            if (strcmp(camelCaseKey, "obcState") == 0 || strcmp(camelCaseKey, "epsState") == 0) {
                if (PyLong_Check(value_obj)) {
                    // union allows $1->epsState to be accessible even if you assign it to obcState since it
                    // shares the same memory location
                    uint8_t value = (uint8_t)PyLong_AsLong(value_obj);
                    memcpy((void*)(&$1->obcState), &value, sizeof(uint8_t)); // set appropriate uint8_t based on union position
                } else {
                    SWIG_exception_fail(SWIG_TypeError, "Expected an integer for 'obcState' or 'epsState'");
                }
            } else if (strcmp(camelCaseKey, "numCspPacketsRcvd") == 0) {
                if (PyLong_Check(value_obj)) {
                    $1->numCspPacketsRcvd = (uint32_t)PyLong_AsLong(value_obj);
                } else {
                    SWIG_exception_fail(SWIG_TypeError, "Expected an integer for 'numCspPacketsRcvd'");
                }
            } else {
                if (PyFloat_Check(value_obj)) {
                    float value = (float)PyFloat_AsDouble(value_obj);
                    memcpy((void*)(&$1->cc1120Temp), &value, sizeof(float)); // set appropriate float based on union position
                } else {
                    SWIG_exception_fail(SWIG_TypeError, "Expected a float value");
                }
            }
        }
    }
}

%typemap(freearg) telemetry_data_t * {
    if ($1) {
        free($1);
    }
}


// Create wrappers for the following files
// type definitions
%include "../../commands/obc_gs_command_data.h"
%include "../../commands/obc_gs_command_id.h"
%include "../../telemetry/obc_gs_telemetry_data.h"
%include "../../telemetry/obc_gs_telemetry_id.h"

// pack functions
%include "../../telemetry/obc_gs_telemetry_pack.h"
%include "../../commands/obc_gs_command_pack.h"
%include "../../commands/obc_gs_commands_response_pack.h"

// unpack functions
%include "../../telemetry/obc_gs_telemetry_unpack.h"
%include "../../commands/obc_gs_command_unpack.h"
%include "../../commands/obc_gs_commands_response_unpack.h"