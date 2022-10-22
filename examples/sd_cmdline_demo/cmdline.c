#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "diskio.h"
#include "ff.h"
#include "cmdline.h"
#include "uartstdio.h"

extern void disk_timerproc(void);

//*****************************************************************************
//
// An array to hold the pointers to the command line arguments.
//
//*****************************************************************************
static char *g_ppcArgv[CMDLINE_MAX_ARGS + 1];

//*****************************************************************************
//
// This buffer holds the full path to the current working directory.  Initially
// it is root ("/").
//
//*****************************************************************************
static char g_pcCwdBuf[PATH_BUF_SIZE] = "/";

//*****************************************************************************
//
// A temporary data buffer used when manipulating file paths, or reading data
// from the SD card.
//
//*****************************************************************************
static char g_pcTmpBuf[PATH_BUF_SIZE];

//*****************************************************************************
//
// The buffer that holds the command line.
//
//*****************************************************************************
static char g_pcCmdBuf[CMD_BUF_SIZE];

//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and it's
// name as a string.  This is used for looking up error codes for printing to
// the console.
//
//*****************************************************************************
tFResultString g_psFResultStrings[] =
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


//*****************************************************************************
//
//! Process a command line string into arguments and execute the command.
//!
//! \param pcCmdLine points to a string that contains a command line that was
//! obtained by an application by some means.
//!
//! This function will take the supplied command line string and break it up
//! into individual arguments.  The first argument is treated as a command and
//! is searched for in the command table.  If the command is found, then the
//! command function is called and all of the command line arguments are passed
//! in the normal argc, argv form.
//!
//! The command table is contained in an array named <tt>g_psCmdTable</tt>
//! containing <tt>tCmdLineEntry</tt> structures which must be provided by the
//! application.  The array must be terminated with an entry whose \b pcCmd
//! field contains a NULL pointer.
//!
//! \return Returns \b CMDLINE_BAD_CMD if the command is not found,
//! \b CMDLINE_TOO_MANY_ARGS if there are more arguments than can be parsed.
//! Otherwise it returns the code that was returned by the command function.
//
//*****************************************************************************
int
CmdLineProcess(char *pcCmdLine)
{
    char *pcChar;
    uint_fast8_t ui8Argc;
    bool bFindArg = true;
    tCmdLineEntry *psCmdEntry;

    //
    // Initialize the argument counter, and point to the beginning of the
    // command line string.
    //
    ui8Argc = 0;
    pcChar = pcCmdLine;

    //
    // Advance through the command line until a zero character is found.
    //
    while(*pcChar)
    {
        //
        // If there is a space, then replace it with a zero, and set the flag
        // to search for the next argument.
        //
        if(*pcChar == ' ')
        {
            *pcChar = 0;
            bFindArg = true;
        }

        //
        // Otherwise it is not a space, so it must be a character that is part
        // of an argument.
        //
        else
        {
            //
            // If bFindArg is set, then that means we are looking for the start
            // of the next argument.
            //
            if(bFindArg)
            {
                //
                // As long as the maximum number of arguments has not been
                // reached, then save the pointer to the start of this new arg
                // in the argv array, and increment the count of args, argc.
                //
                if(ui8Argc < CMDLINE_MAX_ARGS)
                {
                    g_ppcArgv[ui8Argc] = pcChar;
                    ui8Argc++;
                    bFindArg = false;
                }

                //
                // The maximum number of arguments has been reached so return
                // the error.
                //
                else
                {
                    return(CMDLINE_TOO_MANY_ARGS);
                }
            }
        }

        //
        // Advance to the next character in the command line.
        //
        pcChar++;
    }

    //
    // If one or more arguments was found, then process the command.
    //
    if(ui8Argc)
    {
        //
        // Start at the beginning of the command table, to look for a matching
        // command.
        //
        psCmdEntry = &g_psCmdTable[0];

        //
        // Search through the command table until a null command string is
        // found, which marks the end of the table.
        //
        while(psCmdEntry->pcCmd)
        {
            //
            // If this command entry command string matches argv[0], then call
            // the function for this command, passing the command line
            // arguments.
            //
            if(!strcmp(g_ppcArgv[0], psCmdEntry->pcCmd))
            {
                return(psCmdEntry->pfnCmd(ui8Argc, g_ppcArgv));
            }

            //
            // Not found, so advance to the next entry.
            //
            psCmdEntry++;
        }
    }

    //
    // Fall through to here means that no matching command was found, so return
    // an error.
    //
    return(CMDLINE_BAD_CMD);
}

//*****************************************************************************
//
// This function returns a string representation of an error code that was
// returned from a function call to FatFs.  It can be used for printing human
// readable error messages.
//
//*****************************************************************************
const char *
StringFromFResult(FRESULT iFResult) {
    uint_fast8_t ui8Idx;

    //
    // Enter a loop to search the error code table for a matching error code.
    //
    for (ui8Idx = 0; ui8Idx < NUM_FRESULT_CODES; ui8Idx++) {
        //
        // If a match is found, then return the string name of the error code.
        //
        if (g_psFResultStrings[ui8Idx].iFResult == iFResult) {
            return (g_psFResultStrings[ui8Idx].pcResultStr);
        }
    }

    //
    // At this point no matching code was found, so return a string indicating
    // an unknown error.
    //
    return ("UNKNOWN ERROR CODE");
}

//*****************************************************************************
//
// This is the handler for this SysTick interrupt.  FatFs requires a timer tick
// every 10 ms for internal timing purposes.
//
//*****************************************************************************
void SysTickHandler(void) {  // todo: implement
    //
    // Call the FatFs tick timer.
    //
    disk_timerproc();
}

//*****************************************************************************
//
// This function implements the "ls" command.  It opens the current directory
// and enumerates through the contents, and prints a line for each item it
// finds.  It shows details such as file attributes, time and date, and the
// file size, along with the name.  It shows a summary of file sizes at the end
// along with free space.
//
//*****************************************************************************
int Cmd_ls(int argc, char *argv[]) {
    unsigned int ui32TotalSize;
    unsigned int ui32FileCount;
    unsigned int ui32DirCount;
    FRESULT iFResult;
    FATFS *psFatFs;
    char *pcFileName;
#if _USE_LFN
    char pucLfn[_MAX_LFN + 1];
    g_sFileInfo.lfname = pucLfn;
    g_sFileInfo.lfsize = sizeof(pucLfn);
#endif

    //
    // Open the current directory for access.
    //
    iFResult = f_opendir(&g_sDirObject, g_pcCwdBuf);

    //
    // Check for error and return if there is a problem.
    //
    if (iFResult != FR_OK) {
        return ((int) iFResult);
    }

    ui32TotalSize = 0;
    ui32FileCount = 0;
    ui32DirCount = 0;

    //
    // Give an extra blank line before the listing.
    //
    UARTprintf((unsigned char *)"\n");

    //
    // Enter loop to enumerate through all directory entries.
    //
    for (;;) {
        //
        // Read an entry from the directory.
        //
        iFResult = f_readdir(&g_sDirObject, &g_sFileInfo);

        //
        // Check for error and return if there is a problem.
        //
        if (iFResult != FR_OK) {
            return ((int) iFResult);
        }

        //
        // If the file name is blank, then this is the end of the listing.
        //
        if (!g_sFileInfo.fname[0]) {
            break;
        }

        //
        // If the attribue is directory, then increment the directory count.
        //
        if (g_sFileInfo.fattrib & AM_DIR) {
            ui32DirCount++;
        }

        //
        // Otherwise, it is a file.  Increment the file count, and add in the
        // file size to the total.
        //
        else {
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

    //
    // Print summary lines showing the file, dir, and size totals.
    //
    UARTprintf((unsigned char *)"\n%4u File(s),%10u bytes total\n%4u Dir(s)", ui32FileCount,
            ui32TotalSize, ui32DirCount);

    //
    // Get the free space.
    //
    iFResult = f_getfree("/", (DWORD *) &ui32TotalSize, &psFatFs);

    //
    // Check for error and return if there is a problem.
    //
    if (iFResult != FR_OK) {
        return ((int) iFResult);
    }

    //
    // Display the amount of free space that was calculated.
    //
    UARTprintf((unsigned char *)", %10uK bytes free\n",
            (ui32TotalSize * psFatFs->free_clust / 2));

    //
    // Made it to here, return with no errors.
    //
    return (0);
}

//*****************************************************************************
//
// This function implements the "cd" command.  It takes an argument that
// specifies the directory to make the current working directory.  Path
// separators must use a forward slash "/".  The argument to cd can be one of
// the following:
//
// * root ("/")
// * a fully specified path ("/my/path/to/mydir")
// * a single directory name that is in the current directory ("mydir")
// * parent directory ("..")
//
// It does not understand relative paths, so dont try something like this:
// ("../my/new/path")
//
// Once the new directory is specified, it attempts to open the directory to
// make sure it exists.  If the new path is opened successfully, then the
// current working directory (cwd) is changed to the new path.
//
//*****************************************************************************
int Cmd_cd(int argc, char *argv[]) {
    uint_fast8_t ui8Idx;
    FRESULT iFResult;

    //
    // Copy the current working path into a temporary buffer so it can be
    // manipulated.
    //
    strcpy(g_pcTmpBuf, g_pcCwdBuf);

    //
    // If the first character is /, then this is a fully specified path, and it
    // should just be used as-is.
    //
    if (argv[1][0] == '/') {
        //
        // Make sure the new path is not bigger than the cwd buffer.
        //
        if (strlen(argv[1]) + 1 > sizeof(g_pcCwdBuf)) {
            UARTprintf((unsigned char *)"Resulting path name is too long\n");
            return (0);
        }

        //
        // If the new path name (in argv[1])  is not too long, then copy it
        // into the temporary buffer so it can be checked.
        //
        else {
            strncpy(g_pcTmpBuf, argv[1], sizeof(g_pcTmpBuf));
        }
    }

    //
    // If the argument is .. then attempt to remove the lowest level on the
    // CWD.
    //
    else if (!strcmp(argv[1], "..")) {
        //
        // Get the index to the last character in the current path.
        //
        ui8Idx = strlen(g_pcTmpBuf) - 1;

        //
        // Back up from the end of the path name until a separator (/) is
        // found, or until we bump up to the start of the path.
        //
        while ((g_pcTmpBuf[ui8Idx] != '/') && (ui8Idx > 1)) {
            //
            // Back up one character.
            //
            ui8Idx--;
        }

        //
        // Now we are either at the lowest level separator in the current path,
        // or at the beginning of the string (root).  So set the new end of
        // string here, effectively removing that last part of the path.
        //
        g_pcTmpBuf[ui8Idx] = 0;
    }

    //
    // Otherwise this is just a normal path name from the current directory,
    // and it needs to be appended to the current path.
    //
    else {
        //
        // Test to make sure that when the new additional path is added on to
        // the current path, there is room in the buffer for the full new path.
        // It needs to include a new separator, and a trailing null character.
        //
        if (strlen(g_pcTmpBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcCwdBuf)) {
            UARTprintf((unsigned char *)"Resulting path name is too long\n");
            return (0);
        }

        //
        // The new path is okay, so add the separator and then append the new
        // directory to the path.
        //
        else {
            //
            // If not already at the root level, then append a /
            //
            if (strcmp(g_pcTmpBuf, "/")) {
                strcat(g_pcTmpBuf, "/");
            }

            //
            // Append the new directory to the path.
            //
            strcat(g_pcTmpBuf, argv[1]);
        }
    }

    //
    // At this point, a candidate new directory path is in chTmpBuf.  Try to
    // open it to make sure it is valid.
    //
    iFResult = f_opendir(&g_sDirObject, g_pcTmpBuf);

    //
    // If it can't be opened, then it is a bad path.  Inform the user and
    // return.
    //
    if (iFResult != FR_OK) {
        UARTprintf((unsigned char *)"cd: %s\n", g_pcTmpBuf);
        return ((int) iFResult);
    }

    //
    // Otherwise, it is a valid new path, so copy it into the CWD.
    //
    else {
        strncpy(g_pcCwdBuf, g_pcTmpBuf, sizeof(g_pcCwdBuf));
    }

    //
    // Return success.
    //
    return (0);
}

//*****************************************************************************
//
// This function implements the "pwd" command.  It simply prints the current
// working directory.
//
//*****************************************************************************
int Cmd_pwd(int argc, char *argv[]) {
    //
    // Print the CWD to the console.
    //
    UARTprintf((unsigned char *)"%s\n", g_pcCwdBuf);

    //
    // Return success.
    //
    return (0);
}

//uint16_t bswap16(uint16_t a)
//{
//  a = ((a & 0x00FF) << 8) | ((a & 0xFF00) >> 8);
//  return a;
//}

//uint32_t bswap32(uint32_t a)
//{
//  a = ((a & 0x000000FF) << 24) |
//      ((a & 0x0000FF00) <<  8) |
//      ((a & 0x00FF0000) >>  8) |
//      ((a & 0xFF000000) >> 24);
//  return a;
//}

//uint16_t Color565(uint8_t r, uint8_t g, uint8_t b)
//{
//  return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
//}

//*****************************************************************************
//
// This function implements the "cat" command.  It reads the contents of a file
// and prints it to the console.  This should only be used on text files.  If
// it is used on a binary file, then a bunch of garbage is likely to printed on
// the console.
//
//*****************************************************************************
int Cmd_cat(int argc, char *argv[]) {
    FRESULT iFResult;
    unsigned int ui32BytesRead;

    //
    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    //
    if (strlen(g_pcCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_pcTmpBuf)) {
        UARTprintf((unsigned char *)"Resulting path name is too long\n");
        return (0);
    }

    //
    // Copy the current path to the temporary buffer so it can be manipulated.
    //
    strcpy(g_pcTmpBuf, g_pcCwdBuf);

    //
    // If not already at the root level, then append a separator.
    //
    if (strcmp("/", g_pcCwdBuf)) {
        strcat(g_pcTmpBuf, "/");
    }

    //
    // Now finally, append the file name to result in a fully specified file.
    //
    strcat(g_pcTmpBuf, argv[1]);

    //
    // Open the file for reading.
    //
    iFResult = f_open(&g_sFileObject, g_pcTmpBuf, FA_READ);

    //
    // If there was some problem opening the file, then return an error.
    //
    if (iFResult != FR_OK) {
        return ((int) iFResult);
    }

    //
    // Enter a loop to repeatedly read data from the file and display it, until
    // the end of the file is reached.
    //
    do {
        //
        // Read a block of data from the file.  Read as much as can fit in the
        // temporary buffer, including a space for the trailing null.
        //
        iFResult = f_read(&g_sFileObject, g_pcTmpBuf, sizeof(g_pcTmpBuf) - 1,
                (UINT *) &ui32BytesRead);

        //
        // If there was an error reading, then print a newline and return the
        // error to the user.
        //
        if (iFResult != FR_OK) {
            UARTprintf((unsigned char *)"\n");
            return ((int) iFResult);
        }

        //
        // Null terminate the last block that was read to make it a null
        // terminated string that can be used with printf.
        //
        g_pcTmpBuf[ui32BytesRead] = 0;

        //
        // Print the last chunk of the file that was received.
        //
        UARTprintf((unsigned char *)"%s", g_pcTmpBuf);
    } while (ui32BytesRead == sizeof(g_pcTmpBuf) - 1);

    //
    // Return success.
    //
    return (0);
}

//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list of the
// available commands with a brief description.
//
//*****************************************************************************
int Cmd_help(int argc, char *argv[]) {
    tCmdLineEntry *psEntry;

    //
    // Print some header text.
    //
    UARTprintf((unsigned char *)"\nAvailable commands\n");
    UARTprintf((unsigned char *)"------------------\n");

    //
    // Point at the beginning of the command table.
    //
    psEntry = &g_psCmdTable[0];

    //
    // Enter a loop to read each entry from the command table.  The end of the
    // table has been reached when the command name is NULL.
    //
    while (psEntry->pcCmd) {
        //
        // Print the command name and the brief description.
        //
        UARTprintf((unsigned char *)"%6s: %s\n", psEntry->pcCmd, psEntry->pcHelp);

        //
        // Advance to the next entry in the table.
        //
        psEntry++;
    }

    //
    // Return success.
    //
    return (0);
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions, and
// brief description.
//
//*****************************************************************************
tCmdLineEntry g_psCmdTable[] = {
        { "help", Cmd_help, "Display list of commands" }, { "h", Cmd_help,
                "alias for help" }, { "?", Cmd_help, "alias for help" }, { "ls",
                Cmd_ls, "Display list of files" }, { "chdir", Cmd_cd,
                "Change directory" }, { "cd", Cmd_cd, "alias for chdir" }, {
                "pwd", Cmd_pwd, "Show current working directory" }, { "cat",
                Cmd_cat, "Show contents of a text file" },
#ifdef LCD
        {    "load", Cmd_load, "Load a bmp file"},                  // Load_bmp.c
#endif
        { 0, 0, 0 } };

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned int ui32Line)
{
}
#endif



/*------------------------------------------------------------/
/ Open or create a file in append mode
/------------------------------------------------------------*/

FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const char* path    /* [IN]  File name to be opened */
)
{
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


//*****************************************************************************
//
// The program main function.  It performs initialization, then runs a command
// processing loop to read commands from the console.
//
//*****************************************************************************
int SD_Test(void) {
    int nStatus;
    FRESULT iFResult;

    iFResult = f_mount(&g_sFatFs, "", 1);

    if (iFResult != FR_OK) {
        UARTprintf((unsigned char *)"f_mount error: %s\n", StringFromFResult(iFResult));
        return (1);
    }

    // todo jc write soimething
#define TEST_FILENAME    "WSLOG.TXT"
    FRESULT res;                /* FatFs function common result code */

    // write some info
    FIL fsrc;                /* File objects */

      /* Open  the file for append */
      res = open_append(&fsrc, TEST_FILENAME);
      if (res != FR_OK) {
          /* Error. Cannot create the file */
          while(1);
      }

      // if file empty, write header
      if (! f_size(&fsrc)) {
          res = f_printf(&fsrc, "temperature;humidity;uv\n");
        if (res <0) {
            /* Error. Cannot write header */
            while(1);
        }
      }


      res = f_printf(&fsrc, "%08u;%08u;%08u\n", 1, 2, 3);
      if (res < FR_OK) {
          /* Error. Cannot log data */
          while(1);
      }

      /* Close the file */
      res = f_close(&fsrc);
      if (res != FR_OK)
      {
        /* Error. Cannot close the file */
        while(1);
      }

    //
    // Enter an infinite loop for reading and processing commands from the
    // user.
    //
    while (1) {
        //
        // Print a prompt to the console.  Show the CWD.
        //
        UARTprintf((unsigned char *)"\n%s> ", g_pcCwdBuf);

        //
        // Get a line of text from the user.
        //
        UARTgets((unsigned char *)g_pcCmdBuf, sizeof(g_pcCmdBuf));

        // Pass the line from the user to the command processor.  It will be
        // parsed and valid commands executed.

        nStatus = CmdLineProcess(g_pcCmdBuf);

        //
        // Handle the case of bad command.
        //
        if (nStatus == CMDLINE_BAD_CMD) {
            UARTprintf((unsigned char *)"Bad command!\n");
        }

        //
        // Handle the case of too many arguments.
        //
        else if (nStatus == CMDLINE_TOO_MANY_ARGS) {
            UARTprintf((unsigned char *)"Too many arguments for command processor!\n");
        }

        //
        // Otherwise the command was executed.  Print the error code if one was
        // returned.
        //
        else if (nStatus != 0) {
            UARTprintf((unsigned char *)"Command returned error code %s\n",
                    StringFromFResult((FRESULT) nStatus));
        }
    }
}
