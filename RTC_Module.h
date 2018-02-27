/*
 * RTC_Setup.h
 *
 *  Created on: Feb 20, 2018
 *      Author: joe
 */

#ifndef RTC_MODULE_H_
#define RTC_MODULE_H_

//Used to track user input
typedef enum time_set{
    HOURS = 0,
    MINUTES,
    DAY,
    MONTH,
    YEAR
}time_set_stage;

//Used to track user input for RTC
typedef struct time_step{
    time_set_stage stage;
    char *prompt;
    int increment_mod;
}time_step;

void RTC_Config(void);

void RTC_Initial_Set(void);

void debugTimeSet(void);

void getRTCtime(char *data);

RTC_C_Calendar *getNewTime(void);

#endif /* RTC_MODULE_H_ */
