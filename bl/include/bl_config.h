#ifndef BL_INCLUDE_BL_CONFIG_H_
#define BL_INCLUDE_BL_CONFIG_H_

#define RM46
#define CRYSTAL_FREQ            16             // MHz
#define SYS_CLK_FREQ           180             // MHz

#define UART_ENABLE_UPDATE
#define UART_BAUDRATE     115200
#define UART              scilinREG
#define BL_DATA_BUFF_SIZE 64

//*****************************************************************************
// The starting address of the application.  This must be a multiple of 32K(sector size)
// bytes (making it aligned to a page boundary), and can not be 0 (the first sector is 
// boot loader). 
//
// The flash image of the boot loader must not be larger than this value.
//*****************************************************************************
#define APP_START_ADDRESS       0x00010020

//*****************************************************************************
// The address to store the update status of the application image
// It contains Application Start Address, Application Image Size, etc
//*****************************************************************************
#define APP_STATUS_ADDRESS       0x00010000

//*****************************************************************************
// Enables the pin-based forced update check.  When enabled, the boot loader
// will go into update mode instead of calling the application if a pin is read
// at a particular polarity, forcing an update operation.  In either case, the
// application is still able to return control to the boot loader in order to
// start an update. 
//*****************************************************************************
#define ENABLE_UPDATE_CHECK

#if defined (ENABLE_UPDATE_CHECK)

// GPIO config for the pin-based forced update check
#define FORCED_UPDATE_PORT  gioPORTA
#define FORCED_UPDATE_PIN   7

#endif

#endif // BL_INCLUDE_BL_CONFIG_H_
