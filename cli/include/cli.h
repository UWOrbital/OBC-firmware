#ifndef CLI_INCLUDE_CLI_H_
#define CLI_INCLUDE_CLI_H_

#include "obc_errors.h"

#include <stddef.h>
#include <stdint.h>

typedef obc_error_code_t (*cli_cmd_callback_t)(char *cmdInput, size_t cmdInputLen, char *outputBuff, size_t outputBuffLen);

typedef struct {
	const char * const cmd; // String that is entered to execute the command
	const char * const helpInfo; // Info displayed when "help" is entered
	const cli_cmd_callback_t callback;
	int8_t numParams; // Number of parameters expected by the command; -1 if variable number of parameters
} cli_cmd_info_t;

obc_error_code_t initCli(char * outputBuff, size_t outputBuffLen);

obc_error_code_t cliProcessCommand(char * cmdInput, size_t cmdInputLen, char * outputBuff, size_t outputBuffLen);

#endif /* CLI_INCLUDE_CLI_H_ */
