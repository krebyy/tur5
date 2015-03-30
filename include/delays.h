/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/delays.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    19-Mar�o-2015
  * @brief   Cabe�alho para o m�dulo delays.c
  ******************************************************************************
  */

/* Define para previnir a inclus�o recursiva ---------------------------------*/
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
