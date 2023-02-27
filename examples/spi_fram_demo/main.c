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
    initSpiMutex();
    uint8_t chipID[FRAM_ID_LEN];
    char msg[50] = {0};
   
  
    //Read Manufacture ID
    framReadID(chipID, FRAM_ID_LEN);
    snprintf(msg, 50, "ID:%X %X %X %X %X %X %X %X %X\r\n", chipID[0], chipID[1], chipID[2], chipID[3], chipID[4], chipID[5], chipID[6], chipID[7], chipID[8]);
    // Note: This will send through the USB port on the LaunchPad
    sciPrintText((unsigned char *) msg, strlen(msg));

    // Toggle the LED.
    gioToggleBit(gioPORTB, 1);

    //Write 1 byte to 0x31415
    
    uint32_t addr = 0x31415;
    uint8_t byteData = 0xAB;
    snprintf(msg, 50, "Writting %X to %lX\r\n", byteData, addr);
    sciPrintText((unsigned char *) msg, strlen(msg));
    framWrite(addr, &byteData, 1);
    
    //Read 1 byte from 0x31415
    addr = 0x31415;
    byteData = 0;
    framRead(addr, &byteData, 1);
    snprintf(msg, 50, "Read %X from %lX\r\n", byteData, addr);
    sciPrintText((unsigned char *) msg, strlen(msg));

    //Multipe Bytes
    unsigned char hello_world[12] = {'H','e','l','l','o',' ','W','o','r','l','d'};
    snprintf(msg, 50, "Writting %s to %lX\r\n", hello_world, addr);
    sciPrintText((unsigned char *) msg, strlen(msg));
    //Write Hello World to 0x12345
    addr = 0x12345;
    framWrite(addr, hello_world, sizeof(hello_world));

    //Read Hello World from 0x12345
    memset(hello_world,0,sizeof(hello_world));
    framRead(addr, hello_world, sizeof(hello_world));
    snprintf(msg, 50, "Read %s from %lX\r\n", hello_world, addr);
    sciPrintText((unsigned char *) msg, strlen(msg));

    //Read Status Register
    framReadStatusReg(&byteData);
    snprintf(msg, 50, "Status Register: %X\r\n", byteData);
    sciPrintText((unsigned char *) msg, strlen(msg));

    uint8_t oldStatusReg = byteData;
    byteData = 0b00001100;
    snprintf(msg, 50, "Writting %X to Status Register\r\n", byteData);
    sciPrintText((unsigned char *) msg, strlen(msg));
    framWriteStatusReg(byteData);

    //Read Status Register
    framReadStatusReg(&byteData);
    snprintf(msg, 50, "Status Register: %X, Expected: 4C\r\n", byteData);
    sciPrintText((unsigned char *) msg, strlen(msg));

    //Reset Status Reg
    framWriteStatusReg(oldStatusReg);

    //Sleep
    sciPrintText((unsigned char *)"Going to sleep\r\n", strlen("Going to sleep\r\n"));
    framSleep();
    framWakeUp();
    //Read Hello World from 0x1234
    memset(hello_world,0,sizeof(hello_world));
    framRead(addr, hello_world, sizeof(hello_world));
    snprintf(msg, 50, "Read %s from %lX after wakeup\r\n", hello_world, addr);
    sciPrintText((unsigned char *) msg, strlen(msg));

    return 0;
}