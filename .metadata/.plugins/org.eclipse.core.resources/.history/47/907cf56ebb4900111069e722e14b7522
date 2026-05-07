#include "hall_sensor.h" //make later
#include "stm32g4xx_hal.h"

#define HALL_TIMEOUT_MS 100

static const uint8_t hallToStep[8] =
{
    0, // invalid (000)
    1, // 001
    2, // 010
    3, // 011
    4, // 100
    5, // 101
    6, // 110
    7  // invalid (111)
};


static volatile uint32_t lastEdgeTime = 0;
static volatile uint32_t edgePeriod    = 0;
static volatile uint8_t  lastStep      = 0;
static volatile int8_t   direction     = 0;

uint8_t Hall_ReadRawState(void)
{
    uint8_t a = HAL_GPIO_ReadPin(HALL_A_PORT, HALL_A_PIN);
    uint8_t b = HAL_GPIO_ReadPin(HALL_B_PORT, HALL_B_PIN);
    uint8_t c = HAL_GPIO_ReadPin(HALL_C_PORT, HALL_C_PIN);

    return (c << 2) | (b << 1) | a; //map output to states 'xxxxxcba'
}

uint8_t Hall_GetStep(void)
{
    return hallToStep[Hall_ReadRawState()];
}

//check if hall state is invalid state
uint8_t Hall_IsFault(void)
{
    uint8_t s = Hall_ReadRawState();
    return (s == 0 || s == 7);
}

//updates motor speed and direction every time the Hall sensors detect rotor movement
void Hall_EdgeCallback(void)
{
    uint32_t now  = HAL_GetTick();
    uint8_t step  = Hall_GetStep();

    if (lastEdgeTime != 0){

        uint32_t delta = now - lastEdgeTime;
        if (delta > 0 && delta < HALL_TIMEOUT_MS)
            edgePeriod = delta;
    }

    lastEdgeTime = now;

    //detect direction ---------
    if (lastStep != 0 && step != 0){

        int diff = (int)step - (int)lastStep;

        if (diff == 1 || diff == -5)
            direction = +1;
        else if (diff == -1 || diff == 5)
            direction = -1;
    }

    lastStep = step;
}

//
int32_t Hall_GetSpeed(void)
{
    if ((HAL_GetTick() - lastEdgeTime) > HALL_TIMEOUT_MS) //check if more than timeout has passed since hall pulse
        return 0;

    if (edgePeriod == 0)
        return 0;

    int32_t full_rev_time = edgePeriod * (uint32_t)HALL_EDGES_PER_REV;
    int32_t rpm = (int32_t)(60000 / (full_rev_time)); //calc rev per minute (60000 ms)

    return (direction >= 0) ? rpm : -rpm; //check direction
}

