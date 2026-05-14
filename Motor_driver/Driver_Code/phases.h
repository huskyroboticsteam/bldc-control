/*
 * phases.h
 *
 *  Created on: Feb 21, 2026
 *      Author: becca
 */

#ifndef PHASES_H_
#define PHASES_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void phases_start(void);
void Set_Duty_Cycle(uint32_t new_value);
void Phases_Hall(uint8_t step);

void AH_BL();
void AH_CL();
void BH_CL();
void BH_AL();
void CH_AL();
void CH_BL();

#ifdef __cplusplus
}
#endif

#endif /* PHASES_H_ */
