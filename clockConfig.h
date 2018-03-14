/*
 * clockConfig.h
 *
 *  Created on: Feb 4, 2018
 *      Author: joe
 */

#ifndef CLOCKCONFIG_H_
#define CLOCKCONFIG_H_

//Used to examine the SM clock frequency
extern int getSMCLKfreq(void);

//Used to examine the M clock frequency
extern int getMCLKfreq(void);

//Used to set MCLK = 48MHz and SMCLK = 12MHz
extern void clockStartUp(void);

//Initializes the high frequency external oscillator
extern void clockInit48MHzXTL(void);

#endif /* CLOCKCONFIG_H_ */
