/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/usart1.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    17-Fevereiro-2015
  * @brief   Cabe�alho para o m�dulo usart1.c
  ******************************************************************************
  */

/* Define para previnir a inclus�o recursiva ---------------------------------*/
#ifndef __USART1_H
#define __USART1_H


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx.h"

/* Constantes ----------------------------------------------------------------*/
/* Macros --------------------------------------------------------------------*/
/* Prot�tipos das Fun��es --------------------------------------------------- */
void usart1Config(void);
int _write(int file, char *ptr, int len);

#endif /* __USART1_H */
