/**
 * @file param_manager.h
 * @author Daniel Gobalakrishnan
 * @date 2022-06-01
 */

#ifndef PARAM_MANAGER_H
#define PARAM_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @enum	param_names_t
 * @brief	Parameter Names
 *
 * All parameter names
 * TODO: Add all parameter names
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
 * can have any size, is decision from user to assign the size.
*/
typedef enum
{
    UINT8_SIZE		= sizeof(uint8_t),
    INT8_SIZE		= sizeof(int8_t),
    UINT16_SIZE		= sizeof(uint16_t),
    INT16_SIZE		= sizeof(int16_t),
    UINT32_SIZE		= sizeof(uint32_t),
    INT32_SIZE		= sizeof(int32_t),
    UINT64_SIZE		= sizeof(uint64_t),
    INT64_SIZE		= sizeof(int64_t),
    FLOAT_SIZE		= sizeof(float),
    DOUBLE_SIZE		= sizeof(double)
    // String Size should be decided by user.
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
    TELEMETRY	= 0b001,		/**< Automatic collect this param for telemetry. */
    PERSISTENT	= 0b010,		/**< Persist the value on non volatile memory. */
    READ_ONLY	= 0b100 		/**< Prohibited to write with param_service functions, only applicable for parameterized variables. */
} param_opts_t;

/**
 * @union	param_val_t
 * @brief	Parameter value typess
 *
 * Macros for allowing various value types in parameters
*/
typedef union {
    uint8_t			u8;
    int8_t			i8;
    uint16_t		u16;
    int16_t			i16;
    uint32_t		u32;
    int32_t			i32;
    uint64_t		u64;
    int64_t			i64;
    float			f;
    double			d;
    char*			s;
} param_val_t;

/**
 * @define	NUM_PARAMS
 * @brief	Number of parameters in parameters table
 * @note	This macro should be updated when new parameters are added.
 *
 * Macro for the number of parameters in the table
*/
#define NUM_PARAMS 11

/**
 * @struct	param_t
 * @brief	Parameter struct
 *
 * Holds the data for a parameter in the list. The Parameter list
 * is composed by a collection of this struct.
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
 * @brief	Get the value of a Parameter
 * @param	param_name          Name of the parameter
 * @param	param_type          Type of the parameter. Used to make sure developer is using the right type.
 * @param	out_p               Void pointer to buffer to store param value, should be enough to store value
 *
 * @return	1 if OK; 0 if error, parameter doesn't exist, or buffer too small
 */
uint8_t get_param_val(param_names_t param_name, param_type_t param_type, void *out_p);

/**
 * @brief	Set the value of a Parameter
 * @param	param_name          Name of the parameter
 * @param	param_type          Type of the parameter. Used to make sure developer is using the right type.
 * @param	in_p 				Void pointer to memory where value is stored
 *
 * @return	1 if OK; 0 if error, parameter doesn't exist, or buffer too small
 */
uint8_t set_param_val(param_names_t param_name, param_type_t param_type, void *in_p);

param_size_t get_param_size(param_type_t type);

uint8_t access_param_table(access_type_t access_type, param_names_t param_name, param_type_t param_type, void *out_p);

param_handle_t get_param_handle(param_names_t param_name);

uint8_t initialize_mutex_array(void);

uint8_t is_read_only(param_names_t param_name);

uint8_t is_telemetry(param_names_t param_name);

uint8_t is_persistent(param_names_t param_name);

#endif /* PARAM_MANAGER_H */