/*
 * motor_fsm.c
 *
 *  Created on: Apr 16, 2026
 *      Author: becca
 */

void Control_Loop_Callback(void)  // e.g. TIM interrupt @1kHz
{
    switch(motorState)
    {
        case MOTOR_STATE_IDLE:
            PWM_SetDuty(0);
            break;

        case MOTOR_STATE_STARTUP:
            // open-loop stepping until halls valid
            Startup_Commutation();
            if (Hall_IsValid())
                motorState = MOTOR_STATE_RUN;
            break;

        case MOTOR_STATE_RUN:
        {
            int32_t speed = Hall_GetSpeed();
            int32_t duty = PID(targetSpeed);
            PWM_SetDuty(duty);
            break;
        }

        case MOTOR_STATE_BRAKE:
            BrakeMotor();
            break;

        case MOTOR_STATE_FAULT:
            DisableAllPhases();
            break;
    }
}
