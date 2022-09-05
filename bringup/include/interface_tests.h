#ifndef BRINGUP_INCLUDE_INTERFACE_TESTS_H
#define BRINGUP_INCLUDE_INTERFACE_TESTS_H

#include "gio.h"
#include "adc.h"
#include "sci.h"
#include "spi.h"
#include "can.h"
#include "i2c.h"

#include <stdint.h>

/*
 * @brief  Initialize all interfaces
 */
void interface_init(void);

/*
 * @brief   Test the GPIO output
 */
void test_gpio_out(gioPORT_t *port, uint32_t pin, uint32_t value);

/*
 * @brief   Test the GPIO input
 */
uint8_t test_gpio_in(gioPORT_t *port, uint32_t pin);

/*
 * @brief   Test the interface to the ADC
 * @return  ADC value if successful, 0 if not
 */
uint16_t test_adc(adcBASE_t *adc, uint32_t channel);

/*
 * @brief  Test UART RX
 */
uint8_t test_uart_rx(sciBASE_t *sci);

/*
 * @brief  Test UART TX
 */
void test_uart_tx(sciBASE_t *sci, uint8_t tx_data);

/*
 * @brief  Test I2C TX
 * @return 1 if successful, 0 if not
 */
uint8_t test_i2c_tx(i2cBASE_t *i2c, uint8_t sAddr, uint8_t data);

/*
 * @brief  Test SPI RX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_spi_rx(void);

/*
 * @brief  Test SPI TX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_spi_tx(void);

/*
 * @brief  Test CAN RX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_can_rx(void);

/*
 * @brief  Test CAN TX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_can_tx(void);


#endif /* BRINGUP_INCLUDE_INTERFACE_TESTS_H */