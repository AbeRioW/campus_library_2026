/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
/* USER CODE END 0 */

TIM_HandleTypeDef htim4;

/* TIM4 init function */
void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();

    /* TIM4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
#include "ds1302.h"
#include "oled.h"
#include "esp8266.h"
#include "gpio.h"

volatile uint8_t time_update_flag = 0;
DS1302_Time current_display_time;
void show_time(DS1302_Time time);
void show_time_from_main(void)
{
	if (time_update_flag)
	{
		time_update_flag = 0;
		show_time(current_display_time);
		OLED_Refresh();
	}
}

void show_time(DS1302_Time time)
{
	char year_str[8], month_str[4], day_str[4], hour_str[4], minute_str[4], sec_str[4];
	
	sprintf(year_str, "%04d-", (2000 + time.year));
	sprintf(month_str, "%02d-", time.month);
	sprintf(day_str, "%02d", time.date);
	sprintf(hour_str, "%02d:", time.hour);
	sprintf(minute_str, "%02d:", time.minute);
	sprintf(sec_str, "%02d", time.second);
	
	OLED_ShowString(0, 0, (uint8_t*)year_str, 8, time_set_count == 1 ? 0 : 1);
	OLED_ShowString(30, 0, (uint8_t*)month_str, 8, time_set_count == 2 ? 0 : 1);
	OLED_ShowString(46, 0, (uint8_t*)day_str, 8, time_set_count == 3 ? 0 : 1);
	OLED_ShowString(64, 0, (uint8_t*)hour_str, 8, time_set_count == 4 ? 0 : 1);
	OLED_ShowString(80, 0, (uint8_t*)minute_str, 8, time_set_count == 5 ? 0 : 1);
	OLED_ShowString(96, 0, (uint8_t*)sec_str, 8, time_set_count == 6 ? 0 : 1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM4)
	{
		if(time_set_count == 0)
		{
			DS1302_GetTime(&current_display_time);
		}
		else
		{
			current_display_time = set_time;
		}
		time_update_flag = 1;
	}
}

/* USER CODE END 1 */
