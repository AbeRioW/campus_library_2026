/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "stdbool.h"
#include "usart.h"
#include "ds1302.h"
#include "flash.h"
#include "rc522.h"
#include "oled.h"
int time_set_count = 0;
bool time_up = false;
bool time_down = false;

bool nfc_register = false;
bool nfc_delete = false;

DS1302_Time set_time;
uint8_t nfc_cardid[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t g_ucTempbuf[20];
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, OLED_SCL_Pin|OLED_SDA_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED2_Pin|LED3_Pin|SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI1_RST_Pin|ULN2003_IN4_Pin|ULN2003_IN3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DS1302_CLK_GPIO_Port, DS1302_CLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DS1302_DAT_Pin|DS1302_RST_Pin|ULN2003_IN1_Pin|ULN2003_IN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : OLED_SCL_Pin OLED_SDA_Pin LED1_Pin */
  GPIO_InitStruct.Pin = OLED_SCL_Pin|OLED_SDA_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LED2_Pin LED3_Pin SPI1_CS_Pin */
  GPIO_InitStruct.Pin = LED2_Pin|LED3_Pin|SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_RST_Pin ULN2003_IN4_Pin ULN2003_IN3_Pin ULN2003_IN1_Pin
                           ULN2003_IN2_Pin */
  GPIO_InitStruct.Pin = SPI1_RST_Pin|ULN2003_IN4_Pin|ULN2003_IN3_Pin|ULN2003_IN1_Pin
                          |ULN2003_IN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : NFC_REGISTER_Pin NFC_DELETE_Pin TIME_SET_Pin TIME_UP_Pin */
  GPIO_InitStruct.Pin = NFC_REGISTER_Pin|NFC_DELETE_Pin|TIME_SET_Pin|TIME_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TIME_DOWN_Pin */
  GPIO_InitStruct.Pin = TIME_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(TIME_DOWN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DS1302_CLK_Pin */
  GPIO_InitStruct.Pin = DS1302_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DS1302_CLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : HC_SR505_1_Pin HC_SR505_2_Pin HC_SR505_3_Pin */
  GPIO_InitStruct.Pin = HC_SR505_1_Pin|HC_SR505_2_Pin|HC_SR505_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DS1302_DAT_Pin DS1302_RST_Pin */
  GPIO_InitStruct.Pin = DS1302_DAT_Pin|DS1302_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */
static uint32_t last_key_time = 0;
#define DEBOUNCE_TIME 200

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t current_time = HAL_GetTick();
	
	if((current_time - last_key_time) < DEBOUNCE_TIME)
	{
		return;
	}
	last_key_time = current_time;
	
	if(GPIO_Pin == TIME_SET_Pin)
	{
		if(time_set_count == 0)
		{
			DS1302_GetTime(&set_time);
		}
		time_set_count++;
		if(time_set_count > 6)
		{
			time_set_count = 0;
			set_time.day = 1;
			DS1302_SetTime(&set_time);
		}
	}
	else if(GPIO_Pin == TIME_UP_Pin)
	{
		if(time_set_count == 1)
		{
			set_time.year++;
			if(set_time.year > 99) set_time.year = 20;
		}
		else if(time_set_count == 2)
		{
			set_time.month++;
			if(set_time.month > 12) set_time.month = 1;
		}
		else if(time_set_count == 3)
		{
			set_time.date++;
			if(set_time.date > 31) set_time.date = 1;
		}
		else if(time_set_count == 4)
		{
			set_time.hour++;
			if(set_time.hour > 23) set_time.hour = 0;
		}
		else if(time_set_count == 5)
		{
			set_time.minute++;
			if(set_time.minute > 59) set_time.minute = 0;
		}
		else if(time_set_count == 6)
		{
			set_time.second++;
			if(set_time.second > 59) set_time.second = 0;
		}
		time_up = true;
	}
	else if(GPIO_Pin == TIME_DOWN_Pin)
	{
		if(time_set_count == 1)
		{
			set_time.year--;
			if(set_time.year < 20) set_time.year = 99;
		}
		else if(time_set_count == 2)
		{
			set_time.month--;
			if(set_time.month < 1) set_time.month = 12;
		}
		else if(time_set_count == 3)
		{
			set_time.date--;
			if(set_time.date < 1) set_time.date = 31;
		}
		else if(time_set_count == 4)
		{
			if(set_time.hour == 0) set_time.hour = 23;
			else set_time.hour--;
		}
		else if(time_set_count == 5)
		{
			if(set_time.minute == 0) set_time.minute = 59;
			else set_time.minute--;
		}
		else if(time_set_count == 6)
		{
			if(set_time.second == 0) set_time.second = 59;
			else set_time.second--;
		}
		time_down = true;
	}
	else if(GPIO_Pin == NFC_REGISTER_Pin)
	{
		nfc_register = true;
	}
	else if(GPIO_Pin == NFC_DELETE_Pin)
	{
		nfc_delete = true;
		HAL_UART_Transmit(&huart2, (uint8_t *)"KEY2", 4, 100);
	}
}
/* USER CODE END 2 */
