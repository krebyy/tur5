/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/main.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    16-Fevereiro-2015
  * @brief   Inclus�o das bibliotecas de usu�rio e defini��o de par�metros
  ******************************************************************************
  */

/* Define para previnir a inclus�o recursiva ---------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "buzzer.h"
#include "leds.h"
#include "motores.h"
#include "botao.h"
#include "delays.h"
#include "usart1.h"
#include "sensores.h"
#include "encoders.h"
#include "speedProfile.h"


/* Constantes ----------------------------------------------------------------*/
/* Macros --------------------------------------------------------------------*/
/* Prot�tipos das Fun��es --------------------------------------------------- */
void systick(void);


#endif /* __MAIN_H */
