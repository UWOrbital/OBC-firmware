#include "cli.h"
#include "obc_logging.h"
#include "obc_errors.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include <string.h>
#include <stdint.h>

const char * cliInitMsg = "\r\n\r\nOBC CLI\r\n\r\n";

//--------------------------------------------------
// Command registration
//--------------------------------------------------

static obc_error_code_t helpCmdCallback(char * cmdInput, size_t cmdInputLen, char * outputBuff, size_t outputBuffLen);

static const cli_cmd_info_t helpCommand = {
    "help",
    "\r\nhelp:\r\n Lists all the registered commands\r\n\r\n",
    helpCmdCallback,
    0
};

// TODO: Add callbacks and info struct for all commands

static const cli_cmd_info_t *cmdList[] = {
    &helpCommand,
    // TODO: Add all commands here
};

#define NUM_CLI_CMDS sizeof(cmdList) / sizeof(cmdList[0])

//--------------------------------------------------

static obc_error_code_t getNumParams(char * cmdInput, size_t cmdInputLen, int8_t * numParams);

obc_error_code_t initCli(char * outputBuff, size_t outputBuffLen) {
    if (outputBuff == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    if (strlen(cliInitMsg) > outputBuffLen) {
        return OBC_ERR_CODE_BUFF_OVERFLOW;
    }

    memcpy(outputBuff, cliInitMsg, strlen(cliInitMsg));

    return OBC_ERR_CODE_SUCCESS;
}

obc_error_code_t cliProcessCommand(char * cmdInput, size_t cmdInputLen, char * outputBuff, size_t outputBuffLen) {
    obc_error_code_t errCode;

    if (cmdInput == NULL || outputBuff == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    // Iterate through the list of registered commands
    for (size_t i = 0; i < NUM_CLI_CMDS; i++) {
        size_t expectedCmdLen = strlen(cmdList[i]->cmd);

        if (expectedCmdLen > cmdInputLen) {
            continue;
        }

        // Check if the command matches the current command in the list
        if (strncmp(cmdInput, cmdList[i]->cmd, expectedCmdLen) == 0) {
            // Check if the command has the correct number of parameters
            int8_t numParams;
            RETURN_IF_ERROR_CODE(getNumParams(cmdInput, cmdInputLen, &numParams));

            if (numParams != cmdList[i]->numParams) {
                continue;
            }

            // Execute the command
            RETURN_IF_ERROR_CODE(cmdList[i]->callback(cmdInput, cmdInputLen, outputBuff, outputBuffLen));
            return OBC_ERR_CODE_SUCCESS;
        }
    }

    return OBC_ERR_CODE_CLI_CMD_NOT_FOUND;
}

static obc_error_code_t getNumParams(char * cmdInput, size_t cmdInputLen, int8_t * numParams) {
    if (cmdInput == NULL || numParams == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    *numParams = 0;

    size_t ix;    
    for (ix = 0; ix < cmdInputLen; ix++) {
        if (cmdInput[ix] == ' ') {
            (*numParams)++;
        }
    }

    return OBC_ERR_CODE_SUCCESS;
}

static obc_error_code_t helpCmdCallback(char * cmdInput, size_t cmdInputLen, char * outputBuff, size_t outputBuffLen) {
    if (cmdInput == NULL || outputBuff == NULL) {
        return OBC_ERR_CODE_INVALID_ARG;
    }

    memset(outputBuff, 0, outputBuffLen);

    size_t charsWritten = 0;

    for (size_t i = 0; i < NUM_CLI_CMDS; i++) {
        size_t helpInfoLen = strlen(cmdList[i]->helpInfo);

        if (charsWritten + helpInfoLen > outputBuffLen) {
            return OBC_ERR_CODE_BUFF_OVERFLOW;
        }

        memcpy(outputBuff + charsWritten, cmdList[i]->helpInfo, helpInfoLen);
        charsWritten += helpInfoLen;
    }

    return OBC_ERR_CODE_SUCCESS;
}