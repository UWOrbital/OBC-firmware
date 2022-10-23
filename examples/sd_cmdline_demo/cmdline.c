//*****************************************************************************
//
// cmdline.c - Functions to help with processing command lines.
//
// Copyright (c) 2007-2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "diskio.h"
#include "ff.h"
#include "cmdline.h"
#include "uartstdio.h"

extern void disk_timerproc(void);

// Holds pointers to command line arguments
static char *g_ppcArgv[CMDLINE_MAX_ARGS + 1];

// Current working directory.
static char g_pcCwdBuf[PATH_BUF_SIZE] = "/";

// A temporary data buffer used when manipulating file paths, or reading data
// from the SD card.
static char g_pcTmpBuf[PATH_BUF_SIZE];

// The buffer that holds the command line.
static char g_pcCmdBuf[CMD_BUF_SIZE];

// The following are data structures used by FatFs.
static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

// This is the command table that holds the command names, implementing functions,
// and brief description.
extern cmd_line_entry_t g_psCmdTable[];

// Maps error codes to strings.
fresult_str_t g_psFResultStrings[] =
{
    FRESULT_ENTRY(FR_OK),
    FRESULT_ENTRY(FR_DISK_ERR),
    FRESULT_ENTRY(FR_INT_ERR),
    FRESULT_ENTRY(FR_NOT_READY),
    FRESULT_ENTRY(FR_NO_FILE),
    FRESULT_ENTRY(FR_NO_PATH),
    FRESULT_ENTRY(FR_INVALID_NAME),
    FRESULT_ENTRY(FR_DENIED),
    FRESULT_ENTRY(FR_EXIST),
    FRESULT_ENTRY(FR_INVALID_OBJECT),
    FRESULT_ENTRY(FR_WRITE_PROTECTED),
    FRESULT_ENTRY(FR_INVALID_DRIVE),
    FRESULT_ENTRY(FR_NOT_ENABLED),
    FRESULT_ENTRY(FR_NO_FILESYSTEM),
    FRESULT_ENTRY(FR_MKFS_ABORTED),
    FRESULT_ENTRY(FR_TIMEOUT),
    FRESULT_ENTRY(FR_LOCKED),
    FRESULT_ENTRY(FR_NOT_ENOUGH_CORE),
    FRESULT_ENTRY(FR_TOO_MANY_OPEN_FILES),
    FRESULT_ENTRY(FR_INVALID_PARAMETER),
};

/**
 * @brief Process a command line string into arguments and execute the command.
 * 
 * @param pcCmdLine Points to a string that contains a command line.
 * @return int Returns CMDLINE_BAD_CMD if the command is not found, or the
 * return code from the command handler function.
 */
int cmdLineProcess(char *pcCmdLine) {
    if (pcCmdLine == NULL)
        return CMDLINE_BAD_CMD;
    
    char *pcChar = pcCmdLine; // Pointer to the first character in the command line
    uint8_t ui8Argc = 0; // Number of arguments found
    bool bFindArg = true; // True if we are looking for the start of an argument
    cmd_line_entry_t *psCmdEntry; // Pointer to the command entry in the command table

    ui8Argc = 0;
    pcChar = pcCmdLine;

    // Iterate over the characters in the command line
    while(*pcChar) {
        if (*pcChar == ' ') {
            *pcChar = 0;
            bFindArg = true;
        } else {
            if (bFindArg) {
                if (ui8Argc < CMDLINE_MAX_ARGS) {
                    g_ppcArgv[ui8Argc] = pcChar;
                    ui8Argc++;
                    bFindArg = false;
                } else {
                    return CMDLINE_TOO_MANY_ARGS ;
                }
            }
        }
        pcChar++;
    }

    // If one or more arguments was found, then process the command.
    if (ui8Argc) {
        // Iterate over the command table until a matching command is found
        psCmdEntry = &g_psCmdTable[0];
        while (psCmdEntry->pcCmd) {
            if (!strcmp(g_ppcArgv[0], psCmdEntry->pcCmd)) {
                return psCmdEntry->pfnCmd(ui8Argc, g_ppcArgv);
            }
            psCmdEntry++;
        }
    }
    return CMDLINE_BAD_CMD;
}

/**
 * @brief Convert an error code to a string.
 * 
 * @param iFResult The error code to convert.
 * @return const char* A pointer to the string representation of the error code.
 */
const char * stringFromFResult(FRESULT iFResult) {
    uint8_t ui8Idx;

    // Loop through the error code table looking for a matching error code.
    for (ui8Idx = 0; ui8Idx < NUM_FRESULT_CODES; ui8Idx++) {
        if (g_psFResultStrings[ui8Idx].iFResult == iFResult) {
            return (g_psFResultStrings[ui8Idx].pcResultStr);
        }
    }

    return "UNKNOWN ERROR CODE";
}

/**
 * @brief SysTick interrupt handler.
 * @note FatFs requires a timer tick every 10 ms for internal timing purposes.
 */
void sysTickHandler(void) {
    disk_timerproc();
}

/**
 * @brief List the contents of a directory.
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int Error code.
 */
int lsCmd(int argc, char *argv[]) {
    uint32_t ui32TotalSize;
    uint32_t ui32FileCount;
    uint32_t ui32DirCount;
    FRESULT iFResult;
    FATFS *psFatFs;
    char *pcFileName;

#if _USE_LFN
    char pucLfn[_MAX_LFN + 1];
    g_sFileInfo.lfname = pucLfn;
    g_sFileInfo.lfsize = sizeof(pucLfn);
#endif

    // Open the current directory for access.
    iFResult = f_opendir(&g_sDirObject, g_pcCwdBuf);

    // Check for error and return if there is a problem.
    if (iFResult != FR_OK)
        return (int)iFResult;

    ui32TotalSize = 0;
    ui32FileCount = 0;
    ui32DirCount = 0;

    // Give an extra blank line before the listing.
    UARTprintf((unsigned char *)"\n");

    // Enter loop to enumerate through all directory entries.
    for (;;) {
        // Read an entry from the directory.
        iFResult = f_readdir(&g_sDirObject, &g_sFileInfo);

        // Check for error and return if there is a problem.
        if (iFResult != FR_OK)
            return (int)iFResult;

        // If the file name is blank, then this is the end of the listing.
        if (!g_sFileInfo.fname[0])
            break;

        // If the attribue is directory, then increment the directory count.
        if (g_sFileInfo.fattrib & AM_DIR) {
            ui32DirCount++;
        } else {
            ui32FileCount++;
            ui32TotalSize += g_sFileInfo.fsize;
        }

#if _USE_LFN
        pcFileName = ((*g_sFileInfo.lfname)?g_sFileInfo.lfname:g_sFileInfo.fname);
#else
        pcFileName = g_sFileInfo.fname;
#endif
        //
        // Print the entry information on a single line with formatting to show
        // the attributes, date, time, size, and name.
        //
        UARTprintf((unsigned char *)"%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
                (g_sFileInfo.fattrib & AM_DIR) ? 'D' : '-',
                (g_sFileInfo.fattrib & AM_RDO) ? 'R' : '-',
                (g_sFileInfo.fattrib & AM_HID) ? 'H' : '-',
                (g_sFileInfo.fattrib & AM_SYS) ? 'S' : '-',
                (g_sFileInfo.fattrib & AM_ARC) ? 'A' : '-',
                (g_sFileInfo.fdate >> 9) + 1980, (g_sFileInfo.fdate >> 5) & 15,
                g_sFileInfo.fdate & 31, (g_sFileInfo.ftime >> 11),
                (g_sFileInfo.ftime >> 5) & 63, g_sFileInfo.fsize, pcFileName);
    }

    // Print summary lines showing the file, dir, and size totals.
    UARTprintf((unsigned char *)"\n%4u File(s),%10u bytes total\n%4u Dir(s)", ui32FileCount,
            ui32TotalSize, ui32DirCount);

    // Get the free space.
    iFResult = f_getfree("/", (DWORD *) &ui32TotalSize, &psFatFs);

    // Check for error and return if there is a problem.
    if (iFResult != FR_OK) {
        return (int)iFResult;
    }

    // Display the amount of free space that was calculated.
    UARTprintf((unsigned char *)", %10uK bytes free\n",
            (ui32TotalSize * psFatFs->free_clust / 2));

    return 0; // Success
}

/**
 * @brief Change the current working directory.
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int Error code.
 */
int cdCmd(int argc, char *argv[]) {
    uint8_t ui8Idx;
    FRESULT iFResult;

    // Copy the current working path into a temporary buffer so it can be
    // manipulated.
    strcpy(g_pcTmpBuf, g_pcCwdBuf);

    // If the first character is /, then this is a fully specified path, and it
    // should just be used as-is.
    if (argv[1][0] == '/') {
        // Make sure the new path is not bigger than the cwd buffer.
        if (strlen(argv[1]) + 1 > sizeof(g_pcCwdBuf)) {
            UARTprintf((unsigned char *)"Resulting path name is too long\n");
            return (0);
        } else {
            strncpy(g_pcTmpBuf, argv[1], sizeof(g_pcTmpBuf));
        }
    } else if (!strcmp(argv[1], "..")) {
        // Get the index to the last character in the current path.
        ui8Idx = strlen(g_pcTmpBuf) - 1;

        // Back up from the end of the path name until a separator (/) is
        // found, or until we bump up to the start of the path.
        while ((g_pcTmpBuf[ui8Idx] != '/') && (ui8Idx > 1)) {
            // Back up one character.
            ui8Idx--;
        }

        // Now we are either at the lowest level separator in the current path,
        // or at the beginning of the string (root).  So set the new end of
        // string here, effectively removing that last part of the path.
        g_pcTmpBuf[ui8Idx] = 0;
    }

    // Otherwise this is just a normal path name from the current directory,
    // and it needs to be appended to the current path.
    else {
        // Test to make sure that when the new additional path is added on to
        // the current path, there is room in the buffer for the full new path.
        // It needs to include a new separator, and a trailing null character.
        if (strlen(g_pcTmpBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcCwdBuf)) {
            UARTprintf((unsigned char *)"Resulting path name is too long\n");
            return (0);
        } else {
            // The new path is okay, so add the separator and then append the new
            // directory to the path.

            // If not already at the root level, then append a /
            if (strcmp(g_pcTmpBuf, "/")) {
                strcat(g_pcTmpBuf, "/");
            }

            // Append the new directory to the path.
            strcat(g_pcTmpBuf, argv[1]);
        }
    }

    // At this point, a candidate new directory path is in chTmpBuf.  Try to
    // open it to make sure it is valid.
    iFResult = f_opendir(&g_sDirObject, g_pcTmpBuf);

    // If it can't be opened, then it is a bad path.  Inform the user and
    // return.
    if (iFResult != FR_OK) {
        UARTprintf((unsigned char *)"cd: %s\n", g_pcTmpBuf);
        return (int)iFResult;
    }

    // Otherwise, it is a valid new path, so copy it into the CWD.
    else {
        strncpy(g_pcCwdBuf, g_pcTmpBuf, sizeof(g_pcCwdBuf));
    }

    // Return success.
    return 0;
}

/**
 * @brief Print the current working directory.
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int Error code.
 */
int pwdCmd(int argc, char *argv[]) {
    UARTprintf((unsigned char *)"%s\n", g_pcCwdBuf);
    return 0;
}

/**
 * @brief Print the contents of a file.
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int Error code.
 * @warning This function should only be used for text files.
 */
int catCmd(int argc, char *argv[]) {
    FRESULT iFResult;
    unsigned int ui32BytesRead;

    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
        UARTprintf((unsigned char *)"Resulting path name is too long\n");
        return 0;
    }

    // Copy the current path to the temporary buffer so it can be manipulated.
    strcpy(g_pcTmpBuf, g_pcCwdBuf);

    // If not already at the root level, then append a separator.
    if (strcmp("/", g_pcCwdBuf)) {
        strcat(g_pcTmpBuf, "/");
    }

    // Now finally, append the file name to result in a fully specified file.
    strcat(g_pcTmpBuf, argv[1]);

    // Open the file for reading.
    iFResult = f_open(&g_sFileObject, g_pcTmpBuf, FA_READ);

    // If there was some problem opening the file, then return an error.
    if (iFResult != FR_OK) {
        return ((int) iFResult);
    }

    // Enter a loop to repeatedly read data from the file and display it, until
    // the end of the file is reached.
    do {
        // Read a block of data from the file.  Read as much as can fit in the
        // temporary buffer, including a space for the trailing null.
        iFResult = f_read(&g_sFileObject, g_pcTmpBuf, sizeof(g_pcTmpBuf) - 1,
                (UINT *) &ui32BytesRead);

        // If there was an error reading, then print a newline and return the
        // error to the user.
        if (iFResult != FR_OK) {
            UARTprintf((unsigned char *)"\n");
            return (int)iFResult;
        }

        // Null terminate the last block that was read to make it a null
        // terminated string that can be used with printf.
        g_pcTmpBuf[ui32BytesRead] = 0;

        // Print the last chunk of the file that was received.
        UARTprintf((unsigned char *)"%s", g_pcTmpBuf);
    } while (ui32BytesRead == sizeof(g_pcTmpBuf) - 1);

    return 0; // Success.
}

/**
 * @brief Print a list of commands.
 * 
 * @param argc Number of arguments.
 * @param argv Array of arguments.
 * @return int Error code.
 */
int cmdHelp(int argc, char *argv[]) {
    cmd_line_entry_t *psEntry;

    UARTprintf((unsigned char *)"\nAvailable commands\n");
    UARTprintf((unsigned char *)"------------------\n");

    psEntry = &g_psCmdTable[0];

    // Print the command strings and brief description.
    while (psEntry->pcCmd) {
        UARTprintf((unsigned char *)"%6s: %s\n", psEntry->pcCmd, psEntry->pcHelp);
        psEntry++;
    }
    return 0; // Success.
}

/**
 * @brief Command table w/ command names, implementing functions, and brief description.
 */
cmd_line_entry_t g_psCmdTable[] = {
        { "help", cmdHelp, "Display list of commands" }, 
        { "h", cmdHelp, "alias for help" }, 
        { "?", cmdHelp, "alias for help" }, 
        { "ls", lsCmd, "Display list of files" }, 
        { "chdir", cdCmd, "Change directory" }, 
        { "cd", cdCmd, "alias for chdir" }, 
        { "pwd", pwdCmd, "Show current working directory" }, 
        { "cat", catCmd, "Show contents of a text file" },
        { 0, 0, 0 } // Used to signal end of table.
        };

/**
 * @brief Open file in append mode
 * @param fp Pointer to file object
 * @param filename Name of file to open
 */
FRESULT f_open_append(FIL* fp, const char* path) {
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
        fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}

/**
 * @brief SD Card Test
 */
int testCmdLineSDC(void) {
    int nStatus;
    FRESULT iFResult;

    iFResult = f_mount(&g_sFatFs, "", 1);

    if (iFResult != FR_OK) {
        UARTprintf((unsigned char *)"f_mount error: %s\n", stringFromFResult(iFResult));
        return 1;
    }

    FRESULT res;                /* FatFs function common result code */
    FIL fsrc;                /* File objects */

    // Append to a test file
    res = f_open_append(&fsrc, TEST_FILENAME);
    if (res != FR_OK)
        while(1);

    if (!f_size(&fsrc)) {
        res = f_printf(&fsrc, "THIS IS A TEST FILE\n");
        if (res < 0)
            while(1);
    }


    res = f_printf(&fsrc, "TEST DATA: %08u, %08u, %08u\n", 1, 2, 3);
    if (res < FR_OK)
        while(1);

    /* Close the file */
    res = f_close(&fsrc);
    if (res != FR_OK)
        while(1);

    while (1) {
        UARTprintf((unsigned char *)"\n%s> ", g_pcCwdBuf);
        UARTgets((unsigned char *)g_pcCmdBuf, sizeof(g_pcCmdBuf));
        nStatus = cmdLineProcess(g_pcCmdBuf);

        if (nStatus == CMDLINE_BAD_CMD) {
            UARTprintf((unsigned char *)"Bad command!\n");
        } else if (nStatus == CMDLINE_TOO_MANY_ARGS) {
            UARTprintf((unsigned char *)"Too many arguments for command processor!\n");
        } else if (nStatus != 0) {
            UARTprintf((unsigned char *)"Command returned error code %s\n",
                        stringFromFResult((FRESULT) nStatus));
        }
    }
}
