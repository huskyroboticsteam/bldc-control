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
int phases(void)
{

	/*
	0, // invalid (000)
    1, // 001
    2, // 010
    3, // 011
    4, // 100
    5, // 101
    6, // 110
    7  // invalid (111)
	 */


  int i = 0;
  int32_t CH1_DC = 0;

  //don't delete ---------------------------------------------------------------------
  /*
  HAL_GPIO_WritePin(GPIOA, A_Low_Pin, GPIO_PIN_SET); // CN10 - 11 (6 down on right side of left)
  HAL_GPIO_WritePin(GPIOA, B_Low_Pin, GPIO_PIN_SET); // CN10 - 15
  HAL_GPIO_WritePin(GPIOB, C_Low_Pin, GPIO_PIN_SET); // CN10 - 26
  HAL_GPIO_WritePin(GPIOB, A_High_Pin, GPIO_PIN_SET);// CN10 - 30
  HAL_GPIO_WritePin(GPIOA, B_High_Pin, GPIO_PIN_SET); // CN10 - 13
  HAL_GPIO_WritePin(GPIOB, C_High_Pin, GPIO_PIN_SET); //CN10 - 28
  */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);


  while (1)
  {

	  //reset all


      switch(Hall_GetStep()){
      	  case 1:

      		break;

      	  case 2:

      		break;

      	  case 3:

      		break;
      	  case 4:

			break;

      	  case 5:

			break;

      	  case 6:

			break;
      	  case 7:

			break;
        }
	  //coil states
      HAL_Delay(100);

      //printf("The number is: %d\n", i);
      i++;

      if(i >= 6){
    	  i = 0;
      }

  }

}
