/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/encoders.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    21-Mar�o-2015
  * @brief   Cabe�alho para o m�dulo encoders.c
  ******************************************************************************
  */

/* Define para previnir a inclus�o recursiva ---------------------------------*/
#ifndef __ENCODERS_H
#define __ENCODERS_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Constantes ----------------------------------------------------------------*/
#define BUZZER_PORT	GPIOB
#define BUZZER_PIN	GPIO_PIN_4

/* Macros --------------------------------------------------------------------*/
#define BUZZER_CLK	__GPIOB_CLK_ENABLE()
#define BEEP_ON		HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET)
#define BEEP_OFF	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET)

/* Prot�tipos das Fun��es --------------------------------------------------- */
void encodersConfig(void);
int32_t getLeftEncCount(void);
int32_t getRightEncCount(void);
void resetRightEncCount(void);
void resetLeftEncCount(void);


#endif /* __ENCODERS_H */
