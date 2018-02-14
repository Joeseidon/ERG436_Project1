/*
 * BOSCH_Sensor.c
 *
 *  Created on: Feb 11, 2018
 *      Author: joe
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

#include "bme280.h"
#include "BOSCH_Sensor.h"

#include "I2C_interface.h"

#include "sysTick.h"

struct bme280_dev dev;
int8_t rslt,com_rslt;
uint8_t v_data_u8;
struct bme280_data device_data;
struct bme280_dev *p_bme280 = &dev;

int8_t sensorInit(void){
    int8_t rslt = BME280_OK;
    dev.dev_id = BME280_I2C_ADDR_PRIM;
    dev.intf = BME280_I2C_INTF;
    dev.read = (bme280_com_fptr_t)I2C_WRITE_READ_STRING;
    dev.write = (bme280_com_fptr_t)I2C_WRITE_STRING;
    dev.delay_ms = (bme280_com_fptr_t)SysTick_delay;
    //set default values
    device_data.humidity=0.0;
    device_data.pressure=0.0;
    device_data.temperature=0.0;

    rslt = bme280_init(&dev);
    dev.delay_ms(10);

    uint8_t settings_sel;

    /* Recommended mode of operation: Indoor navigation */
    dev.settings.osr_h = BME280_OVERSAMPLING_1X;
    dev.settings.osr_p = BME280_OVERSAMPLING_16X;
    dev.settings.osr_t = BME280_OVERSAMPLING_2X;
    dev.settings.filter = BME280_FILTER_COEFF_16;

    settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;


    /*settings_sel = BME280_OSR_PRESS_SEL;
    settings_sel |= BME280_OSR_TEMP_SEL;
    settings_sel |= BME280_OSR_HUM_SEL;
    settings_sel |= BME280_STANDBY_SEL;
    settings_sel |= BME280_FILTER_SEL;*/

    dev.delay_ms(10);
    rslt = bme280_set_sensor_settings(settings_sel, &dev);
    dev.delay_ms(10);

    return rslt;
}

int8_t verifyDeviceAddress(void){
    //struct bme280_dev *p_bme280 = &dev;
    com_rslt = p_bme280->read(p_bme280->dev_id,BME280_CHIP_ID_ADDR, &v_data_u8,8);
    if(v_data_u8 == 0x60){
        return 1; //correct address
    }else
        return 0;
}

int8_t retrieveCalibratedData(void){
    int8_t rtn;
    /*rtn = bme280_set_sensor_mode(BME280_FORCED_MODE, p_bme280);
    dev.delay_ms(40);
    rtn = bme280_get_sensor_data(BME280_ALL, &device_data, p_bme280);
    */
    rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
    dev.delay_ms(70);
    rslt = bme280_get_sensor_data(BME280_ALL, &device_data, &dev);
    return rtn;
}

double getTemp(void){
    return device_data.temperature;
}

double getHumidity(void){
    return device_data.humidity;
}

double getPressure(void){
    return device_data.pressure;
}
