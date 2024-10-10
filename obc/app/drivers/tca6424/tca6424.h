#pragma once

#include "obc_errors.h"

#include <stdbool.h>
#include <stdint.h>

#define TCA6424A_PIN_00 0x00
#define TCA6424A_PIN_01 0x01
#define TCA6424A_PIN_02 0x02
#define TCA6424A_PIN_03 0x03
#define TCA6424A_PIN_04 0x04
#define TCA6424A_PIN_05 0x05
#define TCA6424A_PIN_06 0x06
#define TCA6424A_PIN_07 0x07

#define TCA6424A_PIN_10 0x10
#define TCA6424A_PIN_11 0x11
#define TCA6424A_PIN_12 0x12
#define TCA6424A_PIN_13 0x13
#define TCA6424A_PIN_14 0x14
#define TCA6424A_PIN_15 0x15
#define TCA6424A_PIN_16 0x16
#define TCA6424A_PIN_17 0x17

#define TCA6424A_PIN_20 0x20
#define TCA6424A_PIN_21 0x21
#define TCA6424A_PIN_22 0x22
#define TCA6424A_PIN_23 0x23
#define TCA6424A_PIN_24 0x24
#define TCA6424A_PIN_25 0x25
#define TCA6424A_PIN_26 0x26
#define TCA6424A_PIN_27 0x27

#define TCA6424A_GPIO_HIGH 1U
#define TCA6424A_GPIO_LOW 0U

typedef enum { TCA6424A_GPIO_CONFIG_OUTPUT = 0x00, TCA6424A_GPIO_CONFIG_INPUT } TCA6424A_gpio_config_t;

obc_error_code_t configureTCA6424APin(uint8_t pinLocation, TCA6424A_gpio_config_t gpioPinConfig);
obc_error_code_t readTCA6424APinInput(uint8_t pinLocation, uint8_t* IOPortValue);
obc_error_code_t driveTCA6424APinOutput(uint8_t pinLocation, uint8_t IOPortValue);
obc_error_code_t readTCA642CompleteInput(uint32_t* ioPortInput);
