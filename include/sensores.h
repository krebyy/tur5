/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/sensores.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    17-Fevereiro-2015
  * @brief   Cabeçalho para o módulo sensores.c
  ******************************************************************************
  */

/* Define para previnir a inclusão recursiva ---------------------------------*/
#ifndef __SENSORES_H
#define __SENSORES_H


/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "delays.h"

/* Constantes ----------------------------------------------------------------*/
#define LOW		GPIO_PIN_RESET
#define HIGH	GPIO_PIN_SET
#define LINHA	GPIO_PIN_RESET	// RESET (linha branca); SET (linha preta)
#define INFINITO	8888		// Valor que indica a não identificação de linha

#define SENSORES_CLK	__GPIOA_CLK_ENABLE(); __GPIOB_CLK_ENABLE(); __GPIOC_CLK_ENABLE(); __GPIOD_CLK_ENABLE()

#define N_EMISSORES	6
#define LF_E_PORT	GPIOD
#define LF_E_PIN	GPIO_PIN_2
#define POWER1_PORT	GPIOC
#define POWER1_PIN	GPIO_PIN_12
#define POWER2_PORT	GPIOC
#define POWER2_PIN	GPIO_PIN_11
#define RF_E_PORT	GPIOC
#define RF_E_PIN	GPIO_PIN_10
#define L_LINE_PORT	GPIOA
#define L_LINE_PIN	GPIO_PIN_14
#define R_LINE_PORT	GPIOA
#define R_LINE_PIN	GPIO_PIN_13

#define N_RECEPTORES	8
#define LINE1_PORT	GPIOC
#define LINE1_PIN	GPIO_PIN_9
#define LINE2_PORT	GPIOC
#define LINE2_PIN	GPIO_PIN_8
#define LINE3_PORT	GPIOC
#define LINE3_PIN	GPIO_PIN_7
#define LINE4_PORT	GPIOC
#define LINE4_PIN	GPIO_PIN_6
#define LINE5_PORT	GPIOB
#define LINE5_PIN	GPIO_PIN_15
#define LINE6_PORT	GPIOB
#define LINE6_PIN	GPIO_PIN_14
#define LINE7_PORT	GPIOB
#define LINE7_PIN	GPIO_PIN_13
#define LINE8_PORT	GPIOB
#define LINE8_PIN	GPIO_PIN_12

#define N_ANALOGICAS	7
#define LF_R_PORT	GPIOC
#define LF_R_PIN	GPIO_PIN_5
#define LF_R_CH		ADC_CHANNEL_15
#define L_R_PORT	GPIOC
#define L_R_PIN		GPIO_PIN_4
#define L_R_CH		ADC_CHANNEL_14
#define R_R_PORT	GPIOA
#define R_R_PIN		GPIO_PIN_7
#define R_R_CH		ADC_CHANNEL_7
#define RF_R_PORT	GPIOA
#define RF_R_PIN	GPIO_PIN_6
#define RF_R_CH		ADC_CHANNEL_6
#define G_OUTZ_PORT	GPIOB
#define G_OUTZ_PIN	GPIO_PIN_1
#define G_OUTZ_CH	ADC_CHANNEL_9
#define G_VREF_PORT	GPIOB
#define G_VREF_PIN	GPIO_PIN_0
#define G_VREF_CH	ADC_CHANNEL_8
#define VBAT_PORT	GPIOA
#define VBAT_PIN	GPIO_PIN_2
#define VBAT_CH		ADC_CHANNEL_2
#define VBAT_RAW	3434
#define VBAT_V		8270
#define VBAT_ALERTA 7000

/* Macros --------------------------------------------------------------------*/
/* Protótipos das Funções --------------------------------------------------- */
void sensoresConfig(void);
int32_t getSensoresParede(int32_t*, int32_t*, int32_t*, int32_t*);
int32_t getSensoresLinha();
int32_t readLine();
int32_t getGyro();
int32_t getTensao();
uint32_t getRawADC(uint32_t canal);



#endif /* __SENSORES_H */
