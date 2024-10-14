%module obc_gs_aes128
%{
    #include "../../aes128/obc_gs_aes128.h"
%}

%include <stdint.i>

%typemap(in) const uint8_t * {
    if (!PyBytes_Check($input)) {
        SWIG_exception_fail(SWIG_TypeError, "Expecting a bytes object");
    }
    if (PyBytes_Size($input) != AES_KEY_SIZE) {  // Ensure the key is the correct size
        SWIG_exception_fail(SWIG_ValueError, "Key size must be exactly AES_KEY_SIZE");
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

// Return obc_gs_error_code_t by value
%typemap(out) obc_gs_error_code_t {
    $result = PyLong_FromLong((long)$1);
}

%typemap(in) aes_data_t * {
    if ($input == Py_None) {
        $1 = NULL;  // Pass a NULL pointer to the C function if input is None
    } else {
        if (!PyDict_Check($input)) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary");
        }

        $1 = (aes_data_t *)malloc(sizeof(aes_data_t));
        if (!$1) {
            SWIG_exception_fail(SWIG_MemoryError, "Failed to allocate memory for aes_data_t");
        }

        PyObject *iv_obj = PyDict_GetItemString($input, "iv");
        PyObject *ciphertext_obj = PyDict_GetItemString($input, "ciphertext");
        PyObject *ciphertextLen_obj = PyDict_GetItemString($input, "ciphertextLen");

        if (!iv_obj || !ciphertext_obj || !ciphertextLen_obj) {
            SWIG_exception_fail(SWIG_TypeError, "Expected a dictionary with keys 'iv', 'ciphertext', and 'ciphertextLen'");
        }

        // Fill the iv field (must be of length AES_IV_SIZE)
        if (!PyBytes_Check(iv_obj) || PyBytes_Size(iv_obj) != AES_IV_SIZE) {
            SWIG_exception_fail(SWIG_ValueError, "iv must be a bytes object of size AES_IV_SIZE");
        }
        memcpy($1->iv, PyBytes_AsString(iv_obj), AES_IV_SIZE);

        // Fill the ciphertext field
        if (PyBytes_Check(ciphertext_obj)) {
            $1->ciphertextLen = PyBytes_Size(ciphertext_obj);
            $1->ciphertext = (uint8_t *)malloc($1->ciphertextLen);
            if (!$1->ciphertext) {
                SWIG_exception_fail(SWIG_MemoryError, "Failed to allocate memory for ciphertext");
            }
            memcpy($1->ciphertext, PyBytes_AsString(ciphertext_obj), $1->ciphertextLen);
        } else {
            SWIG_exception_fail(SWIG_TypeError, "ciphertext must be a bytes object");
        }

        // Fill the ciphertextLen field
        $1->ciphertextLen = PyLong_AsSize_t(ciphertextLen_obj);
        if (PyErr_Occurred()) {
            SWIG_exception_fail(SWIG_ValueError, "Invalid value for ciphertextLen");
        }
    }
}

%typemap(freearg) aes_data_t * {
    if ($1) {
        if ($1->ciphertext) {
            free($1->ciphertext);
            $1->ciphertext = NULL;
        }
        free($1);
        $1 = NULL;
    }
}

%include "../../aes128/obc_gs_aes128.h"