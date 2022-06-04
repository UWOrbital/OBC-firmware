#ifndef PARAM_TABLE_H
#define PARAM_TABLE_H

#include "param_manager.h"

/**
 * @def  parameterize
 * @brief Macro to parameterize a variable into the Parameters Table.
 *
 * Use this macro to create parameters into the Parameters Table using a variable.
 * Variable should be visible in the scope where the Table is declared, this can be
 * done with the "extern" keyword.
 *
 * **Example:**
 * @code
 * uint32_t example_var;	// Variable to parameterize
 * // The Parameter Table
 * param_table_t param_table= {
 *		{.name="example",	.type=UINT32_PARAM,	.size=UINT32_SIZE,	.opts=TELEMETRY	.value=parameterize(example_var),
 * }
 * @endcode
 * @param	variable_name			Name of the variable to parameterize
 */
/* TODO: Decide if we want to store values as pointers or not */
/* #define parameterize(variable_name)  (void*)&variable_name */

/**
 * @brief Parameter table
 *
 * All parameters and their associated properties
 * @warning    Remember to add any new parameters to the param_names_t enum
 * @see param_names_t
 * TODO: Add all parameters
*/
param_t PARAM_TABLE [NUM_PARAMS] = {
        {.name=ALTITUDE, .type=DOUBLE_PARAM, .size=DOUBLE_SIZE, .opts=TELEMETRY, .value.i8=30},
        {.name=TEST_PARAM1, .type=UINT8_PARAM, .size=UINT8_SIZE, .opts=PERSISTENT|TELEMETRY, .value.i8=0},
        {.name=TEST_PARAM2, .type=UINT16_PARAM, .size=UINT16_SIZE, .opts=PERSISTENT, .value.i16=0},
        {.name=TEST_PARAM3, .type=UINT32_PARAM, .size=UINT32_SIZE, .opts=PERSISTENT|READ_ONLY, .value.i32=0},
        {.name=TEST_PARAM4, .type=UINT64_PARAM, .size=UINT64_SIZE, .opts=PERSISTENT, .value.i64=0},
        {.name=TEST_PARAM5, .type=INT8_PARAM, .size=INT8_SIZE, .opts=PERSISTENT, .value.i8=0},
        {.name=TEST_PARAM6, .type=INT16_PARAM, .size=INT16_SIZE, .opts=PERSISTENT, .value.i16=0},
        {.name=TEST_PARAM7, .type=INT32_PARAM, .size=INT32_SIZE, .opts=PERSISTENT|READ_ONLY, .value.i32=0},
        {.name=TEST_PARAM8, .type=INT64_PARAM, .size=INT64_SIZE, .opts=PERSISTENT, .value.i64=0},
        {.name=TEST_PARAM9, .type=FLOAT_PARAM, .size=FLOAT_SIZE, .opts=PERSISTENT, .value.f=0},
        {.name=TEST_PARAM10, .type=DOUBLE_PARAM, .size=DOUBLE_SIZE, .opts=PERSISTENT, .value.d=0}
};

#endif /* PARAM_TABLE_H */