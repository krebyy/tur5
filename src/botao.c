/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/src/botao.c
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    17-Fevereiro-2015
  * @brief   Fun��es para acionamento do Bot�o SW1
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "botao.h"


/**
  * @brief Configura��o do GPIO do bot�o SW1
  * @param Nenhum
  * @return Nenhum
  */
void botaoConfig(void)
{
	BOTAO_CLK;	// Habilita o barramento de clock do GPIO do bot�o

	// Configura o GPIO do Bot�o como entrada sem resistor interno
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = BOTAO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BOTAO_PORT, &GPIO_InitStructure);
}


/**
  * @brief Verifica o estado do bot�o SW1
  * @param Nenhum
  * @return estado LOW ou HIGH
  */
GPIO_PinState getSW1(void)
{
	return HAL_GPIO_ReadPin(BOTAO_PORT, BOTAO_PIN);
}
