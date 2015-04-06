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

#define ESQUERDA 1
#define DIREITA 2

#define N_TRECHOS 8

#define CASO 3
//#define DEBUG_PRINTS

int32_t erro = 0, erro_a = 0;
int32_t dist_percorrida = 0;
int32_t dist_aux = 0;
bool run = false;
uint32_t trecho = 0, curva_90 = 0, curva_90_aux = 0;
uint32_t desv_cnt = 0, loop_cnt = 0;
const int32_t dist[N_TRECHOS] = {50,	150,	550,	751,	1551,	1752,	2152,	2252,	2302};
const int32_t speedX[N_TRECHOS] = {500, 500, 500, 500, 500, 500, 500, 500, 500};
const int32_t speedW[N_TRECHOS] = {0, 415, 0, -415, 0, 415, 0, -415, 0};

const int32_t param_speedX = 800;

// Desvio
const uint32_t param_desv_d1 = 2500;

// Caso 1:
//const uint32_t param_loop_d1 = 500, param_loop_d2 = 1000, param_loop_d3 = 2500, param_offset = 300;

// Caso 2:
//const uint32_t param_loop_d1 = 1000, param_loop_d2 = 1000, param_loop_d3 = 1000, param_loop_d4 = 2500;

// Caso 3:
const uint32_t param_loop_d1 = 500, param_loop_d2 = 2000, param_loop_d3 = 1000,
		param_loop_d4 = 1000, param_loop_d5 = 2500, param_offset = 700;


const int32_t param_90_v1 = -100, param_90_v2 = 500, param_90_cnt = 600, param_90_erro = 0;

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


	targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX);
	targetSpeedW = SPEED_TO_COUNTS(speedW[trecho]);
	accX = 50;
	decX = 50;
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

		if (distance_mm >= dist[trecho] && (desv_cnt == 1))
		{
			beep(50);
			trecho++;
			if(trecho < N_TRECHOS)
			{
				targetSpeedX = SPEED_TO_COUNTS(2 * speedX[trecho]);
				targetSpeedW = SPEED_TO_COUNTS(speedW[trecho]);
			}
			else
			{
				// Ao acabar o desvio volta para o controle normal
				targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX);
				desv_cnt = 2;
				run = true;
			}
		}

		if (curva_90 == ESQUERDA)
		{
			setMotores(param_90_v1, param_90_v2);
			if ((getRightEncCount() - curva_90_aux) > param_90_cnt)
			{
				erro = -param_90_erro;
				run = true;
				curva_90 = 0;
				curva_90_aux = 0;
			}
		}
		else if (curva_90 == DIREITA)
		{
			setMotores(param_90_v2, param_90_v1);
			if ((getLeftEncCount() - curva_90_aux) > param_90_cnt)
			{
				erro = param_90_erro;
				run = true;
				curva_90 = 0;
				curva_90_aux = 0;
			}
		}

		delay_ms(1);
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
		printf("Distacia do obstaculo: %d\r\n", readWall());
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

	if (desv_cnt <= 1) tratamento_desvio();
	//tratamento_loop();

	// Controlador PID
	if (run == true)
	{
		integral += erro;
		MV = (erro / KP) + (integral / KI) + ((erro - erro_a) * KD);
		targetSpeedW = MV;
		controlMotorPwm();
	}


}


void tratamento_desvio(void)
{
	// Reseta o encoder (um pouco antes de acabar a linha) para habilitar o speedProfile()
	if ((distance_mm >= param_desv_d1) && (desv_cnt == 0))
	{
		run = false;
		printf("-- DESVIO --\r\n");
		beep(500);
		desv_cnt = 1;

		resetSpeedProfile();
		targetSpeedX = SPEED_TO_COUNTS(2 * speedX[trecho]);
		targetSpeedW = SPEED_TO_COUNTS(speedW[trecho]);
	}

	// Executa o speedProfile() com os parâmetros do desvio
	if (desv_cnt == 1) speedProfile();
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
		beep(50);
		loop_cnt = 1;

		curva_90 = ESQUERDA;
		curva_90_aux = getRightEncCount();
		run = false;
	}
	if (((distance_mm - dist_aux) >= param_loop_d1) && (loop_cnt == 1))
	{
		printf("-- VOLTOU AO NORMAL --\r\n");
		beep(50);
		loop_cnt = 2;
	}

	// Primeira curva de 90 graus à direita
	if ((readSpecial() == DIREITA_90) && (loop_cnt == 2))
	{
		dist_aux = distance_mm;

		printf("-- 1a. 90 GRAUS A DIREITA --\r\n");
		beep(50);
		loop_cnt = 3;

		curva_90 = DIREITA;
		curva_90_aux = getLeftEncCount();
		run = false;
	}
	if (((distance_mm - dist_aux) >= param_loop_d2) && (loop_cnt == 3))
	{
		printf("-- VOLTOU AO NORMAL --\r\n");
		beep(50);
		loop_cnt = 4;
	}

	// Segunda curva de 90 graus à direita
	if ((readSpecial() == DIREITA_90) && (loop_cnt == 4))
	{
		dist_aux = distance_mm;

		printf("-- 2a. 90 GRAUS A DIREITA --\r\n");
		beep(50);
		loop_cnt = 5;

		curva_90 = DIREITA;
		curva_90_aux = getLeftEncCount();
		run = false;
	}
	if (((distance_mm - dist_aux) >= param_loop_d3) && (loop_cnt == 5))
	{
		printf("-- VOLTOU AO NORMAL --\r\n");
		beep(50);
		loop_cnt = 6;
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

		curva_90 = ESQUERDA;
		curva_90_aux = getRightEncCount();
		run = false;
	}
	if (((distance_mm - dist_aux) >= param_loop_d3) && (loop_cnt == 4))
	{
		printf("-- VOLTOU AO NORMAL --\r\n");
		beep(100);
		loop_cnt = 5;
	}

	// Segunda curva de 90 graus à esquerda
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 5))
	{
		dist_aux = distance_mm;

		printf("-- 2a. 90 GRAUS A ESQUERDA --\r\n");
		beep(100);
		loop_cnt = 6;

		curva_90 = ESQUERDA;
		curva_90_aux = getRightEncCount();
		run = false;
	}
	if (((distance_mm - dist_aux) >= param_loop_d4) && (loop_cnt == 6))
	{
		printf("-- VOLTOU AO NORMAL --\r\n");
		beep(100);
		loop_cnt = 7;
	}

	// Curva de 90 graus à esquerda no T
	if ((readSpecial() == AMBOS_90) && (loop_cnt == 7))
	{
		dist_aux = distance_mm;

		printf("-- 90 GRAUS A ESQUERDA NO T --\r\n");
		beep(100);
		loop_cnt = 8;

		curva_90 = ESQUERDA;
		curva_90_aux = getRightEncCount();
		run = false;
	}
	if (((distance_mm - dist_aux) >= param_loop_d1) && (loop_cnt == 8))
	{
		printf("-- BIFURCACAO DIREITA --\r\n");
		beep(100);
		loop_cnt = 9;
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
