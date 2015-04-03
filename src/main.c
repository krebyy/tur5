/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/src/main.c
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    16-Fevereiro-2015
  * @brief   Programa principal de demonstração uMaRT LITE+ V1.1
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define KP 10
#define KD 4
#define PWM_MEDIO 350

#define N_TRECHOS 9
//#define PID
int32_t erro = 0, erro_a = 0;
int32_t dist_percorrida = 0;

/**
  * @brief Programa Principal
  */
int main(void)
{
	uint32_t trecho = 0;
	int32_t dist[N_TRECHOS] = {50, 129, 199, 356, 580, 737, 807, 886, 936};
	int32_t speedX[N_TRECHOS] = {500, 500, 500, 500, 500, 500, 500, 500, 500};
	int32_t speedW[N_TRECHOS] = {0, 415, 0, -415, 0, 415, 0, -415, 0};

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock to 168 MHz */
	SystemClock_Config();

	buzzerConfig();
	ledsConfig();
	motoresConfig();
	botaoConfig();
	sensoresConfig();
	encodersConfig();
	usart1Config();


	targetSpeedX = SPEED_TO_COUNTS(2 * speedX[trecho]);
	targetSpeedW = SPEED_TO_COUNTS(speedW[trecho]);
	accX = 50;
	decX = 50;
	accW = 50;
	decW = 50;


	printf("Programa DEMO - uMaRT LITE+ V1.1\r\n");
	if(getTensao() > VBAT_ALERTA)
	{
		beeps(1, 50, 50);
		allLEDs(HIGH);
	}
	else
	{
		printf("Bateria BAIXA!\r\n");
		beeps(5, 50, 50);
		beep(500);
		while(1) delay_ms(1000);
	}
	while(getSW1() == LOW);
	beep(200);
	delay_ms(1000);
	allLEDs(LOW);
	//setMotores(PWM_MEDIO, PWM_MEDIO);

	printf("Microsegundos: %d\r\n", micros());

	resetLeftEncCount();
	resetRightEncCount();
	speedProfileConfig();

	// Loop infinito
	while (1)
	{
		//setMotores(100, 500);
		delay_ms(100);
		//int32_t erro = getSensoresLinha();
		int32_t erro = readLine();
		printf("Error: %d\r\n", erro);
		allLEDs(LOW);
		if(erro > 0 && erro != INFINITO) setLED(LED3, HIGH);
		else if(erro < 0) setLED(LED1, HIGH);
		else if(erro == 0) setLED(LED2, HIGH);
		else allLEDs(HIGH);
		//printf("Microsegundos: %d\r\n", micros());
		//setMotores(0, 0);
		//delay_us(1000005);
		/*delay_ms(1000);
		printf("Error: %d\r\n", getSensoresLinha());
		printf("Left: %d | Right: %d\r\n", getLeftEncCount(), getRightEncCount());
		printf("RAW Battery: %d\r\n", getTensao());
		printf("RAW Gyro: %d\r\n", getGyro());*/
		//printf("Distance Left: %d\r\n", speedProfile());

		/*if(dist_percorrida >= dist[trecho])
		{
			toggleLED(LED4);
			trecho++;
			if(trecho < N_TRECHOS)
			{
				targetSpeedX = SPEED_TO_COUNTS(2 * speedX[trecho]);
				targetSpeedW = SPEED_TO_COUNTS(speedW[trecho]);
			}
			else
			{
				targetSpeedX = 0;
				targetSpeedW = 0;
				while(1) delay_ms(1000);
			}
		}
		delay_ms(1);*/
	}
}


void systick(void)
{
#ifdef PID
	if(run == true)
	{
		int32_t MV = 0;

		erro_a = erro;
		erro = getSensoresLinha();
		allLEDs(LOW);
		if(erro > 0 && erro != INFINITO) setLED(LED3, HIGH);
		else if(erro < 0) setLED(LED1, HIGH);
		else if(erro == 0) setLED(LED2, HIGH);
		else allLEDs(HIGH);

		if(erro == INFINITO) erro = erro_a;
		MV = (erro * KP) + ((erro - erro_a) * KD);

		setMotores(PWM_MEDIO + MV, PWM_MEDIO - MV);

	}
#endif
}


/**
  * @brief  Period elapsed callback in non blocking mode - 5ms
  * @param  htim: TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//dist_percorrida = speedProfile();
}
