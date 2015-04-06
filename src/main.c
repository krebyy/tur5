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

#define CASO 3
//#define DEBUG_PRINTS

int32_t erro = 0, erro_a = 0;
int32_t dist_percorrida = 0;
int32_t dist_aux = 0;
bool run = false;
uint32_t trecho = 0;
uint32_t loop_cnt = 0;
const int32_t dist[N_TRECHOS] = {50,	150,	550,	751,	1551,	1752,	2152,	2252,	2302};
const int32_t speedX[N_TRECHOS] = {800, 500, 500, 500, 500, 500, 500, 500, 500};
const int32_t speedW[N_TRECHOS] = {0, 415, 0, -415, 0, 415, 0, -415, 0};

// Caso 1:
//const uint32_t param_loop_d1 = 500, param_loop_d2 = 1000, param_loop_d3 = 2500, param_offset = 700;

// Caso 2:
//const uint32_t param_loop_d1 = 1000, param_loop_d2 = 1000, param_loop_d3 = 1000, param_loop_d4 = 2500, param_90_d1 = 100, param_90_erro = 2000;

// Caso 3:
const uint32_t param_loop_d1 = 500, param_loop_d2 = 2000, param_loop_d3 = 1000,
		param_loop_d4 = 1000, param_loop_d5 = 2500, param_offset = 700,
		param_90_d1 = 100, param_90_erro = 2000;

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
	if (erro == INFINITO)
	{
		erro = erro_a;
	}

	tratamento_loop();

	// Controlador PID
	integral += erro;
	MV = (erro / KP) + (integral / KI) + ((erro - erro_a) * KD);
	targetSpeedW = MV;
	if (run == true) controlMotorPwm();

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


void tratamento_desvio(void)
{

}

void tratamento_posRampa(void)
{

}

void tratamento_loop(void)
{
#if CASO == 1
	// param_loop_d1: Distância entre o T e um pouco antes da bifurcação
	// param_loop_d2: Distância entre o T e um pouco depois da bifurcação
	// param_loop_d3: Distância entre o T e a parada final

	// Lê o T, registra a distância e inicializa o contador para realizar o caso 1
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 0))
	{
		erro = 0;
		dist_aux = distance_mm;

		printf("-- PASSOU PELO T --\r\n");
		beep(100);
		loop_cnt = 1;
	}

	// Habilita um offset para garantir que o robô vire para direita na bifurcação
	if (((distance_mm - dist_aux) >= param_loop_d1) && (loop_cnt == 1))
	{
		printf("-- BIFURCACAO DIREITA --\r\n");
		beep(100);
		loop_cnt = 2;
	}
	if(loop_cnt == 2)
	{
		// Desloca o robô para direita durante um pequeno trecho (d1 e d2)
		erro += param_offset;
		if((distance_mm - dist_aux) >= param_loop_d2)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 3;
		}
	}

	// Parada final na linha de chegada
	if (((distance_mm - dist_aux) >= param_loop_d3) && (loop_cnt == 3))
	{
		run = false;
		setMotores(0, 0);
		printf("-- FIM DO PERCURSO --\r\n");
		beep(500);
		targetSpeedX = 0;
		loop_cnt = 4;
	}

#elif CASO == 2
	// param_loop_d1: Distância entre o T e um pouco antes da curva de 90
	// param_loop_d2: Distância entre a 1a. curva de 90 à direita e um pouco antes da 2a.
	// param_loop_d3: Distância entre 2a. curva de 90 à direita e um pouco depois da bifurcação
	// param_loop_d4: Distância entre 2a. curva de 90 à direita e a parada final

	// 90 graus à esquerda, registra a distância e inicializa o contador para realizar o caso 2
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 0))
	{
		dist_aux = distance_mm;

		printf("-- 90 GRAUS A ESQUERDA --\r\n");
		beep(100);
		loop_cnt = 1;
	}
	if (loop_cnt == 1)
	{
		if(((distance_mm - dist_aux) <= param_90_d1)) erro = -param_90_erro;
		else if ((distance_mm - dist_aux) >= param_loop_d1)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 2;
		}
	}

	// Primeira curva de 90 graus à direita
	if ((readSpecial() == DIREITA_90) && (loop_cnt == 2))
	{
		dist_aux = distance_mm;

		printf("-- 1a. 90 GRAUS A DIREITA --\r\n");
		beep(100);
		loop_cnt = 3;
	}
	if (loop_cnt == 3)
	{
		if(((distance_mm - dist_aux) <= param_90_d1)) erro = param_90_erro;
		else if ((distance_mm - dist_aux) >= param_loop_d2)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 4;
		}
	}

	// Segunda curva de 90 graus à direita
	if ((readSpecial() == DIREITA_90) && (loop_cnt == 4))
	{
		dist_aux = distance_mm;

		printf("-- 2a. 90 GRAUS A DIREITA --\r\n");
		beep(100);
		loop_cnt = 5;
	}
	if (loop_cnt == 5)
	{
		if(((distance_mm - dist_aux) <= param_90_d1)) erro = param_90_erro;
		else if ((distance_mm - dist_aux) >= param_loop_d3)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 6;
		}
	}

	// Parada final na linha de chegada
	if (((distance_mm - dist_aux) >= param_loop_d4) && (loop_cnt == 6))
	{
		run = false;
		setMotores(0, 0);
		printf("-- FIM DO PERCURSO --\r\n");
		beep(500);
		loop_cnt = 7;
	}


#elif CASO == 3
	// Lê o T, registra a distância e inicializa o contador para realizar o caso 3
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 0))
	{
		erro = 0;
		dist_aux = distance_mm;

		printf("-- PASSOU PELO T --\r\n");
		beep(100);
		loop_cnt = 1;
	}

	// Habilita um offset para garantir que o robô vire para esquerda na bifurcação
	if (((distance_mm - dist_aux) >= param_loop_d1) && (loop_cnt == 1))
	{
		printf("-- BIFURCACAO ESQUERDA --\r\n");
		beep(100);
		loop_cnt = 2;
	}
	if(loop_cnt == 2)
	{
		// Desloca o robô para esquerda durante um pequeno trecho (d1 e d2)
		erro -= param_offset;
		if((distance_mm - dist_aux) >= param_loop_d2)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 3;
		}
	}

	// Primeira curva de 90 graus à esquerda
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 3))
	{
		dist_aux = distance_mm;

		printf("-- 1a. 90 GRAUS A ESQUERDA --\r\n");
		beep(100);
		loop_cnt = 4;
	}
	if (loop_cnt == 4)
	{
		if(((distance_mm - dist_aux) <= param_90_d1)) erro = -param_90_erro;
		else if ((distance_mm - dist_aux) >= param_loop_d3)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 5;
		}
	}

	// Segunda curva de 90 graus à esquerda
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 5))
	{
		dist_aux = distance_mm;

		printf("-- 2a. 90 GRAUS A ESQUERDA --\r\n");
		beep(100);
		loop_cnt = 6;
	}
	if (loop_cnt == 6)
	{
		if(((distance_mm - dist_aux) <= param_90_d1)) erro = -param_90_erro;
		else if ((distance_mm - dist_aux) >= param_loop_d4)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 7;
		}
	}

	// Curva de 90 graus à esquerda no T
	if ((readSpecial() == AMBOS_90) && (loop_cnt == 7))
	{
		dist_aux = distance_mm;

		printf("-- 90 GRAUS A ESQUERDA NO T --\r\n");
		beep(100);
		loop_cnt = 8;
	}
	if (loop_cnt == 8)
	{
		if(((distance_mm - dist_aux) <= param_90_d1)) erro = -param_90_erro;
		else if ((distance_mm - dist_aux) >= param_loop_d1)
		{
			printf("-- BIFURCACAO DIREITA --\r\n");
			beep(100);
			loop_cnt = 9;
		}
	}

	// Habilita um offset para garantir que o robô vire para direita na bifurcação
	if(loop_cnt == 9)
	{
		// Desloca o robô para direita durante um pequeno trecho (d1 e d2)
		erro += param_offset;
		if((distance_mm - dist_aux) >= param_loop_d2)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 10;
		}
	}

	// Parada final na linha de chegada
	if (((distance_mm - dist_aux) >= param_loop_d5) && (loop_cnt == 10))
	{
		run = false;
		setMotores(0, 0);
		printf("-- FIM DO PERCURSO --\r\n");
		beep(500);
		targetSpeedX = 0;
		loop_cnt = 11;
	}
#endif
}
