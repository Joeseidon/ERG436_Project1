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

void print_current_status_pic(Light_Status current_status){
    // Must be less than or equal to 128 pixels wide by 160 pixels high
    uint16_t picture_width = 64, picture_hight = 80;
    //was 32 and v = 100
    uint16_t horizontal_start = 10, vertical_start = 100;

    //Clear status text

    switch(current_status){
    case DARK:
        ST7735_DrawBitmap(horizontal_start, vertical_start, dark, picture_width, picture_hight);
        TenMsDelay(10);
        /*ST7735_SetCursor(30,0);
        ST7735_OutString("Status: Dark");*/
        ST7735_DrawString(0,0,"Status: Dark        ", menu_text_color);
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
    }
}

void updateForecast(Light_Status newForecast){
    return;
}

void create_data_display(void){
    /*ST7735_DrawString(15,3,"Menu:",menu_text_color);
    ST7735_DrawFastHLine(80,38,50,grid_color);
    ST7735_DrawFastHLine(0,10,128,grid_color);
    TenMsDelay(1);
    // Input: x         columns from the left edge (0 to 20)
    //        y         rows from the top edge (0 to 15)
    ST7735_DrawString(0,11,"  Inside     Outside",menu_text_color);
    TenMsDelay(1);
    ST7735_DrawFastHLine(0,120,128,grid_color);
    TenMsDelay(1);
    ST7735_DrawFastVLine(64,120,50,grid_color);

    //Home stats
    ST7735_DrawString(0,13,"TP:",menu_text_color);
    ST7735_DrawString(0,14,"RH:",menu_text_color);

    //Outside stats
    ST7735_DrawString(11,13,"TP:",menu_text_color);
    ST7735_DrawString(11,14,"RH:",menu_text_color);
    ST7735_DrawString(11,15,"BP:",menu_text_color);
    */
    display_cell inside={
                    0,      //x_start
                    0,      //x_finish
                    0,      //y_start
                    0,      //y_finish
                    " ",    //display title
                    0.0,    //temp
                    0.0,    //humidity
                    0.0     //Pressure
    };

    display_cell outside={
                    0,      //x_start
                    0,      //x_finish
                    0,      //y_start
                    0,      //y_finish
                    " ",    //display title
                    0.0,    //temp
                    0.0,    //humidity
                    0.0     //Pressure
    };

    //Draw inside display
    ST7735_DrawString2(110,50,"Out",menu_text_color,ST7735_BLACK);
    ST7735_DrawFastHLine(100,65,50,grid_color);
    ST7735_DrawString2(95,70,"T:",menu_text_color,ST7735_BLACK);
    ST7735_DrawString2(95,90,"H:",menu_text_color,ST7735_BLACK);

    //Draw outside display
    ST7735_DrawString2(10,50,"In",menu_text_color,ST7735_BLACK);
    ST7735_DrawFastHLine(0,65,50,grid_color);
    ST7735_DrawString2(5,70,"T:",menu_text_color,ST7735_BLACK);
    ST7735_DrawString2(5,90,"H:",menu_text_color,ST7735_BLACK);


    ST7735_DrawString2(35,110,"Bp:",menu_text_color,ST7735_BLACK);
    ST7735_DrawFastHLine(0,105,160,grid_color);

    //draw forecast display
    ST7735_DrawBitmap(50, 50, overcast, 55, 36);
    TenMsDelay(10);

}


