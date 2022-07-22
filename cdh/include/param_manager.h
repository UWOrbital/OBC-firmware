#ifndef CDH_INCLUDE_PARAM_MANAGER_H_
#define CDH_INCLUDE_PARAM_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @enum	param_names_t
 * @brief	Parameter Names
 *
 * All parameter names
*/
typedef enum
{
    ALTITUDE,
    TEST_PARAM1,
    TEST_PARAM2,
    TEST_PARAM3,
    TEST_PARAM4,
    TEST_PARAM5,
    TEST_PARAM6,
    TEST_PARAM7,
    TEST_PARAM8,
    TEST_PARAM9,
    TEST_PARAM10,
    // The rest of the parameters should be added here
} param_names_t;

/**
 * @enum	param_type_t
 * @brief	Parameter Types
 *
 * Supported types for parameters
*/
typedef enum
{
    UINT8_PARAM,
    INT8_PARAM,
    UINT16_PARAM,
    INT16_PARAM,
    UINT32_PARAM,
    INT32_PARAM,
    UINT64_PARAM,
    INT64_PARAM,
    FLOAT_PARAM,
    DOUBLE_PARAM,
    STRING_PARAM
} param_type_t;

/**
 * @enum	param_size_t
 * @brief	Parameter Sizes
 *
 * Macros for assigning the size in Parameters list, note that STRING_PARAM
 * can have any size and must be decided by user.
*/
typedef enum
{
    UINT8_SIZE		= sizeof(uint8_t),
    INT8_SIZE		= sizeof(int8_t),
    UINT16_SIZE		= sizeof(uint16_t),
    INT16_SIZE		= sizeof(int16_t),
    UINT32_SIZE		= sizeof(uint32_t),
    INT32_SIZE      = sizeof(int32_t),
    UINT64_SIZE     = sizeof(uint64_t),
    INT64_SIZE		= sizeof(int64_t),
    FLOAT_SIZE		= sizeof(float),
    DOUBLE_SIZE		= sizeof(double)
    /* String Size should be decided by user. */
} param_size_t;

/**
 * @enum	param_opts_t
 * @brief	Parameter Options
 * @note	READ_ONLY is only applicable for parameterized variables.
 *
 * Macros for assigning the options in Parameters list
*/
typedef enum
{
    TELEMETRY	= 0b001,		/** Collect this param for telemetry. */
    PERSISTENT	= 0b010,		/** Persist the value on non volatile memory. */
    READ_ONLY	= 0b100 		/** Prohibited to write with set_param functions. */
} param_opts_t;

/**
 * @union	param_val_t
 * @brief	Parameter value types
 *
 * Macros for allowing various value types in parameters
*/
typedef union {
    uint8_t     u8;
    int8_t      i8;
    uint16_t    u16;
    int16_t     i16;
    uint32_t    u32;
    int32_t     i32;
    uint64_t    u64;
    int64_t     i64;
    float       f;
    double      d;
    char*       s;
} param_val_t;

/**
 * @struct	param_t
 * @brief	Parameter struct
 *
 * Holds the data for a parameter in the list. The Parameter list
 * is composed of a collection of this struct.
 *
 * Do not handle this struct directly, use instead param_handle_t type
 * and the API defined functions.
 * @see param_handle_t
*/
typedef struct
{
    const param_names_t name;
    const param_type_t type;				/**< Param Type, from param_type_t. */
    const param_size_t size;				/**< Size in bytes, from param_size_t, or custom if STRING_PARAM.*/
    uint8_t opts;							/**< Special options, from param_opts_t.*/
    param_val_t value;						/**< Pointer to param space where value is stored. */
} param_t;

/**
 * @typedef	param_handle_t
 * @brief	Typedef for parameter handle
*/
typedef param_t * param_handle_t;

/**
 * @enum	access_type_t
 * @brief	Access Options
 *
 * Macros for specifying the intent for accessing the parameter table.
*/
typedef enum
{
    GET_PARAM,
    SET_PARAM
} access_type_t;

/**
 * @brief	Initialize the parameter manager
 *
 * @return	1 if OK; 0 if already initialized
 */
uint8_t initParamManager(void);

/**
 * @brief	Get the parameter handle for the given parameter name
 * @param	paramName          Name of the parameter
 * @param	param              Pointer to the parameter
 *
 * @return	1 if OK; 0 if error or parameter doesn't exist
 */
uint8_t getParam(param_names_t paramName, param_handle_t param);

/**
 * @brief	Get the parameter handle for the given parameter index
 * @param	paramIndex         Index of the parameter
 * @param	param              Pointer to the parameter
 *
 * @return	1 if OK; 0 if error or parameter doesn't exist
 */
uint8_t getParamByIndex(uint16_t paramIndex, param_handle_t param);

/**
 * @brief	Get the value of a Parameter
 * @param	paramName          Name of the parameter
 * @param	paramType          Type of the parameter. Used to make sure developer is using the right type.
 * @param	out               Void pointer to buffer to store param value, should be enough to store value
 *
 * @return	1 if OK; 0 if error or parameter doesn't exist
 */
uint8_t getParamVal(param_names_t paramName, param_type_t paramType, void *out);

/**
 * @brief	Get the value of a parameter by index
 * @param	paramIndex         Index of the parameter
 * @param	paramType          Type of the parameter. Used to make sure developer is using the right type.
 * @param	out                Void pointer to buffer to store param value, should be enough to store value
 *
 * @return	1 if OK; 0 if error or parameter doesn't exist
 */
uint8_t getParamValByIndex(uint16_t paramIndex, param_type_t paramType, void *out);

/**
 * @brief	Set the value of a Parameter
 * @param	paramName          Name of the parameter
 * @param	paramType          Type of the parameter. Used to make sure developer is using the right type.
 * @param	in 				Void pointer to memory where value is stored
 *
 * @return	1 if OK; 0 if error or parameter doesn't exist
 */
uint8_t setParamVal(param_names_t paramName, param_type_t paramType, void *in);

/**
 * @brief	Helper function to allow concurrent access to table
 * @param accessType       Type of access, either GET_PARAM or SET_PARAM
 * @param paramName        Name of the parameter
 * @param paramType        Type of the parameter. Used to make sure developer is using the right type.
 * @param p                Void pointer to buffer to store param value, should be enough to store value
 *
 * @return	1 if OK; 0 if error or parameter doesn't exist
 */
static uint8_t accessParamTable(access_type_t accessType, param_names_t paramName, param_type_t paramType, void *p);

/**
 * @brief	Get size of the parameter type in bytes
 * @param   paramType      Data type of the parameter.
 *
 * @return	parameter size if OK; 0 if error
 */
static param_size_t getParamSize(param_type_t type);

/**
 * @brief	Get pointer to parameter struct
 * @param   paramName      Name of the parameter.
 *
 * @return	Pointer to parameter struct, NULL if not found
 */
static param_handle_t getParamHandle(param_names_t paramName);

/**
 * @brief	Check if parameter is read-only
 * @param   paramHandle    Pointer to the parameter struct
 *
 * @return	1 if the parameter is read-only, 0 if not
 */
static uint8_t isReadOnly(param_handle_t paramHandle);

/**
 * @brief	Check if parameter is for telemetry
 * @param   paramHandle    Pointer to the parameter struct
 *
 * @return	1 if the parameter is for telemetry, 0 if not
 */
static uint8_t isTelemetry(param_handle_t paramHandle);

/**
 * @brief	Check if parameter should be stored in non-volatile memory
 * @param   paramHandle    Pointer to the parameter struct
 *
 * @return	1 if the parameter should be stored in non-volatile memory, 0 if not
 */
static uint8_t isPersistent(param_handle_t paramHandle);

#endif /* CDH_INCLUDE_PARAM_MANAGER_H_ */
