/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/usart1.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    17-Fevereiro-2015
  * @brief   Cabeçalho para o módulo usart1.c
  ******************************************************************************
  */

/* Define para previnir a inclusão recursiva ---------------------------------*/
#ifndef __USART1_H
#define __USART1_H


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "stm32f4xx.h"

/* Constantes ----------------------------------------------------------------*/
#define BAUD_RATE	115200
#define BUFFER_SIZE	10

#define USARTx_IRQn			USART1_IRQn
#define USARTx_IRQHandler	USART1_IRQHandler
/* Macros --------------------------------------------------------------------*/
/* Protótipos das Funções --------------------------------------------------- */
void usart1Config(void);
int _write(int file, char *ptr, int len);

extern UART_HandleTypeDef huart1;
extern uint8_t RxBuffer[BUFFER_SIZE];
extern uint8_t RxByte;
extern uint32_t rx_available;

#endif /* __USART1_H */
