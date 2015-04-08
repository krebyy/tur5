/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/main.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    16-Fevereiro-2015
  * @brief   Inclusão das bibliotecas de usuário e definição de parâmetros
  ******************************************************************************
  */

/* Define para previnir a inclusão recursiva ---------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "buzzer.h"
#include "leds.h"
#include "motores.h"
#include "botao.h"
#include "delays.h"
#include "usart1.h"
#include "sensores.h"
#include "encoders.h"
#include "speedProfile.h"
#include "flash.h"


/* Constantes ----------------------------------------------------------------*/
/* Macros --------------------------------------------------------------------*/
/* Protótipos das Funções --------------------------------------------------- */
void systick(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void tratamento_desvio(void);
void tratamento_rampa(void);
void tratamento_loop(void);
void parametros_defaut(void);
void init_paramentros(void);


#endif /* __MAIN_H */
