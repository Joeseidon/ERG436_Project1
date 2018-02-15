/*
 * systick.c
 *
 *  Created on: Feb 15, 2018
 *      Author: lourw
 */

#include "systick.h"


// Setup systick
void sysTick_Init(void) {

    SysTick->CTRL = 0;
    SysTick->LOAD = 0xBB8;
    SysTick->VAL = 0;
    SysTick->CTRL = 0x5;

}

void sysTick_Wait_ms(uint32_t ms) {

    int i;
    for (i = ms; i > 0; i--) {
        SysTick->LOAD = 2998;
        SysTick->VAL = 0;

        while((SysTick->CTRL & 0x1000) == 0);
    }

}
