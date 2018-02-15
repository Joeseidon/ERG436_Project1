/*
 * systick.h
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "driverlib.h"


void sysTick_Init(void);
void sysTick_Wait_ms(uint32_t);


#endif /* SYSTICK_H_ */
