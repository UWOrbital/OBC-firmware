#include "obc_sci_io.h"

#include "obc_spi_io.h"
#include "obc_sci_io.h"
#include "fram.h"

#include <gio.h>
#include <sci.h>
#include <spi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
    // Initialize hardware.
    
    gioInit();
    sciInit();
    spiInit();

    
    // Initialize the SCI mutex.
    initSciMutex();
    spiMutexInit();
    uint8_t chipID[9];
    char msg[50] = {0};
   
  
    //Read Manufacture ID
    framRead(NULL, &chipID, 9, RDID);
    snprintf(msg, 50, "ID:%X %X %X %X %X %X %X %X %X\r\n", chipID[0], chipID[1], chipID[2], chipID[3], chipID[4], chipID[5], chipID[6], chipID[7], chipID[8]);
    // Note: This will send through the USB port on the LaunchPad
    printTextSci(scilinREG, (unsigned char *) msg, strlen(msg));

    // Toggle the LED.
    gioToggleBit(gioPORTB, 1);

    //Write 1 byte to 0x31415
    
    void* addr = (void *)0x31415;
    uint8_t byteData = 0xAB;
    snprintf(msg, 50, "Writting %X to %p\r\n", byteData, addr);
    printTextSci(scilinREG, (unsigned char *) msg, strlen(msg));
    framWrite(addr, &byteData, sizeof(uint8_t), WRITE);
    
    //Read 1 byte from 0x3210
    addr = (void *)0x31415;
    byteData = 0;
    framRead(addr, &byteData, sizeof(uint8_t), READ);
    snprintf(msg, 50, "Read %X from %p\r\n", byteData, addr);
    printTextSci(scilinREG, (unsigned char *) msg, strlen(msg));

    //Multipe Bytes
    unsigned char hello_world[12] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    snprintf(msg, 50, "Writting %s to %p\r\n", hello_world, addr);
    printTextSci(scilinREG, (unsigned char *) msg, strlen(msg));
    //Write Hello World to 0x1234
    addr = 0x12345;
    framWrite(addr, hello_world, sizeof(hello_world), WRITE);

    //Read Hello World from 0x1234
    memset(hello_world,0,sizeof(hello_world));
    framRead(addr, hello_world, sizeof(hello_world), READ);
    snprintf(msg, 50, "Read %s from %p\r\n", hello_world, addr);
    printTextSci(scilinREG, (unsigned char *) msg, strlen(msg));

    return 0;
}