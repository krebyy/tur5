/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/flash.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    05-Abril-2015
  * @brief   Cabe�alho para o m�dulo flash.c
  ******************************************************************************
  */

/* Define para previnir a inclus�o recursiva ---------------------------------*/
#ifndef __FLASH_H
#define __FLASH_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_hal_flash.h"

/* Constantes ----------------------------------------------------------------*/
#define	ADDR_FLASH_SECTOR	0x080E0000	// Endere�o do setor utilizado - Setor 11 do STM32F405
/* Macros --------------------------------------------------------------------*/
/* Prot�tipos das Fun��es --------------------------------------------------- */
uint8_t writeFlash(uint32_t *buffer, uint32_t num_words);
void readFlash(uint32_t *buffer, uint32_t num_words);



#endif /* __FLASH_H */
