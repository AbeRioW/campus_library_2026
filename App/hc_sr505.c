#include "hc_sr505.h"
#include "mqtt_publisher.h"

uint8_t hc_sr505_counter1 = 0; // HC_SR505_1高电平计数器
uint8_t hc_sr505_counter2 = 0; // HC_SR505_2高电平计数器
uint8_t hc_sr505_counter3 = 0; // HC_SR505_3高电平计数器

// 非阻塞式状态变量
static uint32_t last_check_time1 = 0;
static uint32_t last_check_time2 = 0;
static uint32_t last_check_time3 = 0;
static uint8_t last_state1 = 0;
static uint8_t last_state2 = 0;
static uint8_t last_state3 = 0;

// MQTT发布状态标志
static uint8_t mqtt_pub_state1 = 0; // 0=未发布true, 1=已发布true
static uint8_t mqtt_pub_state2 = 0; // 0=未发布true, 1=已发布true
static uint8_t mqtt_pub_state3 = 0; // 0=未发布true, 1=已发布true

#define HC_SR505_CHECK_INTERVAL 100  // 检测间隔100ms

/**
  * @brief  检测HC_SR505传感器状态并控制LED（阻塞式，已废弃）
  * @param  sensor_port: 传感器端口
  * @param  sensor_pin: 传感器引脚
  * @param  led_port: LED端口
  * @param  led_pin: LED引脚
  * @param  counter: 计数器指针
  * @retval None
  */
void CheckHC_SR505(GPIO_TypeDef* sensor_port, uint16_t sensor_pin, GPIO_TypeDef* led_port, uint16_t led_pin, uint8_t* counter)
{
	// 检测传感器状态
	if (HAL_GPIO_ReadPin(sensor_port, sensor_pin) == GPIO_PIN_SET)
	{
		// 高电平计数器加1
		(*counter)++;
		// 延时一小段时间，防止抖动
		HAL_Delay(100);
	}
	else
	{
		// 低电平计数器清零
		(*counter) = 0;
		// 延时一小段时间，防止抖动
		HAL_Delay(100);
		HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);
	}

	// 判断是否连续3次高电平
	if (*counter >= 3)
	{
		// 连续3次高电平，触发报警LED
		HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_RESET);
		// 清零计数器避免重复触发
		(*counter) = 0;
	}
}

/* ==================== 非阻塞式接口 ==================== */

/**
  * @brief  非阻塞式检测HC_SR505_1，带MQTT发布功能
  * @retval None
  */
void CheckHC_SR505_1_NB(void)
{
	uint32_t current_time = HAL_GetTick();
	
	// 检查是否到达检测间隔
	if ((current_time - last_check_time1) < HC_SR505_CHECK_INTERVAL)
	{
		return;
	}
	last_check_time1 = current_time;
	
	// 读取当前传感器状态
	uint8_t current_state = (HAL_GPIO_ReadPin(HC_SR505_1_GPIO_Port, HC_SR505_1_Pin) == GPIO_PIN_SET) ? 1 : 0;
	
	// 检测传感器状态
	if (current_state == 1)
	{
		// 高电平计数器加1
		hc_sr505_counter1++;
		
		// 连续3次高电平，触发LED并发布MQTT
		if (hc_sr505_counter1 >= 3)
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
			hc_sr505_counter1 = 0;
			
			// 发布SET1 true（只在状态变化时发布一次）
			if (mqtt_pub_state1 == 0)
			{
				mqtt_pub_state1 = 1;
				MQTT_Publish_SET1("true");
			}
		}
	}
	else
	{
		// 低电平计数器清零，关闭LED
		hc_sr505_counter1 = 0;
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		
		// 发布SET1 false（只在状态变化时发布一次）
		if (mqtt_pub_state1 == 1)
		{
			mqtt_pub_state1 = 0;
			MQTT_Publish_SET1("false");
		}
	}
	
	last_state1 = current_state;
}

/**
  * @brief  非阻塞式检测HC_SR505_2，带MQTT发布功能
  * @retval None
  */
void CheckHC_SR505_2_NB(void)
{
	uint32_t current_time = HAL_GetTick();
	
	// 检查是否到达检测间隔
	if ((current_time - last_check_time2) < HC_SR505_CHECK_INTERVAL)
	{
		return;
	}
	last_check_time2 = current_time;
	
	// 读取当前传感器状态
	uint8_t current_state = (HAL_GPIO_ReadPin(HC_SR505_2_GPIO_Port, HC_SR505_2_Pin) == GPIO_PIN_SET) ? 1 : 0;
	
	// 检测传感器状态
	if (current_state == 1)
	{
		// 高电平计数器加1
		hc_sr505_counter2++;
		
		// 连续3次高电平，触发LED并发布MQTT
		if (hc_sr505_counter2 >= 3)
		{
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
			hc_sr505_counter2 = 0;
			
			// 发布SET2 true（只在状态变化时发布一次）
			if (mqtt_pub_state2 == 0)
			{
				mqtt_pub_state2 = 1;
				MQTT_Publish_SET2("true");
			}
		}
	}
	else
	{
		// 低电平计数器清零，关闭LED
		hc_sr505_counter2 = 0;
		HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
		
		// 发布SET2 false（只在状态变化时发布一次）
		if (mqtt_pub_state2 == 1)
		{
			mqtt_pub_state2 = 0;
			MQTT_Publish_SET2("false");
		}
	}
	
	last_state2 = current_state;
}

/**
  * @brief  非阻塞式检测HC_SR505_3，带MQTT发布功能
  * @retval None
  */
void CheckHC_SR505_3_NB(void)
{
	uint32_t current_time = HAL_GetTick();
	
	// 检查是否到达检测间隔
	if ((current_time - last_check_time3) < HC_SR505_CHECK_INTERVAL)
	{
		return;
	}
	last_check_time3 = current_time;
	
	// 读取当前传感器状态
	uint8_t current_state = (HAL_GPIO_ReadPin(HC_SR505_3_GPIO_Port, HC_SR505_3_Pin) == GPIO_PIN_SET) ? 1 : 0;
	
	// 检测传感器状态
	if (current_state == 1)
	{
		// 高电平计数器加1
		hc_sr505_counter3++;
		
		// 连续3次高电平，触发LED并发布MQTT
		if (hc_sr505_counter3 >= 3)
		{
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
			hc_sr505_counter3 = 0;
			
			// 发布SET3 true（只在状态变化时发布一次）
			if (mqtt_pub_state3 == 0)
			{
				mqtt_pub_state3 = 1;
				MQTT_Publish_SET3("true");
			}
		}
	}
	else
	{
		// 低电平计数器清零，关闭LED
		hc_sr505_counter3 = 0;
		HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
		
		// 发布SET3 false（只在状态变化时发布一次）
		if (mqtt_pub_state3 == 1)
		{
			mqtt_pub_state3 = 0;
			MQTT_Publish_SET3("false");
		}
	}
	
	last_state3 = current_state;
}

/**
  * @brief  同时检测所有HC_SR505传感器（非阻塞式）
  * @retval None
  */
void CheckAllHC_SR505_NB(void)
{
	CheckHC_SR505_1_NB();
	CheckHC_SR505_2_NB();
	CheckHC_SR505_3_NB();
}
