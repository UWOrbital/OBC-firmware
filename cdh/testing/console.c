#include <stdarg.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <semphr.h>

SemaphoreHandle_t xStdioMutex;

void console_init( void ) {
    xStdioMutex = xSemaphoreCreateMutex();
}

void console_print( const char * fmt, ... ) {
    va_list vargs;
    va_start( vargs, fmt );
    xSemaphoreTake( xStdioMutex, portMAX_DELAY );
    vprintf( fmt, vargs );
    xSemaphoreGive( xStdioMutex );
    va_end( vargs );
}
