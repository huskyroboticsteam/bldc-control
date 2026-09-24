/*
 * hallSensor.h
 *
 *  Created on: May 13, 2026
 *      Author: shahnazmohideen
 */

#ifndef INC_HALLSENSOR_H_
#define INC_HALLSENSOR_H_

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

#endif /* INC_HALLSENSOR_H_ */
