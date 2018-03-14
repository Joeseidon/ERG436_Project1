/*
 * timer_32.h
 *
 *  Created on: Feb 2018
 *      Author: lourw
 */

#ifndef TIMER_32_H_
#define TIMER_32_H_

#include "driverlib.h"

/*
 * Used to initialize timer32 in free run mode with a prescaler of 256.
 */
void timer32_Init(void);

/*
 * Uses timer32 to create n ms delay
 */
void timer32_Wait_ms(uint32_t);


#endif /* TIMER_32_H_ */
