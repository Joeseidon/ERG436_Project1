/*
 * timerAInterface.h
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

#ifndef TIMERAINTERFACE_H_
#define TIMERAINTERFACE_H_

/* Application Defines  */
#define TIMER_PERIOD    0x2DC6

void startCounter(void);
void TA1_0_IRQHandler(void);
void timerAConfig(void);


#endif /* TIMERAINTERFACE_H_ */
