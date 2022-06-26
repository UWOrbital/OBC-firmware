#ifndef CONSOLE_H
    #define CONSOLE_H

    #ifdef __cplusplus
        extern "C" {
    #endif

    void console_init( void );
    void console_print( const char * fmt,
                        ... );

    #ifdef __cplusplus
        }
    #endif

#endif
