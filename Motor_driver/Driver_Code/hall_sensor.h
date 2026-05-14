/*
 * hall_sensor.h
 *
 *  Created on: May 13, 2026
 *      Author: shahnazmohideen
 */

#ifndef HALL_SENSOR_H_
#define HALL_SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g4xx_hal.h"

#define HALL_TIMEOUT_MS 100

uint8_t Hall_ReadRawState(void);
uint8_t Hall_GetStep(void);
uint8_t Hall_IsFault(void);
void Hall_EdgeCallback(void);
int32_t Hall_GetSpeed(void);

#ifdef __cplusplus
}
#endif



#endif /* HALL_SENSOR_H_ */
