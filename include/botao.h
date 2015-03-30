/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/botao.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    17-Fevereiro-2015
  * @brief   Cabe�alho para o m�dulo botao.c
  ******************************************************************************
  */

/* Define para previnir a inclus�o recursiva ---------------------------------*/
#ifndef __BOTAO_H
#define __BOTAO_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Constantes ----------------------------------------------------------------*/
#define LOW		GPIO_PIN_RESET
#define HIGH	GPIO_PIN_SET
#define BOTAO_CLK	__GPIOA_CLK_ENABLE()
#define BOTAO_PORT	GPIOA
#define BOTAO_PIN	GPIO_PIN_4

/* Macros --------------------------------------------------------------------*/
/* Prot�tipos das Fun��es --------------------------------------------------- */
void botaoConfig(void);
GPIO_PinState getSW1(void);


#endif /* __BOTAO_H */
