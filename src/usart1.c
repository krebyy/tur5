/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/src/usart1.c
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    20-Março-2015
  * @brief   Funções para configuração e inicialização da USART1
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart1.h"

UART_HandleTypeDef huart1;
uint8_t RxBuffer[BUFFER_SIZE];
uint8_t RxByte;
uint32_t rx_available = 0;

/**
  * @brief Configuração da USART1
  * @param Nenhum
  * @return Nenhum
  */
void usart1Config(void)
{
	__GPIOA_CLK_ENABLE();	// Habilita o barramento de clock do GPIOA
	__USART1_CLK_ENABLE();	// Habilita o barramento de clock da USART1

	// Configura os GPIOs da USART1 como Alternate Function
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Configuração do periférico USART
	huart1.Instance = USART1;
	huart1.Init.BaudRate = BAUD_RATE;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);

	HAL_NVIC_SetPriority(USARTx_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(USARTx_IRQn);

	//__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

	//HAL_UART_IRQHandler(&huart1);
}


/**
  * @brief  Rx Transfer completed callback
  * @param  UartHandle: UART handle
  * @note   This example shows a simple way to report end of IT Rx transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    /*static uint32_t i = 0;

    RxBuffer[i] = RxByte;
    i++;
    if(RxBuffer[i-1] == '\r' || RxBuffer[i-1] == 'I' || RxBuffer[i-1] == 'P' || RxBuffer[i-1] == 'S' || RxBuffer[i-1] == 'G')
    {
        rx_available = i;
        i = 0;
    }*/

    static uint32_t pos = 0;

    //beep(50);

	RxBuffer[pos] = RxByte;//USART1->DR; // alimentou o Buffer
	//printf("%s\r\n", RxBuffer);

	HAL_UART_Receive_IT(UartHandle, &RxByte, 1);

	pos++;
	if (RxBuffer[pos - 1] == '\n')
	{
		//printf("%s\n", RxBuffer);
		rx_available = pos;
		pos = 0;
	}

}


int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1, ptr, len, 100);

	return len;
}

int _read (int file, char *ptr, int len)
{
	HAL_UART_Receive(&huart1, ptr, len, 100);

	return len;
}
