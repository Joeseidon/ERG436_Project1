/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_21_00_05 
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 Empty Project
 *
 * Description: An empty project that uses DriverLib
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST               |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 * Author: 
*******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*Personal Library Includes*/
#include "clockConfig.h"
#include "ST7735.h"
#include "LCD.h"

#include "bme280.h"

#include "BOSCH_Sensor.h"

#include "clockConfig.h"

#include "sysTick.h"

#include "I2C_Interface.h"

Light_Status currentStatus = DARK;

int current_count, target_count=8,light_status_updated=0;

int main(void)
{
    /* Halting the Watchdog  */
    MAP_WDT_A_holdTimer();

    clockStartUp();

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /* Configuring SysTick */
   // MAP_SysTick_enableModule();
    //MAP_SysTick_setPeriod(12000000);
    //MAP_Interrupt_enableSleepOnIsrExit();

    clockStartUp();

    I2C_Init();

    sysTickInit();

    rst=sensorInit();

    validAddress = verifyDeviceAddress();
    if(rst == BME280_E_DEV_NOT_FOUND){
        //chip not found
        ;
    }

    retrieveError=retrieveCalibratedData();

    LCD_init();

    create_data_display();
    print_current_status_pic(currentStatus);

    updateDataDisplay();

    MAP_SysTick_enableInterrupt();
    MAP_Interrupt_enableMaster();

    while(1)
    {
       if(light_status_updated){
           print_current_status_pic(currentStatus);
           light_status_updated=0;
       }
    }
}
void SysTick_Handler(void)
{
    current_count++;
    if(current_count==target_count){
        currentStatus++;
        light_status_updated = 1;
        current_count=0;
    }
}
