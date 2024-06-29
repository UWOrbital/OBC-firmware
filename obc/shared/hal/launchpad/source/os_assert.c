#include <FreeRTOSConfig.h>

__attribute__((noreturn))
__attribute__((weak))
void freertosConfigAssert( const char * pcFile, unsigned long ulLine) { while(1); }
