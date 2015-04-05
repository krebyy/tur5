/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/src/flash.c
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    05-Abril-2015
  * @brief   Fun��es de acesso � mem�ria flash para guardar dados
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "flash.h"


/**
  * @brief Fun��o para gravar um buffer na Flash
  * @param buffer com os dados a serem gravados
  * @param num_words (tamanho do buffer)
  * @return 0: erro durante a grava��o; 1: grava��o realizada com sucesso
  */
uint8_t writeFlash(uint32_t *buffer, uint32_t num_words)
{
	uint32_t startAddress = ADDR_FLASH_SECTOR;

	HAL_FLASH_Unlock();
	for(uint32_t i = 0; i < num_words; i++)
	{
		if(HAL_FLASH_Program(TYPEPROGRAM_WORD,(uint32_t)(startAddress+(i*4)),*buffer)!= HAL_OK)
		{
			HAL_FLASH_Lock();
			return 0;
		}
		buffer++;
	}

	HAL_FLASH_Lock();
	return 1;
}


/**
  * @brief Fun��o para ler o buffer gravado na Flash pela writeFlash
  * @param buffer para receber os dados
  * @param num_words (tamanho do buffer)
  * @return nenhum
  */
void readFlash(uint32_t *buffer, uint32_t num_words)
{
	uint32_t startAddress = ADDR_FLASH_SECTOR;

	for(uint32_t i = 0; i < num_words; i++)
	{
		buffer[i] = *(uint16_t *)(startAddress + (i*4));
	}
}
