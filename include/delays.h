/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/delays.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    19-Março-2015
  * @brief   Cabeçalho para o módulo delays.c
  ******************************************************************************
  */

/* Define para previnir a inclusão recursiva ---------------------------------*/
#ifndef __DELAYS_H
#define __DELAYS_H

#include "stm32f4xx.h"

#define SYSTEM_FREQ 168

void SystemClock_Config(void);
uint32_t micros(void);
uint32_t millis(void);
void elapse_us(uint32_t targetTime, uint32_t oldt);
void delay_ms(uint32_t delay);

#endif /* __DELAYS_H */
