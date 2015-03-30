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

#define KP 5
#define KD 1
#define PWM_MEDIO 300
int32_t erro = 0, erro_a = 0;
bool run = false;

/**
  * @brief Programa Principal
  */
int main(void)
{
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


	printf("Programa DEMO - uMaRT LITE+ V1.1\r\n");
	beeps(3, 50, 50);
	allLEDs(HIGH);
	while(getSW1() == LOW);
	beep(500);
	delay_ms(1000);
	allLEDs(LOW);
	setMotores(PWM_MEDIO, PWM_MEDIO);
	run = true;

	printf("Microsegundos: %d\r\n", micros());

	resetLeftEncCount();
	resetRightEncCount();
	// Loop infinito
	while (1)
	{
		//setMotores(100, 500);
		/*delay_ms(1);
		int32_t erro = getSensoresLinha();
		allLEDs(LOW);
		if(erro > 0 && erro != INFINITO) setLED(LED3, HIGH);
		else if(erro < 0) setLED(LED1, HIGH);
		else if(erro == 0) setLED(LED2, HIGH);
		else allLEDs(HIGH);*/
		//printf("Microsegundos: %d\r\n", micros());
		//setMotores(0, 0);
		//delay_us(1000005);
		//printf("Erro: %d\r\n", getSensoresLinha());
		delay_ms(1000);
		printf("Left: %d | Right: %d\r\n", getLeftEncCount(), getRightEncCount());
		printf("RAW Bateria: %d", getTensao());
	}
}


void systick(void)
{
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
}
