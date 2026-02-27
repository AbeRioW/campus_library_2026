#include "hc_sr505.h"

uint8_t hc_sr505_counter1 = 0; // HC_SR505_1高电平计数器
uint8_t hc_sr505_counter2 = 0; // HC_SR505_2高电平计数器
uint8_t hc_sr505_counter3 = 0; // HC_SR505_3高电平计数器

/**
  * @brief  检测HC_SR505传感器状态并控制LED
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
		// 高电平，计数器加1
		(*counter)++;
		// 延时一小段时间，避免抖动
		HAL_Delay(100);
	}
	else
	{
		// 低电平，计数器重置
		(*counter) = 0;
		// 延时一小段时间，避免抖动
		HAL_Delay(100);
		HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);
	}

	// 检查是否连续3次高电平
	if (*counter >= 3)
	{
		// 连续3次高电平，拉低并点亮LED
		HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_RESET);
		// 重置计数器，避免重复触发
		(*counter) = 0;
	}
	else
	{
		// 否则，拉高LED
		//HAL_GPIO_WritePin(led_port, led_pin, GPIO_PIN_SET);
	}
}
