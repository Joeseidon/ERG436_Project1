/*
 * timerAInterface.c
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

/* DriverLib Includes */
#include "driverlib.h"

#include "timerAInterface.h"

int timer_count = 0;

/* Timer_A UpMode Configuration Parameter */
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/1 = 3MHz //one second
        TIMER_PERIOD,                           // 5000 tick period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE ,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void timerAConfig(void){
    /* Configuring Timer_A1 for Up Mode */
    MAP_Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);
    MAP_Timer_A_registerInterrupt(TIMER_A1_BASE,TIMER_A_CCR0_INTERRUPT,TA1_0_IRQHandler);
}

void startCounter(void){
    MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
}

void TA1_0_IRQHandler(void){
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
                TIMER_A_CAPTURECOMPARE_REGISTER_0);
    timer_count++;
}
