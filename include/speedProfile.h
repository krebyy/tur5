/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/include/sensores.h
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    01-Abril-2015
  * @brief   Cabeçalho para o módulo speedProfile.c
  ******************************************************************************
  */

/* Define para previnir a inclusão recursiva ---------------------------------*/
#ifndef __SPEEDPROFILE_H
#define __SPEEDPROFILE_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "encoders.h"
#include "motores.h"

/* Constantes ----------------------------------------------------------------*/
#define KP_X 10
#define KD_X 0
#define KP_W 10
#define KD_W 10

#define TS 5	// Tempo de atualização em [ms]

// Timer para base de tempo de atualização
#define TIM_CLK			__TIM3_CLK_ENABLE()
#define TIM				TIM3
#define TIM_PERIOD		499
#define TIM_PRESCALER	839
#define TIMx_IRQn		TIM3_IRQn
#define TIMx_IRQHandler	TIM3_IRQHandler

#define CNT_PER_1000MM 3560


/* Macros --------------------------------------------------------------------*/
#define SPEED_TO_COUNTS(speed)	((CNT_PER_1000MM * (speed) * TS) / 1000000)
#define ACC_TO_COUNTS(acc)	((((CNT_PER_1000MM * (acc) * TS) / 1000) * TS) / 1000000)
#define COUNTS_TO_MM(cnt)	(((cnt) * 1000) / CNT_PER_1000MM)
/* Protótipos das Funções --------------------------------------------------- */
void speedProfileConfig(void);
int32_t speedProfile(void);
int32_t getEncoderStatus(void);
void updateCurrentSpeed(void);
void calculateMotorPwm(void);


extern TIM_HandleTypeDef htim3;
extern int32_t targetSpeedX, targetSpeedW;
extern int32_t accX, decX, accW, decW;


#endif /* __SPEEDPROFILE_H */
