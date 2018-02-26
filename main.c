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
#include "environment_sensor.h"
#include "ST7735.h"
#include "LCD.h"

#include "RTC_Module.h"

#include "msp432_spi.h"
#include "msprf24.h"

Light_Status currentStatus = DARK;

int current_count, target_count=8,light_status_updated=0;

int second_count;
int reset_time = 0;

extern volatile display_cell inside;
extern volatile display_cell outside;
int res;
int read_sensor = 0;
struct bme280_dev dev;
struct bme280_data compensated_data;

//RF
extern volatile uint8_t rf_irq;
volatile unsigned int user;

int main(void)
{
    /* Halting the Watchdog  */
    //MAP_WDT_A_holdTimer();
    uint8_t addr[5];
    uint8_t buf[32];

    // Red LED will be our output
    P1DIR |= BIT0;
    P1OUT &= ~(BIT0);

    //setup IRQ
    P5DIR &= ~BIT0;  // IRQ line is input
    P5OUT |= BIT0;   // Pull-up resistor enabled
    P5REN |= BIT0;
    P5IES |= BIT0;   // Trigger on falling-edge
    P5IFG &= ~BIT0;  // Clear any outstanding IRQ
    P5IE |= BIT0;    // Enable IRQ interrupt
    P5IFG &= 0; // clear flag

    MAP_Interrupt_enableInterrupt(INT_PORT5);

    user = 0xFE;

    clockStartUp();

    /* Initial values for nRF24L01+ library config variables */
    rf_crc = RF24_EN_CRC | RF24_CRCO; // CRC enabled, 16-bit
    rf_addr_width      = 5;
    rf_speed_power     = RF24_SPEED_1MBPS | RF24_POWER_0DBM;
    rf_channel         = 120;

    msprf24_init();
    msprf24_set_pipe_packetsize(0, 32);
    msprf24_open_pipe(0, 1);  // Open pipe#0 with Enhanced ShockBurst

    // Set our RX address
    addr[0] = 0xDE; addr[1] = 0xAD; addr[2] = 0xBE; addr[3] = 0xEF; addr[4] = 0x00;
    w_rx_addr(0, addr);

    // Receive mode
    if (!(RF24_QUEUE_RXEMPTY & msprf24_queue_state())) {
        flush_rx();
    }

    msprf24_activate_rx();

    LCD_init();

    RTC_Config();

    RTC_Initial_Set();

    BME280_Init(&dev);

    /* Enabling the FPU for floating point operation */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    MAP_Interrupt_enableInterrupt(INT_RTC_C);

    //Clear display
    Output_Clear();

    create_data_display();
    print_current_status_pic(currentStatus);

    updateDataDisplay();

    MAP_Interrupt_enableMaster();

    while(1)
    {
        if(reset_time){
           char time[10];
           //Get current time (should be 1 minute later)
           getRTCtime(time);

           //Update displayed time
           updateTimeandDate();

           reset_time=0;
        }

        if ((second_count%10) == 0) {
            //Retrieve sensor values
            res = BME280_Read(&dev, &compensated_data);

            float temp_pressure = ( (float) compensated_data.pressure )/ 100.0;
            //convert
            temp_pressure *= ( 0.760 / 101325.0);
            //scale by 1000 for mmHg. Conversion leaves the value in decimal notation
            inside.pressure = temp_pressure * 1000;

            //Convert value to percentage
            inside.humidity = compensated_data.humidity/1000.0;

            //Convert from degrees C to F
            inside.temperature = (((compensated_data.temperature / 100.0) * (9.0/5.0)) + 32.0);

            //Send new values to the screen
            updateDataDisplay();
        }

        if((second_count%20)==0){
            print_current_status_pic(currentStatus);
        }

        if (rf_irq & RF24_IRQ_FLAGGED) {
            rf_irq &= ~RF24_IRQ_FLAGGED;
            msprf24_get_irq_reason();
        }
        if (rf_irq & RF24_IRQ_RX || msprf24_rx_pending()) {
            r_rx_payload(32, buf);
            msprf24_irq_clear(RF24_IRQ_RX);
            user = buf[0];

            //Decode Packet Data
            /*if (buf[0] == '0')
                P1OUT &= ~BIT0;
            if (buf[0] == '1')
                P1OUT |= BIT0;*/
            float light_status = 0.0;
            sscanf(buf, "%f,%f,%f,%f",&outside.humidity,&outside.pressure,&outside.temperature,&light_status);
            updateDataDisplay();

        } else {
            user = 0xFF;
        }
    }
}

/* RTC ISR */
void RTC_C_IRQHandler(void)
{
    uint32_t status;

    status = MAP_RTC_C_getEnabledInterruptStatus();
    MAP_RTC_C_clearInterruptFlag(status);

    if (status & RTC_C_CLOCK_READ_READY_INTERRUPT)
    {
        second_count++;
    }
    if (status & RTC_C_TIME_EVENT_INTERRUPT)
    {
        /* Interrupts every minute - Set breakpoint here */
        reset_time = 1;
    }
}

void PORT5_ISR(void){
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    //MAP_GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    P5IFG &= ~BIT0; // clear flag

    if(status & GPIO_PIN0)
    {
        //RF_Flag=0x80;
        rf_irq |= RF24_IRQ_FLAGGED;
    }
}
