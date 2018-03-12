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
 * EGR436 Project 1
 *
 * Description: This project can run stand alone or paired with a remote station.
 *  In both cases, this program runs as a weather station. It communicates with
 *  with a BME280 to read temperature, pressure, and humidity. This data will
 *  be display on an LCD screen using an ST7735 control IC. When paired with a
 *  remote station, data is relayed to this program via an nRF24L01+. Each
 *  packet from the remote station is expected to contain temperature, pressure,
 *  humidity, and light level values. The first three values will also be displayed
 *  on the LCD screen. The fourth value, light level, will be used to determine
 *  which bitmap image should be display. The following bitmap images are used
 *  to indicate light level: dark, overcast, partly sunny, sunny, and twilight.
 *  The local BME280 sensor will be interacted with every 30 seconds and the station
 *  will monitor for RF packets every 15 seconds.
 *
 *  In addition to sensor values, this station will also make use of the on-board
 *  RTC to track time and generate periodic interrupts. On start up, the RTC will
 *  be configured using a capacitive touch interface.
 *
 * Author: Joseph Cutino & Lourens Willekes
*******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/*Module Library Includes*/
#include "clockConfig.h"            //MSP432 clock configurations w/ ext. crystal
#include "environment_sensor.h"     //BME280 library

#include "ST7735.h"                 //Direct LCD manipulation
#include "LCD.h"                    //Menu and page data manipulation

#include "RTC_Module.h"             //RTC configuration and data retrieval

//#include "msp432_spi.h"

#include "RF_Module.h"              //RF module and support variables/functions
#include "msprf24.h"

/*
 *Establishes indication lights for sensor operation
 */
void config_transmition_leds(void){
    //RED for RF RX
    //GREEN for BME sensor read
    //BLUE for RTC Update
    P2DIR |= BIT0|BIT1|BIT2;
    P2OUT &= ~(BIT0|BIT1|BIT2);
}

Light_Status currentStatus = DARK;

int current_count;
int target_count=8;
int light_status_updated=0;

int second_count;
int reset_time = 0;

//structures defined in LCD module
extern volatile display_cell inside;
extern volatile display_cell outside;

int res;
int read_sensor = 0;
struct bme280_dev dev;
struct bme280_data compensated_data;

//RF
extern volatile uint8_t rf_irq;
volatile unsigned int user;
int first_rx = 1;
int light_level = 0;
uint8_t buf[32];

int main(void)
{
    /*
     * Configure the MSP432's onboard RGB LED
     * such that the three primary colors can
     * be used to indicate high priority method
     * execution.
     */
    config_transmition_leds();

    /*
     * Configures P5.0 as an interrupt pin
     * for the RF module. The trigger is
     * set on the falling edge.
     */
    RF_IRQ_Init();

    /*
     * Initialization of local variable used
     * to confirm the RF module is functioning
     * normally.
     */
    user = 0xFE;

    /*
     * Starts up the external oscillator and
     * configures MCLk to be sourced for the
     * external oscillator with a frequency of
     * 48MHz. SMCLK is then sourced from MCLK
     * and divided down to 12MHz.
     */
    clockStartUp();

    /*
     * The RF module is setup to be primary reciever
     * with auto retransmit acknowledgment packets.
     */
    RF_Module_Init_RX();

    /*
     * Initializes the LCD module and set the correct
     * screen rotation.
     */
    LCD_init();

    /*
     * Configures pins on port J for RTC use. The low
     * frequency external oscillator is then started.
     * An interrupt is then setup of every minute. This
     * method also initializes the capcative touch interface
     * used to receive user input.
     */
    RTC_Config();

    /*
     * Using the capactiie touch interface, this method
     * receives the current time and date fomr he user
     * so that he on-board RTC can be initialized with
     * the correct values.
     */
    //RTC_Initial_Set();

    //used while debugging. Avoid Cap touch time input
    debugTimeSet();

    /*
     * This method established the settings for the
     * BME280 BOSCH sensor. Within this method, the
     * I2C and timer32 modules are initialized for
     * use in the method.
     */
    BME280_Init(&dev);

    /*
     * Enabling the FPU for floating point operation
     */
    MAP_FPU_enableModule();
    MAP_FPU_enableLazyStacking();

    /*
     * RTC interrupts are mapped to the local RTC
     * interrupt service routine.
     */
    MAP_Interrupt_enableInterrupt(INT_RTC_C);

    //Clear display
    Output_Clear();

    /*
     * Generates a data display for the LCD module.
     */
    create_data_display();

    /*
     * Prints a status picture, that corresponds to the
     * current status, on the LCD module.
     */
    print_current_status_pic(currentStatus);

    /*
     * Fills in the data display with updated values.
     */
    updateDataDisplay();

    /*
     * Enables all interrupts.
     */
    MAP_Interrupt_enableMaster();

    while(1)
    {
        if(reset_time){
            P2OUT |= BIT2; //Blue for RTC update
            char time[10];
            //Get current time (should be 1 minute later)
            getRTCtime(time);

            //Update displayed time
            updateTimeandDate();

            //Clear flag
            reset_time=0;

            //Turn off the BLUE LED
            P2OUT &= ~BIT2;
        }

        if ((second_count%30) == 0) {
            P2OUT |= BIT1; //Green For BME sensor

            //Retrieve sensor values from BME280
            res = BME280_Read(&dev, &compensated_data);

            //Format pressure measurement
            float temp_pressure = ( (float) compensated_data.pressure )/ 100.0;

            //convert from kPa to mmHg
            temp_pressure *= ( 0.760 / 101325.0);

            //scale by 1000 for mmHg. Conversion leaves the value in decimal notation
            //Save current formated measurement
            inside.pressure = temp_pressure * 1000;

            //Save formated humidity measurement
            inside.humidity = compensated_data.humidity/1000.0; //Convert value to percentage

            //Convert from degrees C to F and save
            inside.temperature = (((compensated_data.temperature / 100.0) * (9.0/5.0)) + 32.0);

            //This new data is compared with past trends.
            update_totals(1,0);

            //Send new values to the screen
            updateDataDisplay();

            //Turn off Green status LED
            P2OUT &= ~BIT1;
        }

        if((second_count%10)==0){
            //Check for a IRQ flag from the RF Module
            if (rf_irq & RF24_IRQ_FLAGGED) {
                //Clear flag
                rf_irq &= ~RF24_IRQ_FLAGGED;

                //Determine interrupt source
                msprf24_get_irq_reason();
            }
            if (rf_irq & RF24_IRQ_RX || msprf24_rx_pending()) {
                P2OUT |= BIT0; //Red LED for RF transmition

                //Configure RF packet payload size
                r_rx_payload(32, buf);

                //Clear RX status
                msprf24_irq_clear(RF24_IRQ_RX);

                //Set local variable for normal operation validation
                user = buf[0];

                //Format packet data for use by the LCD module
                sscanf(buf, "%f,%f,%f,%d",&outside.humidity,&outside.pressure,&outside.temperature,&light_level);

                //Update the forecast picture using new data
                updateForecast(light_level);

                //Update outside data trends
                update_totals(0,1);

                //Send data to the LCD module for display
                updateDataDisplay();

                //Turn off RED LED
                P2OUT &= ~BIT0;

            } else {
                //This value is used to indicate RX data is not currently available
                user = 0xFF;
            }
        }
    }
}

// This is the interrupt service routine for RTC interrupts.
// Inputs: NONE
// Outputs: NONE
// Assumes: NONE
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

// This is t port 5 interrupt service routine. It serves as
// and indication to the RF module software the an event has
// occurred within the RF IC.
// Inputs: NONE
// Outputs: NONE
// Assumes: Manually added to interrupt vector table
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
