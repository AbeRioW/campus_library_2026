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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "esp8266.h"
#include "oled.h"
#include "hc_sr505.h"
#include "uln2003.h"
#include "rc522.h"
#include "ds1302.h"
#include "flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TEST 0
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  uint8_t wifi_try = 0, mqtt_try = 0;
		  uint8_t cardid[4]={0x00,0x00,0x00,0x00};
		char data_show[20];
					uint8_t g_ucTempbuf[20]; 
		uint32_t oled_show_time = 0;
		uint8_t oled_show_active = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	//NFC初始化
	PCD_Reset();
  PCD_AntennaOff(); 
  PCD_AntennaOn();
  Delay_Init();
	OLED_Init();
	DS1302_Init();
	ULN2003_Init();
	Flash_Init();
	
	#if TEST
	  //OLED TEST
//	 OLED_ShowString(0,0,(uint8_t*)"hello",8,1);
//	 OLED_ShowString(0,8,(uint8_t*)"Welcome",8,1);
//	 OLED_Refresh();
	#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//	ESP8266_Init();
//	

//  //WIFI连接
//  while (wifi_try < 5 && !ESP8266_ConnectWiFi())
//  {
//      HAL_UART_Transmit(&huart2, (uint8_t*)"WiFi connect retry\r\n", 20, 100);
//      wifi_try++;
//      delay_ms(1000);
//  }
//  //上云
//	if(ESP8266_ConnectCloud()==false)
//	{
//		  HAL_UART_Transmit(&huart2, (uint8_t*)"ConnectCloud failed\r\n", 22, 100);
//		  while(1);
//	}
//		
//	// 订阅发布属性回复主题（OneNET要求）
//	if(!ESP8266_MQTT_Subscribe(MQTT_TOPIC_POST_REPLY,1))
//	{
//		  HAL_UART_Transmit(&huart2, (uint8_t*)"MQTT subscribe post_reply failed\r\n", 32, 100);
//		  while(1);
//	}
//	
//	if(!ESP8266_MQTT_Subscribe(MQTT_TOPIC_SET,0))
//	{
//		  HAL_UART_Transmit(&huart2, (uint8_t*)"MQTT subscribe failed\r\n", 22, 100);
//		  while(1);
//	}
	
	// 启动定时器4，用于每秒读取DS1302时间
	HAL_TIM_Base_Start_IT(&htim4);
	
	// 初始化NFC检测时间戳
	uint32_t nfc_last_check_time = 0;
	const uint32_t NFC_CHECK_INTERVAL = 100; // NFC检测间隔100ms
	
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		uint8_t cardid[4] = {0x00, 0x00, 0x00, 0x00};
		char data_show[20];
		uint32_t current_time = HAL_GetTick();
		
		// 处理非阻塞式电机控制（每次循环都调用，确保电机转动流畅）
		ULN2003_Handle_NB();
		
		// NFC检测使用基于时间戳的间隔控制
		if((current_time - nfc_last_check_time) >= NFC_CHECK_INTERVAL)
		{
			nfc_last_check_time = current_time;
			
			if(nfc_register_mode && nfc_delete_mode)
			{
				HAL_UART_Transmit(&huart2, (uint8_t*)"ERROR: Both modes ON\r\n", 22, 100);
				nfc_register_mode = false;
				nfc_delete_mode = false;
			}
			
			if(nfc_register_mode)
			{
				HAL_UART_Transmit(&huart2, (uint8_t*)"Reg mode\r\n", 10, 100);
				if(PCD_Request(PICC_REQALL, g_ucTempbuf) == PCD_OK)
				{
					if(PCD_Anticoll(cardid) == PCD_OK)
					{
						uint8_t found = Flash_FindID(cardid);
						if(found == 0xFF)
						{
							uint8_t ids[40];
							Flash_ReadIDs(ids);
							uint8_t empty_index = 0xFF;
							for(uint8_t i = 0; i < 10; i++)
							{
								uint8_t empty = 1;
								for(uint8_t j = 0; j < 4; j++)
								{
									if(ids[i * 4 + j] != 0xFF)
									{
										empty = 0;
										break;
									}
								}
								if(empty)
								{
									empty_index = i;
									break;
								}
							}
							if(empty_index != 0xFF)
							{
								Flash_WriteID(empty_index, cardid);
								sprintf(data_show, "Reg:%02X%02X%02X%02X", cardid[0], cardid[1], cardid[2], cardid[3]);
								OLED_ShowString(0, 8, (uint8_t*)data_show, 8, 1);
								oled_show_time = HAL_GetTick();
								oled_show_active = 1;
							}
							else
							{
								OLED_ShowString(0, 8, (uint8_t*)"Full", 8, 1);
								oled_show_time = HAL_GetTick();
								oled_show_active = 1;
							}
							OLED_Refresh();
							PCD_Halt();
							nfc_register_mode = false;
							HAL_UART_Transmit(&huart2, (uint8_t*)"Reg mode exit\r\n", 15, 100);
						}
						else
						{
							OLED_ShowString(0, 8, (uint8_t*)"ID already reg", 8, 1);
							OLED_Refresh();
							delay_ms(1000);
							OLED_ShowString(0, 8, (uint8_t*)"                ", 8, 1);
							OLED_Refresh();
							nfc_register_mode = false;
							PCD_Halt();
							HAL_UART_Transmit(&huart2, (uint8_t*)"Reg mode exit\r\n", 15, 100);
						}
					}
				}
			}
			else if(nfc_delete_mode)
			{
				HAL_UART_Transmit(&huart2, (uint8_t*)"Del mode\r\n", 10, 100);
				if(PCD_Request(PICC_REQALL, g_ucTempbuf) == PCD_OK)
				{
					HAL_UART_Transmit(&huart2, (uint8_t*)"Card detected\r\n", 15, 100);
					if(PCD_Anticoll(cardid) == PCD_OK)
					{
						HAL_UART_Transmit(&huart2, (uint8_t*)"Card ID read\r\n", 14, 100);
						uint8_t found = Flash_FindID(cardid);
						if(found != 0xFF)
						{
							Flash_DeleteID(found);
							delay_ms(500);
							sprintf(data_show, "Del:%02X%02X%02X%02X", cardid[0], cardid[1], cardid[2], cardid[3]);
							OLED_ShowString(0, 8, (uint8_t*)data_show, 8, 1);
							oled_show_time = HAL_GetTick();
							oled_show_active = 1;
						}
						else
						{
							delay_ms(500);
							OLED_ShowString(0, 8, (uint8_t*)"Illegal card", 8, 1);
							oled_show_time = HAL_GetTick();
							oled_show_active = 1;
						}
						OLED_Refresh();
						delay_ms(2000);
						PCD_Halt();
						nfc_delete_mode = false;
					}
				}
			}
			else
			{
				if(PCD_Request(PICC_REQALL, g_ucTempbuf) == PCD_OK)
				{
					if(PCD_Anticoll(cardid) == PCD_OK)
					{
						uint8_t found = Flash_FindID(cardid);
						
						if(found != 0xFF)
						{
							sprintf(data_show, "ID:%02X%02X%02X%02X", cardid[0], cardid[1], cardid[2], cardid[3]);
							OLED_ShowString(0, 8, (uint8_t*)data_show, 8, 1);
							oled_show_time = HAL_GetTick();
							oled_show_active = 1;
							OLED_Refresh();
							HAL_UART_Transmit(&huart2, (uint8_t*)"Motor start\r\n", 12, 100);
							// 使用非阻塞式启动电机
							ULN2003_StartForward_NB(100);
							HAL_UART_Transmit(&huart2, (uint8_t*)"Motor started (NB)\r\n", 19, 100);
						}
						else
						{
							OLED_ShowString(0, 8, (uint8_t*)"Illegal card", 8, 1);
							oled_show_time = HAL_GetTick();
							oled_show_active = 1;
						}
						OLED_Refresh();
						PCD_Halt();
					}
				}
			}
		}
		
		if(oled_show_active && (HAL_GetTick() - oled_show_time) > 5000)
		{
			OLED_ShowString(0, 8, (uint8_t*)"                ", 8, 1);
			OLED_Refresh();
			oled_show_active = 0;
		}
		
		// 短延时，确保主循环快速执行，电机控制流畅
		delay_ms(5);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//相关测试
		#if TEST 
//		OLED_Clear();
//	  OLED_ShowString(0,0,(uint8_t*)"TEST",8,1);
//	  OLED_Refresh();
		
	  // 读取当前时间并显示
//		DS1302_Time current_time;
//		DS1302_GetTime(&current_time);
		
		// 格式化时间字符串
//		char time_str[40];
//		sprintf(time_str, "20%02d-%02d-%02d %02d:%02d:%02d", current_time.year, current_time.month, current_time.date,current_time.hour,current_time.minute,current_time.second);
//		OLED_ShowString(0, 8, (uint8_t*)time_str, 8, 1);
//		 OLED_Refresh();
//		HAL_Delay(1000);
//		
//		OLED_Refresh();
//	 //HCSR505_TEST
//	 CheckHC_SR505(HC_SR505_1_GPIO_Port, HC_SR505_1_Pin, LED1_GPIO_Port, LED1_Pin, &hc_sr505_counter1);	
//	 // 检测HC_SR505_2状态并控制LED2
//	 CheckHC_SR505(HC_SR505_2_GPIO_Port, HC_SR505_2_Pin, LED2_GPIO_Port, LED2_Pin, &hc_sr505_counter2);		
//	 // 检测HC_SR505_3状态并控制LED3
//		CheckHC_SR505(HC_SR505_3_GPIO_Port, HC_SR505_3_Pin, LED3_GPIO_Port, LED3_Pin, &hc_sr505_counter3);

		//步进电机测试
		//ULN2003_Rotate(90,0);
		
//		if(PCD_Request(PICC_REQALL, g_ucTempbuf)!=PCD_OK) //寻卡
//	 {
////		PCD_Reset();
////		PCD_AntennaOff(); 
////		PCD_AntennaOn(); 
////	  continue;
//	 }
//			 if(PCD_Anticoll(cardid)==PCD_OK)
//	 {
//		 sprintf(data_show,"id:%02x%02x%02x%02x",cardid[0],cardid[1],cardid[2],cardid[3]);
//	   OLED_ShowString(0,8,(uint8_t*)data_show,8,1);
//     OLED_Refresh();
//	 }
	 #endif
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
