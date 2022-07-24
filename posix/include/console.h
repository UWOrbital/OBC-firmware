#ifndef POSIX_INCLUDE_CONSOLE_H_
#define POSIX_INCLUDE_CONSOLE_H_

#ifdef __cplusplus
    extern "C" {
#endif

/*
 * Initialize the mutex for the console.
 */
void initConsole(void);

/*
 * Print a string to the console.
 */
void printConsole(const char * fmt, ...);

#ifdef __cplusplus
    }
#endif

#endif /* POSIX_INCLUDE_CONSOLE_H_ */
