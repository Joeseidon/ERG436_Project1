/*
 * BOSCH_Sensor.h
 *
 *  Created on: Feb 11, 2018
 *      Author: joe
 */

#ifndef BOSCH_SENSOR_H_
#define BOSCH_SENSOR_H_


int8_t sensorInit(void);
int8_t verifyDeviceAddress(void);
int8_t retrieveCalibratedData(void);
double getTemp(void);
double getHumidity(void);
double getPressure(void);

#endif /* BOSCH_SENSOR_H_ */
