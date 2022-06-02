/**
 * @file param_manager.h
 * @author Daniel Gobalakrishnan
 * @date 2022-06-01
 */

#ifndef PARAM_MANAGER_H
#define PARAM_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_PARAMS 6

/**
 * @typedef	param_handle_t
 * @brief	Parameter Handle Type
 *
 * Pointer to a parameter struct, i.e: entry in parameters table.
 * Use this type and the API functions for handling parameters.
 * @see set_param_val(), get_param_val(), set_param(), get_param()
*/
typedef param_t* param_handle_t;

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


typedef enum 
{
	GET_PARAM,
	SET_PARAM
} access_type_t;

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
	TELEMETRY,		/**< Automatic collect this param for telemetry. */
	PERSISTENT,		/**< Persist the value in non-volatile memory. */
	READ_ONLY   	/**< Prohibit writing to these variables. */
} param_opts_t;


/**
 * @enum	param_names_t
 * @brief	Parameter Names
 *
 * All parameter names
 * TODO: Add all parameter names
*/
typedef enum
{
	ALTITUDE
	// The rest of the parameters should be added here
} param_names_t;

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
 * @struct	param_t
 * @brief	Parameter struct
 *
 * Holds the data for a parameter in the list. The Parameter list
 * is composed by a collection of this struct.
 * @see param_list_t
 *
 * Do not handle this struct directly, use instead param_handle_t type
 * and the API defined functions.
 * @see param_handle_t
*/
typedef struct
{
	const param_names_t name;
	const param_type_t type;				/**< Param Type, from param_type_t. */
	const param_size_t size;						/**< Size in bytes, from param_size_t, or custom if STRING_PARAM.*/
	uint8_t opts;							/**< Special options, from param_opts_t.*/
	param_val_t value;						/**< Pointer to param space where value is stored. */
} param_t;

/**
 * @brief Parameter table
 *
 * All parameters and their associated properties
 * TODO: Add all parameters
*/
param_t PARAM_TABLE [] = {
	{.name=ALTITUDE, .type=DOUBLE_PARAM, .size=DOUBLE_SIZE, .opts=TELEMETRY, .value.i8=30},
};

/**
 * @brief	Get the value of a Parameter
 * @param	param   			Name of the parameter
 * @param	out_p 				Void pointer to buffer to store param value, should be enough to store value
 * @return	1 if OK; 0 if error, parmeter doesn't exist, or buffer too small
 */
uint8_t get_param_val(param_names_t param_name, param_type_t param_type, void *out_p);

/**
 * @brief	Set the value of a Parameter
 * @warning	Not thread-safe, user shall use mutex if sharing a param between many tasks, as with variables
 * @param	param  				Name of the param
 * @param	in_p 				Void pointer to memory where value is stored
 * @return	1 if OK; 0 if error, parmeter doesn't exist, or buffer too small
 */
uint8_t set_param_val(param_names_t param_name, param_type_t param_type, void *in_p);


#endif /* PARAM_MANAGER_H */