/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/src/sensores.c
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    01-Abril-2015
  * @brief   Funções para implementação da técnica de perfil de velocidade
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "speedProfile.h"


TIM_HandleTypeDef htim3;

int32_t leftEncoderChange = 0, rightEncoderChange = 0;
int32_t encoderChange = 0, encoderCount = 0;

int32_t leftEncoderOld = 0, rightEncoderOld = 0;
int32_t leftEncoderCount = 0, rightEncoderCount = 0;
int32_t distanceLeft = 0;

int32_t curSpeedX = 0, curSpeedW = 0;
int32_t targetSpeedX = 0, targetSpeedW = 0;
int32_t accX = 0, decX = 0, accW = 0, decW = 0;
int32_t distance_mm = 0;


/**
  * @brief Função para executar os perfis de velocidade translacional e rotacional
  * @param dist: (mm ou deg) distância translacional ou rotacional
  * @param r: 0 - translacional; != 0 - raio da curva (mm)
  * @param iSpeed: (mm/s ou deg/s) velocidade inicial do perfil
  * @param topSpeed: (mm/s ou deg/s) velocidade máxima do perfil
  * @param endSpeed: (mm/s ou deg/s) velocidade final do perfil
  * @param acc: (mm/s/s ou deg/s/s) aceleração do perfil
  * @return Nenhum
  */
/*uint32_t runProfile(int32_t dist, int32_t r, int32_t iSpeed, int32_t topSpeed, int32_t endSpeed, int32_t acc)
{
	uint32_t t1 = 0, t2 = 0, t3 = 0, T = 0;

	if(r == 0)	// Movimento translacional
	{
		t1 = (((topSpeed - iSpeed) * 1000) / acc);
		t3 = (((topSpeed - endSpeed) * 1000) / acc);
		t2 = ((dist * 1000) / topSpeed);
		t2 = (t2 - (((topSpeed + iSpeed) * t1) / (2 * topSpeed)) - (((topSpeed + endSpeed) * t3) / (2 * topSpeed)));
		T =  t1 + t2 + t3;

		for(int32_t i = 0; i < T; i++)
		{

		}
	}
	else	// Movimento rotacional
	{
		t1 = (((topSpeed - iSpeed) * 1000) / acc);
		t3 = (((topSpeed - endSpeed) * 1000) / acc);
		t2 = ((dist * 1000) / topSpeed);
		t2 = (t2 - (t1 / 2) - (t3 / 2));
		T =  t1 + t2 + t3;

		for(int32_t i = 0; i < T; i++)
		{

		}
	}
	return T;
}*/




/**
  * @brief Configuração da Base de Tempo de atualização do speedProfile
  * @param Nenhum
  * @return Nenhum
  */
void speedProfileConfig(void)
{
	TIM_CLK;

	// Configuração da base de tempo do speedProfile
	htim3.Instance = TIM;
	htim3.Init.Period = TIM_PERIOD;
	htim3.Init.Prescaler = TIM_PRESCALER;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&htim3);

	HAL_TIM_Base_Start_IT(&htim3);

	HAL_NVIC_SetPriority(TIMx_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIMx_IRQn);

	HAL_TIM_IRQHandler(&htim3);
}


int32_t speedProfile(void)
{
	int32_t distanceLeft = getEncoderStatus();
	updateCurrentSpeed();
	calculateMotorPwm();

	return COUNTS_TO_MM(distanceLeft);
}

void resetSpeedProfile(void)
{
	leftEncoderChange = 0; rightEncoderChange = 0;
	encoderChange = 0; encoderCount = 0;

	leftEncoderOld = 0; rightEncoderOld = 0;
	leftEncoderCount = 0; rightEncoderCount = 0;
	distanceLeft = 0;

	curSpeedX = 0; curSpeedW = 0;
	targetSpeedW = 0;
	distance_mm = 0;

	resetLeftEncCount();
	resetRightEncCount();
}

int32_t getEncoderStatus(void)
{
	int32_t leftEncoder, rightEncoder;

	leftEncoder = getLeftEncCount();
	rightEncoder = getRightEncCount();

	leftEncoderChange = leftEncoder - leftEncoderOld;
	rightEncoderChange = rightEncoder - rightEncoderOld;
	encoderChange = (leftEncoderChange + rightEncoderChange) / 2;

	leftEncoderOld = leftEncoder;
	rightEncoderOld = rightEncoder;

	leftEncoderCount += leftEncoderChange;
	rightEncoderCount += rightEncoderChange;
	encoderCount =  (leftEncoderCount + rightEncoderCount) / 2;

	distanceLeft += encoderChange;// update distanceLeft
	distance_mm = COUNTS_TO_MM(distanceLeft);

	return distanceLeft;
}

void updateCurrentSpeed(void)
{
	if(curSpeedX < targetSpeedX)
	{
		curSpeedX += SPEED_TO_COUNTS(accX*2);
		if(curSpeedX > targetSpeedX)
			curSpeedX = targetSpeedX;
	}
	else if(curSpeedX > targetSpeedX)
	{
		curSpeedX -= SPEED_TO_COUNTS(decX*2);
		if(curSpeedX < targetSpeedX)
			curSpeedX = targetSpeedX;
	}
	if(curSpeedW < targetSpeedW)
	{
		curSpeedW += accW;
		if(curSpeedW > targetSpeedW)
			curSpeedW = targetSpeedW;
	}
	else if(curSpeedW > targetSpeedW)
	{
		curSpeedW -= decW;
		if(curSpeedW < targetSpeedW)
			curSpeedW = targetSpeedW;
	}
}

void calculateMotorPwm(void) // encoder PD controller
{
	int32_t encoderFeedbackX, encoderFeedbackW;
	static int32_t posPwmX = 0, posPwmW = 0;
	static int32_t posErrorX = 0, posErrorW = 0;
	static int32_t oldPosErrorX = 0, oldPosErrorW = 0;

    /* simple PD loop to generate base speed for both motors */
	encoderFeedbackX = leftEncoderChange + rightEncoderChange;
	encoderFeedbackW = leftEncoderChange - rightEncoderChange;

	posErrorX += curSpeedX - encoderFeedbackX;
	posErrorW += curSpeedW - encoderFeedbackW;

	posPwmX = KP_X * posErrorX + KD_X * (posErrorX - oldPosErrorX);
	posPwmW = KP_W * posErrorW + KD_W * (posErrorW - oldPosErrorW);

	oldPosErrorX = posErrorX;
	oldPosErrorW = posErrorW;

	setMotores(posPwmX + posPwmW, posPwmX - posPwmW);
}

void controlMotorPwm2(void)
{
	int32_t leftSpeedPwmX, rightSpeedPwmX;
	static int32_t leftSpeedErrorX = 0, rightSpeedErrorX = 0;
	static int32_t leftOldSpeedErrorX = 0, rightOldSpeedErrorX = 0;

	getEncoderStatus();

	leftOldSpeedErrorX = leftSpeedErrorX;
	leftSpeedErrorX = targetSpeedX - leftEncoderChange;
	leftSpeedPwmX = 30 * leftSpeedErrorX + 1 * (leftSpeedErrorX - leftOldSpeedErrorX);

	rightOldSpeedErrorX = rightSpeedErrorX;
	rightSpeedErrorX = targetSpeedX - rightEncoderChange;
	rightSpeedPwmX = 30 * rightSpeedErrorX + 1 * (rightSpeedErrorX - rightOldSpeedErrorX);

	setMotores(leftSpeedPwmX + targetSpeedW, rightSpeedPwmX - targetSpeedW);
}

void controlMotorPwm(void)
{
	int32_t leftSpeedPwmX, rightSpeedPwmX, speedPwmX;
	static int32_t leftSpeedErrorX = 0, rightSpeedErrorX = 0, speedErrorX = 0;
	static int32_t leftOldSpeedErrorX = 0, rightOldSpeedErrorX = 0, oldSpeedErrorX = 0;

	getEncoderStatus();

	leftOldSpeedErrorX = leftSpeedErrorX;
	leftSpeedErrorX = targetSpeedX - leftEncoderChange;
	leftSpeedPwmX = 10 * leftSpeedErrorX + 1 * (leftSpeedErrorX - leftOldSpeedErrorX);

	rightOldSpeedErrorX = rightSpeedErrorX;
	rightSpeedErrorX = targetSpeedX - rightEncoderChange;
	rightSpeedPwmX = 10 * rightSpeedErrorX + 1 * (rightSpeedErrorX - rightOldSpeedErrorX);

	oldSpeedErrorX = speedErrorX;
	speedErrorX = targetSpeedX - ((leftEncoderChange + rightEncoderChange) / 2);
	speedPwmX = 20 * speedErrorX + 1 * (speedErrorX - oldSpeedErrorX);

	setMotores(speedPwmX + leftSpeedPwmX + targetSpeedW, speedPwmX + rightSpeedPwmX - targetSpeedW);
}
