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
#define KI 100000
#define KD 2
#define PWM_MEDIO 350

#define N_TRECHOS 9

//#define DEBUG_PRINTS

int32_t erro = 0, erro_a = 0;
int32_t dist_percorrida = 0;
bool run = false;
uint32_t trecho = 0;
const int32_t dist[N_TRECHOS] = {50,	150,	550,	751,	1551,	1752,	2152,	2252,	2302};
const int32_t speedX[N_TRECHOS] = {800, 500, 500, 500, 500, 500, 500, 500, 500};
const int32_t speedW[N_TRECHOS] = {0, 415, 0, -415, 0, 415, 0, -415, 0};

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


	targetSpeedX = SPEED_TO_COUNTS(2 * speedX[trecho]);
	targetSpeedW = SPEED_TO_COUNTS(speedW[trecho]);
	accX = 100;
	decX = 100;
	accW = 50;
	decW = 50;

	printf("Programa TUR5 - uMaRT LITE+ V1.1\r\n");
	delay_ms(100);

	// Leitura da Bateria, gera um alerta quando a tensão for menor que 7,00V
	if(getTensao() > VBAT_ALERTA)
	{
		printf("Bateria: %d mV\r\n", getTensao());
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

	// Loop enquanto o botão de start não é pressionado
	// Neste momento que é realizada a leitura/gravação dos parâmetros do robô
	while(getSW1() == LOW)
	{
		if(HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY)
		{
			HAL_UART_Receive_IT(&huart1, &RxByte, 1);
		}

		if(rx_available > 0)
		{
			printf("%s\n", RxBuffer);

			if (strcmp(RxBuffer, "GET\r") == 0)
			{
				printf("Envia os parametros ao celular!\r\n");
			}
			else if (strcmp(RxBuffer, "SET\r") == 0)
			{
				printf("Recebe os parametros do celular!\r\n");
			}
			else
			{
				printf("ERRO\r\n");
			}

			rx_available = 0;
			memset(RxBuffer, 0, BUFFER_SIZE);
		}

		//writeFlash(dist, N_TRECHOS);
		//uint32_t buf[N_TRECHOS];
		//readFlash(buf, N_TRECHOS);
	}


	printf("Inicio em: %d us\r\n", micros());
	beep(200);
	delay_ms(1000);
	allLEDs(LOW);

	resetLeftEncCount();
	resetRightEncCount();
	speedProfileConfig();
	run = true;

	// Loop infinito
	while (1)
	{
		if(HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY)
		{
			HAL_UART_Receive_IT(&huart1, &RxByte, 1);
		}

		if(rx_available > 0)
		{
			printf("%s\n", RxBuffer);

			if (strcmp(RxBuffer, "STOP\r") == 0)
			{
				printf("Freia o robo!\r\n");
			}
			else if (strcmp(RxBuffer, "START\r") == 0)
			{
				printf("Reinicia o robo!\r\n");
			}
			else
			{
				printf("ERRO\r\n");
			}

			rx_available = 0;
			memset(RxBuffer, 0, BUFFER_SIZE);
		}
	}
}


void systick(void)
{
#ifdef DEBUG_PRINTS
	static uint32_t ticks = 0;

	if (++ticks >= 1000 && run == true)
	{
		printf("Erro: %d\r\n", erro);
		printf("Encoders: %d | %d\r\n", getLeftEncCount(), getRightEncCount());
		printf("Bateria: %d mV\r\n", getTensao());
		printf("Gyro: %d\r\n", getGyro());
		printf("Distancia percorrida: %d mm\r\n", COUNTS_TO_MM(getEncoderStatus()));
		printf("\r\n");

		ticks = 0;
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
	int32_t MV = 0;
	static int32_t integral = 0;

	// Leitura dos sensores e indicação pelos LEDs
	erro_a = erro;
	erro = readLine();
	allLEDs(LOW);
	if (erro > 0 && erro != INFINITO) setLED(LED3, HIGH);
	else if (erro < 0) setLED(LED1, HIGH);
	else if (erro == 0) setLED(LED2, HIGH);
	else allLEDs(HIGH);

	// Tratamento para quando não ler linha
	if(erro == INFINITO)
	{
		erro = erro_a;
	}

	// Controlador PID
	integral += erro;
	MV = (erro / KP) + (integral / KI) + ((erro - erro_a) * KD);
	targetSpeedW = MV;
	controlMotorPwm();

	//dist_percorrida = speedProfile(MV);

	/*if(dist_percorrida >= dist[trecho] && run == true)
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
			run = false;
		}
	}*/
}
