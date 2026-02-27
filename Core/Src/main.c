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
	
	#if TEST
	  //OLED TEST
//	 OLED_ShowString(0,0,(uint8_t*)"hello",8,1);
//	 OLED_ShowString(0,8,(uint8_t*)"Welcome",8,1);
//	 OLED_Refresh();
	#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	ESP8266_Init();
	

  //WIFI连接
  while (wifi_try < 5 && !ESP8266_ConnectWiFi())
  {
      HAL_UART_Transmit(&huart2, (uint8_t*)"WiFi connect retry\r\n", 20, 100);
      wifi_try++;
      delay_ms(1000);
  }
  //上云
	ESP8266_ConnectCloud();
	ESP8266_MQTT_Subscribe(MQTT_TOPIC_SET,0);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
		
		if(PCD_Request(PICC_REQALL, g_ucTempbuf)!=PCD_OK) //寻卡
	 {
//		PCD_Reset();
//		PCD_AntennaOff(); 
//		PCD_AntennaOn(); 
//	  continue;
	 }
			 if(PCD_Anticoll(cardid)==PCD_OK)
	 {
		 sprintf(data_show,"id:%02x%02x%02x%02x",cardid[0],cardid[1],cardid[2],cardid[3]);
	   OLED_ShowString(0,8,(uint8_t*)data_show,8,1);
     OLED_Refresh();
	 }
	 #endif
		
		
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
