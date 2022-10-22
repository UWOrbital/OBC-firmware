#ifndef SDC_CMDLINE_H
#define SDC_CMDLINE_H

#include "ff.h"

/*
    Maximum number of args that can be parsed from a command line.
*/
#define CMDLINE_MAX_ARGS        8

/*
    Defines the size of the buffers that hold the path, or temporary data from
    the SD card.  There are two buffers allocated of this size.  The buffer size
    must be large enough to hold the longest expected full path name, including
    the file name, and a trailing null character.
*/
#define PATH_BUF_SIZE           80

/*
    Defines the size of the buffer that holds the command line.
*/
#define CMD_BUF_SIZE            64

/**
 * @brief A structure that holds a mapping between an FRESULT numerical code, and a
 * string representation.  FRESULT codes are returned from the FatFs FAT file
 * system driver.
 * @param iFResult FRESULT code
 * @param pcResultStr String representation
 */
typedef struct
{
    FRESULT iFResult;
    char *pcResultStr;
} fresult_str_t;

/* A macro to make it easy to add result codes to the table */
#define FRESULT_ENTRY(f)        { (f), (#f) }

/* A macro that holds the number of result codes */
#define NUM_FRESULT_CODES (sizeof(g_psFResultStrings)/sizeof(fresult_str_t))


#define CMDLINE_BAD_CMD         -1
#define CMDLINE_TOO_MANY_ARGS   -2
#define CMDLINE_TOO_FEW_ARGS    -3
#define CMDLINE_INVALID_ARG     -4

/**
 * @brief A function pointer to a command handler function.
 * @param iArgc The number of arguments that were parsed from the command line.
 * @param ppcArgv An array of strings.  Each string is an argument from the
 * command line.
 * @warning This won't be used in the satellite code; only for testing.
 */
typedef int (*pfnCmdLine)(int argc, char *argv[]);

/**
 * @brief A structure for an entry in the command list table.
 * @param pcCmd A pointer to a string containing the command name.
 * @param pfnCmd A pointer to the function that handles the command.
 * @param pcHelp A pointer to a string that holds a brief description of the
 * command, suitable for listing in a help menu.
 */
typedef struct {
    const char *pcCmd;
    pfnCmdLine pfnCmd;
    const char *pcHelp;
} cmd_line_entry_t;

/**
 * @brief Process a command line string.
 * 
 * @param pcCmdLine A pointer to a string containing the command line.
 * @return int Returns CMDLINE_BAD_CMD if the command is not found, or the
 * return code from the command handler function.
 */
int CmdLineProcess(char *pcCmdLine);

/**
 * @brief SD card test function.
 */
int SD_Test(void);

#endif // SDC_CMDLINE_H
