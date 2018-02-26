/*
 * LCD.c
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

/* DriverLib Includes */
#include "driverlib.h"

#include "LCD.h"
#include "ST7735.h"
#include "delays.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "RTC_Module.h"

/*Text File Includes*/
#include "dark.txt"
#include "overcast.txt"
#include "partlySunny.txt"
#include "sunny.txt"
#include "twilight.txt"

uint16_t grid_color = ST7735_CYAN;
uint16_t menu_text_color = ST7735_YELLOW;
uint16_t highlight_text_color = ST7735_CYAN;

uint8_t LCD_Rotation = 1;


//create array for menu item names
char *menu_names[5] = {
                     {"0.5Hz"},
                     {"1.0Hz"},
                     {"2.0Hz"},
                     {"5.0Hz"},
                     {"LED 0/1"}
};




volatile display_cell inside={
                0,      //x_start
                0,      //x_finish
                0,      //y_start
                0,      //y_finish
                " ",    //display title
                71.5,    //temp
                22.5,    //humidity
                760     //Pressure
};

volatile display_cell outside={
                0,      //x_start
                0,      //x_finish
                0,      //y_start
                0,      //y_finish
                " ",    //display title
                33.5,    //temp
                22.0,    //humidity
                750     //Pressure
};

forecast light_status_items[5]={
    {DARK,dark , 32,32 ,60 ,60 ,0 ,0},
    {TWILIGHT,twilight,32 ,32 ,60 ,60 ,0 ,0},
    {OVERCAST,overcast,32 ,32 ,60 ,60 ,0 ,0},
    {PARTLY_SUNNY,partlySunny,32 ,32 ,60 ,60 ,0 ,0},
    {SUNNY,sunny,32 ,32 ,60 ,60 ,0 ,0}
};

menu_item menu_options[5]={
    {item1,"0.5Hz",8,4,1,0,12000000,4},
    {item2,"1.0Hz",8,5,0,0,12000000,2},
    {item3,"2.0Hz",8,6,0,0,12000000,1},
    {item4,"5.0Hz",8,7,0,0,4800000,1},
    {item5,"LED 0/1",8,8,0,0,0,0}
};

menu_items num_to_menu_item(int x){
    menu_items main_menu;
    switch(x){
    case 0:
        main_menu = item1;
        break;
    case 1:
        main_menu = item2;
        break;
    case 2:
        main_menu = item3;
        break;
    case 3:
        main_menu = item4;
        break;
    case 4:
        main_menu = item5;
        break;
    }
    return main_menu;
}

void LCD_init(void){
    ST7735_InitR(INITR_REDTAB); // initialize LCD controller IC
    ST7735_SetRotation(LCD_Rotation);
}
Light_Status num_to_enum(int x){
    Light_Status current_status;
    switch(x){
    case 0:
        current_status = DARK;
        break;
    case 1:
        current_status = TWILIGHT;
        break;
    case 2:
        current_status = OVERCAST;
        break;
    case 3:
        current_status = PARTLY_SUNNY;
        break;
    case 4:
        current_status = SUNNY;
        break;
    }
    return current_status;
}

//Use this function once ADC & Photocell are implemented
/*
void print_current_status_pic(int photocell_value){
    int i;
    for(i=0; i<5;i++){
        if(photocell_value > light_status_items[i].minR && photocell_value < light_status_items[i].maxR){
            ST7735_DrawBitmap(light_status_items[i].x, light_status_items[i].y, light_status_items[i].image, light_status_items[i].width, light_status_items[i].height);
        }
    }
}*/
void print_current_status_pic(Light_Status current_status){

    int i;
    for(i=0; i<5;i++){
        if(light_status_items[i].light_quality == current_status){
            ST7735_DrawBitmap(light_status_items[i].x, light_status_items[i].y, light_status_items[i].image, light_status_items[i].width, light_status_items[i].height);
        }
    }

    /*// Must be less than or equal to 128 pixels wide by 160 pixels high
    uint16_t picture_width = 64, picture_hight = 80;
    //was 32 and v = 100
    uint16_t horizontal_start = 10, vertical_start = 100;

    //Clear status text

    switch(current_status){
    case DARK:
        ST7735_DrawBitmap(horizontal_start, vertical_start, dark, picture_width, picture_hight);
        TenMsDelay(10);
        //ST7735_SetCursor(30,0);
        //ST7735_OutString("Status: Dark");*/
        /*ST7735_DrawString(0,0,"Status: Dark        ", menu_text_color);
        break;

    case OVERCAST:
        ST7735_DrawBitmap(horizontal_start, vertical_start, overcast, picture_width, picture_hight);
        TenMsDelay(10);
        ST7735_DrawString(0,0,"Status: Overcast    ", menu_text_color);
        break;

    case PARTLY_SUNNY:
        ST7735_DrawBitmap(horizontal_start, vertical_start, partlySunny, picture_width, picture_hight);
        TenMsDelay(10);
        ST7735_DrawString(0,0,"Status: Partly-Sunny", menu_text_color);
        break;

    case SUNNY:
        ST7735_DrawBitmap(horizontal_start, vertical_start, sunny, picture_width, picture_hight);
        TenMsDelay(10);
        ST7735_DrawString(0,0,"Status: Sunny       ", menu_text_color);
        break;

    case TWILIGHT:
        ST7735_DrawBitmap(horizontal_start, vertical_start, twilight, picture_width, picture_hight);
        TenMsDelay(10);
        ST7735_DrawString(0,0,"Status: Twilight    ", menu_text_color);
        break;
    }*/
}

void updateForecast(Light_Status newForecast){
    return;
}

void create_data_display(void){
    //Draw inside display
    ST7735_DrawString2(110,50,"Out",menu_text_color,ST7735_BLACK);

    //Draw outside display
    ST7735_DrawString2(15,50,"In",menu_text_color,ST7735_BLACK);
    ST7735_DrawFastHLine(0,65,160,grid_color);
    ST7735_DrawString2(0,70,"T:",menu_text_color,ST7735_BLACK);
    ST7735_DrawString2(0,90,"H:",menu_text_color,ST7735_BLACK);


    ST7735_DrawString2(10,110,"Bp",menu_text_color,ST7735_BLACK);
    ST7735_DrawFastHLine(0,108,160,grid_color);

    updateTimeandDate();
}

void updateDataDisplay(void){
    char data[12];
    //print temp
    sprintf(data,"%2.1f%cF",inside.temperature,247);
    TenMsDelay(1);
    ST7735_DrawString2(15,70,data,menu_text_color,ST7735_BLACK);

    //print humidity
    sprintf(data,"%2.1f%%",inside.humidity);
    TenMsDelay(1);
    ST7735_DrawString2(15,90,data,menu_text_color,ST7735_BLACK);

    TenMsDelay(1);

    //print temp
    sprintf(data,"%2.1f%cF",outside.temperature,247);
    TenMsDelay(1);
    ST7735_DrawString2(95,70,data,menu_text_color,ST7735_BLACK);

    //print humidity
    sprintf(data,"%2.1f%%",outside.humidity);
    TenMsDelay(1);
    ST7735_DrawString2(95,90,data,menu_text_color,ST7735_BLACK);

    //print bp
    sprintf(data,"%2.1fmmHg",outside.pressure);
    TenMsDelay(1);
    ST7735_DrawString2(40,110,data,menu_text_color,ST7735_BLACK);
}

void updateTimeandDate(void){
    RTC_C_Calendar *time;
    time = getNewTime();
    char temp[5];
    sprintf(temp,"%02d:%02d",time->hours,time->minutes);
    ST7735_DrawString2(0,5,temp,menu_text_color,ST7735_BLACK);
    sprintf(temp,"%02d/%02d",time->month,time->dayOfmonth);
    ST7735_DrawString2(100,5,temp,menu_text_color,ST7735_BLACK);
}


