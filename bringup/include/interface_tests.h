/**
 * @file interface_tests.h
 * @author Daniel Gobalakrishnan
 * @date 2022-07-03
 */
#ifndef INTERFACE_TESTS_H
#define INTERFACE_TESTS_H

#include "stdint.h"

/*
 * @brief  Initialize all interfaces
 */
void interface_init(void);

/*
 * @brief   Test the interface to the ADC
 * @param   None
 * @return  1 if successful, 0 if not
 */
uint8_t test_gpio(void);

/*
 * @brief   Test the interface to the ADC
 * @param   None
 * @return  1 if successful, 0 if not
 */
uint8_t test_adc(void);

/*
 * @brief  Test UART RX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_uart_rx(void);

/*
 * @brief  Test UART TX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_uart_tx(void);

/*
 * @brief  Test I2C RX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_i2c_rx(void);

/*
 * @brief  Test I2C TX
 * @param  None
 * @return 1 if successful, 0 if not
 */
uint8_t test_i2c_tx(void);

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


#endif /* INTERFACE_TESTS_H */