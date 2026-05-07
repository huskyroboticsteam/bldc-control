/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "phases.h"

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
void phases_start()
{

	//this si different for like every motor so will need to figure out phase order... we assumed the following
	/*
	0, // invalid (000)
    1, // 001
    2, // 101
    3, // 100
    4, // 110
    5, // 010
    6, // 011
    7  // invalid (111)
	 */

  //don't delete ---------------------------------------------------------------------
  /*
  HAL_GPIO_WritePin(GPIOA, A_Low_Pin, GPIO_PIN_SET); // CN10 - 11 (6 down on right side of left)
  HAL_GPIO_WritePin(GPIOA, B_Low_Pin, GPIO_PIN_SET); // CN10 - 15
  HAL_GPIO_WritePin(GPIOB, C_Low_Pin, GPIO_PIN_SET); // CN10 - 26
  HAL_GPIO_WritePin(GPIOB, A_High_Pin, GPIO_PIN_SET);// CN10 - 30
  HAL_GPIO_WritePin(GPIOA, B_High_Pin, GPIO_PIN_SET); // CN10 - 13
  HAL_GPIO_WritePin(GPIOB, C_High_Pin, GPIO_PIN_SET); //CN10 - 28
  */

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

    TIM_BreakDeadTimeConfigTypeDef sBDT = {0};

    //maybe need to change to sBreakDeadTimeConfig (seen used in older stm cube tutorials?)
    // https://deepbluembedded.com/stm32-complementary-pwm-dead-time-code-examples/
    sBDT.DeadTime = DEADTIME_TICKS;
    sBDT.OffStateRunMode = TIM_OSSR_ENABLE;
    sBDT.OffStateIDLEMode = TIM_OSSI_ENABLE;
    sBDT.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;

    HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBDT);

}

void Phases_Hall(uint8_t step){

      switch(step){
      	  case 0: // 000 (invalid)
      		  break;
      		  
      	  case 1: // 001
      		AH_CL(); //1
      		break;

      	  case 2: // 101
      		BH_CL(); //2
			break;

      	  case 3: // 100
      		BH_AL(); //3
			break;

      	  case 4: // 110
      		CH_AL(); //4
			break;

      	  case 5: // 010
      		CH_BL(); //5
      		break;

      	  case 6: // 011
      		AH_BL(); //6
      		break;
      		
      	  case 7: // 111 (invalid)
			break;
        }

}


//taken from https://electronics.stackexchange.com/questions/625709/
	//commutation-bldc-motor-with-complementary-pwm-produces-slower-speeds-than-indepe

//we used the STM pwm since we need deadtime and an inverse pwm signal to ensure no shoot through in the half bridge
//		when upper and lower mosfets are open at the same time.
//		This send an inverse PWM signal to the lower mosfet (so if we had duty cycle of 70%
//		it would send 30% to lower mosfet). This needs to be configured in STM IDE, example here:


void AH_BL() {
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3); //Disable PWM CH 3.
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

    TIM1->CCR1 = dutyCycle; //Set DutyCycle Pin AH.
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
}

void AH_CL() {
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2); //Disable PWM CH 2.
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

    TIM1->CCR3 = 0; //Set Pin CL to High.
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}

void BH_CL() {
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1); //Disable PWM CH 1.
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    TIM1->CCR2 = dutyCycle; //Set DutyCycle Pin BH.
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}

void BH_AL() {
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_3); //Disable PWM CH 3.
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_3);

    TIM1->CCR1 = 0; //Set Pin AL to High.
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
}

void CH_AL() {
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2); //Disable PWM CH 2.
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);

    TIM1->CCR3 = dutyCycle; //Set DutyCycle Pin CH.
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
}

void CH_BL() {
    HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1); //Disable PWM CH 1.
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    TIM1->CCR2 = 0; //Set Pin BL to High.
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
}


