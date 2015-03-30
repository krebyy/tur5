/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/buzzer.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    16-Fevereiro-2015
  * @brief   Cabeçalho para o módulo buzzer.c
  ******************************************************************************
  */

/* Define para previnir a inclusão recursiva ---------------------------------*/
#ifndef __BUZZER_H
#define __BUZZER_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "delays.h"

/* Constantes ----------------------------------------------------------------*/
#define BUZZER_PORT	GPIOB
#define BUZZER_PIN	GPIO_PIN_4

/* Macros --------------------------------------------------------------------*/
#define BUZZER_CLK	__GPIOB_CLK_ENABLE()
#define BEEP_ON		HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET)
#define BEEP_OFF	HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET)

/* Protótipos das Funções --------------------------------------------------- */
void buzzerConfig(void);
void beeps(int vezes, int t_on, int t_off);
void beep(int duracao);


extern int32_t buzzerTime;


#endif /* __BUZZER_H */
