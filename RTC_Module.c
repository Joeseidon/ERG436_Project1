/*
 * RTC_Module.c
 *
 *  Created on: Feb 20, 2018
 *      Author: joe
 */


#include "driverlib.h"
#include "RTC_Module.h"
#include "msp.h"
#include "structure.h"
#include "CTS_Layer.h"
#include "CTS_HAL.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ST7735.h"


time_set_stage current_Stage = HOURS;

time_step setup_steps[5] = {
    {HOURS,  "Hours:    ",13},
    {MINUTES,"Minutes:  ",60},
    {DAY,    "Day:      ",32},
    {MONTH,  "Month:    ",13},
    {YEAR,   "Year: 20XX",100}
};

RTC_C_Calendar currentTime =
{
     1,
     3,
     10,
     12,
     11,
     01,
     2018
};

RTC_C_Calendar newtime;

int SET_TIME = false;
uint16_t value = 0;
//int reset_time = 0;
int print_count = 0;
//int second_count = 0;

uint16_t raw_count[3];
uint16_t delta_count[3];

void RTC_Config(void){
    /* Config RTC */
    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,
            GPIO_PIN0 | GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Starting LFXT in non-bypass mode without a timeout. */
    CS_startLFXT(false);

    /* Specify an interrupt to assert every minute */
    MAP_RTC_C_setCalendarEvent(RTC_C_CALENDAREVENT_MINUTECHANGE);

    /* Enable interrupt for RTC Ready Status, which asserts when the RTC
     * Calendar registers are ready to read.
     * Also, enable interrupts for the Calendar alarm and Calendar event. */
    MAP_RTC_C_clearInterruptFlag(
            RTC_C_CLOCK_READ_READY_INTERRUPT | RTC_C_TIME_EVENT_INTERRUPT);
    MAP_RTC_C_enableInterrupt(
            RTC_C_CLOCK_READ_READY_INTERRUPT | RTC_C_TIME_EVENT_INTERRUPT);

    //Initialize baseline measurement
    TI_CAPT_Init_Baseline(&my_keys);

    //update baseline measurement (average 30 measurements)
    TI_CAPT_Update_Baseline(&my_keys, 30);
}

void RTC_Initial_Set(void){
    //Record element delta and raw counts
    TI_CAPT_Custom(&my_keys, delta_count);
    TI_CAPT_Raw(&my_keys, raw_count);

    ST7735_DrawString2(30,35,"Clock Init:",ST7735_YELLOW,ST7735_BLACK);
    char str_val[2];
    while(!SET_TIME){
        //Inquire if a button has been pressed
        const struct Element *tmp = TI_CAPT_Buttons(&my_keys);

        if(print_count == 0){

            ST7735_DrawString2(30,65,setup_steps[current_Stage].prompt,ST7735_YELLOW,ST7735_BLACK);

            print_count++;
        }

        sprintf(str_val,"%02.0d",value);
        ST7735_DrawString2(50,80,str_val,ST7735_YELLOW,ST7735_BLACK);

        //Determine if a button has been pressed
        if(&select_element == tmp)
        {
            print_count = 0;

            uint16_t temp = value;
            //store setting in RTC struct
            switch(current_Stage){
                case HOURS:
                    currentTime.hours = temp;
                    current_Stage = MINUTES;
                    break;

                case MINUTES:
                    currentTime.minutes = temp;
                    current_Stage = DAY;
                    break;

                case DAY:
                    currentTime.dayOfmonth = temp;
                    current_Stage = MONTH;
                    break;

                case MONTH:
                    currentTime.month = temp;
                    current_Stage = YEAR;
                    currentTime.year = 2018;
                    SET_TIME=1;//break loop
                    break;
                case YEAR:
                    currentTime.year = (2000+temp);
                    current_Stage = MINUTES;
                    SET_TIME=1;//break loop
                    break;

            }

            value = 0;


            //Wait for Button release
            while(TI_CAPT_Buttons(&my_keys) == &select_element){
                    ;
            }
        }else if(&down_element == tmp){

            if(value==0){
                //if positive, decrement is allowed
                value = setup_steps[current_Stage].increment_mod - 1;
            }else{
                value--;
            }

            //Wait for Button Release
            while(TI_CAPT_Buttons(&my_keys) == &down_element){
                ;
            }

        }else if(&up_element == tmp){
            value = (value + 1) % setup_steps[current_Stage].increment_mod;

            //Wait for Button Release
            while(TI_CAPT_Buttons(&my_keys) == &up_element){
                ;
            }
        }else{
            ;
        }
    }

    MAP_RTC_C_initCalendar(&currentTime, RTC_C_FORMAT_BINARY);

    /* Start RTC Clock */
    MAP_RTC_C_startClock();
    return;
}
void debugTimeSet(void){
    MAP_RTC_C_initCalendar(&currentTime, RTC_C_FORMAT_BINARY);

    /* Start RTC Clock */
    MAP_RTC_C_startClock();
}
void getRTCtime(char *data){
    newtime = MAP_RTC_C_getCalendarTime();
    sprintf(data,"%02.0d:%02.0d",newtime.hours,newtime.minutes);
    return;
}

RTC_C_Calendar *getNewTime(void){
    newtime = MAP_RTC_C_getCalendarTime();

    return &newtime;
}
