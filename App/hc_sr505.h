#ifndef __HC_SR505_H
#define __HC_SR505_H 

#include "main.h"


extern uint8_t hc_sr505_counter1; // HC_SR505_1高电平计数器
extern uint8_t hc_sr505_counter2; // HC_SR505_2高电平计数器
extern uint8_t hc_sr505_counter3; // HC_SR505_3高电平计数器

// 阻塞式接口（已废弃，不建议使用）
void CheckHC_SR505(GPIO_TypeDef* sensor_port, uint16_t sensor_pin, GPIO_TypeDef* led_port, uint16_t led_pin, uint8_t* counter);

// 非阻塞式接口（推荐）
void CheckHC_SR505_1_NB(void);
void CheckHC_SR505_2_NB(void);
void CheckHC_SR505_3_NB(void);
void CheckAllHC_SR505_NB(void);

#endif
