/*
 * LCD.h
 *
 *  Created on: Feb 10, 2018
 *      Author: joe
 */

#ifndef LCD_H_
#define LCD_H_

typedef enum
{
    item1 = 0,
    item2 = 1,
    item3 = 2,
    item4 = 3,
    item5 = 4
}menu_items;

typedef enum
{
    DARK = 0,
    TWILIGHT = 1,
    OVERCAST = 2,
    PARTLY_SUNNY = 3,
    SUNNY = 4

}Light_Status;

//typedef struct menu_item menu_item;
typedef struct menu_item{
    /*Menu Item Number*/
    menu_items item_num;   //used to match with enum(menu_items) for selection and click

    /*Option Name*/
    char *name;

    /*LCD Placement*/
    int x_pos;
    int y_pos;

    /*Flagged when cursor is over this item*/
    int selected;

    /*Flagged when the user clicks this item*/
    int clicked;

    /*Counter value required to reach desired frequency*/
    int toggle_period;

    /*Clk Count: used for longer delays*/
    int toggle_count;
}menu_item;

menu_items num_to_menu_item(int x);
Light_Status num_to_enum(int x);
void print_current_status_pic(Light_Status current_status);
void create_data_display(void);

#endif /* LCD_H_ */
