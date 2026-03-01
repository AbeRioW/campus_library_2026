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


void show_time(DS1302_Time time)
{
		  char year_str[5],month_str[2],day_str[2],hour_str[2],minute_str[2],sec_str[2];
	          // 读取DS1302时间
        
        //格式化时间字符串
			  sprintf(year_str,"%04d-",(2000+time.year));
			  sprintf(month_str,"%02d-",time.month);
			  sprintf(day_str,"%02d",time.date);
			  sprintf(hour_str,"%2d:",time.hour);		;
			  sprintf(minute_str,"%02d:",time.minute);
			  sprintf(sec_str,"%02d",time.second);	
	      OLED_ShowString(0, 0, (uint8_t*)year_str, 8, time_set_count==1?0:1);		
			  OLED_ShowString(30, 0, (uint8_t*)month_str, 8, time_set_count==2?0:1);
			  OLED_ShowString(46, 0, (uint8_t*)day_str, 8, time_set_count==3?0:1);
			  OLED_ShowString(64, 0, (uint8_t*)hour_str, 8, time_set_count==4?0:1);
			  OLED_ShowString(80, 0, (uint8_t*)minute_str, 8, time_set_count==5?0:1);
			  OLED_ShowString(96, 0, (uint8_t*)sec_str, 8, time_set_count==6?0:1);
			  OLED_Refresh();
}

// 定时器4中断处理函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    DS1302_Time time;
    if (htim->Instance == TIM4)
    {  
//			     if(time_set_count!=0)
//					 {
						  DS1302_GetTime(&time);
			        show_time(time);
//					 }

//        // 每10次发送一次temp数据
//        if (timer_count >= 10)
//        {
//            timer_count = 0;
//            
//					char payload[256];
//				snprintf(payload, sizeof(payload),
//             "{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{\\\"temp\\\":{\\\"value\\\":31.1}}}");
//    
//    ESP8266_MQTT_Publish(MQTT_TOPIC_POST, payload, 0, 0);
//            
//            // 简单的完成提示
//            HAL_UART_Transmit(&huart2, (uint8_t*)"--PUBLISH DONE\r\n", 15, 100);
//            
//            // 重置计数器，确保循环继续
//            timer_count = 0;
//        }
//        
        /* 注释掉发布时间的代码
        // 计算UTC时间戳（毫秒）
        // 注意：这里简化处理，假设DS1302的时间已经是UTC时间
        // 实际应用中可能需要根据时区进行调整
        struct tm tm_time;
        tm_time.tm_year = time.year + 100; // 2000 + year - 1900 = year + 100
        tm_time.tm_mon = time.month - 1; // 月份从0开始
        tm_time.tm_mday = time.date;
        tm_time.tm_hour = time.hour;
        tm_time.tm_min = time.minute;
        tm_time.tm_sec = time.second;
        tm_time.tm_isdst = 0; // 不考虑夏令时
        
        time_t timestamp = mktime(&tm_time);
        int64_t utc_timestamp_ms = (int64_t)timestamp * 1000;
        
        // 构造JSON payload（time字段类型为date UTC时间格式）
        char payload[128];
        char utc_time_str[25];
        // 格式化为ISO 8601 UTC时间格式：YYYY-MM-DDTHH:MM:SSZ
        sprintf(utc_time_str, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
                2000 + time.year, time.month, time.date, 
                time.hour, time.minute, time.second);
        sprintf(payload, "{\"id\":\"%d\",\"version\":\"1.0\",\"params\":{\"time\":\"%s\"}}", 
                (int)timestamp, utc_time_str);
        
        // 调试打印发布的payload
        HAL_UART_Transmit(&huart2, (uint8_t*)"--PUBLISH PAYLOAD: ", 18, 100);
        HAL_UART_Transmit(&huart2, (uint8_t*)payload, strlen(payload), 500);
        HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 100);
        
        // 发布到MQTT
        ESP8266_MQTT_Publish(MQTT_TOPIC_POST, payload, 0, 0);
        */
    }
}

/* USER CODE END 1 */
