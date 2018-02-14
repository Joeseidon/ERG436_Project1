/*
 * I2C_Interface.h
 *
 *  Created on: Feb 11, 2018
 *      Author: joe
 */

#ifndef I2C_INTERFACE_H_
#define I2C_INTERFACE_H_

#include <stdint.h>

void I2C_Init(void);
int8_t I2C_WRITE_STRING(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t byteCount);
int8_t I2C_WRITE_READ_STRING(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t byteCount);
void OneMsDelay(uint32_t n);
void user_delay_ms(uint32_t period);

#endif /* I2C_INTERFACE_H_ */
