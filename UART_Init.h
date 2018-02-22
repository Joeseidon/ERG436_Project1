/*
 * UART_Init.h
 *
 *  Created on: Feb 20, 2018
 *      Author: joe
 */

#ifndef UART_INIT_H_
#define UART_INIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fputc(int _c, register FILE *_fp);

int fputs(const char *_ptr, register FILE *_fp);

void UART_init(void);

#endif /* UART_INIT_H_ */
