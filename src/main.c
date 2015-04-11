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

#define ESQUERDA 1
#define DIREITA 2

#define N_TRECHOS 8
#define N_PARAMETROS 33

#define TRATAMENTO_GERAL
//#define TRATAMENTO_POS_RAMPA

#define ENABLE_DESVIO
#define ENABLE_RAMPA
#define ENABLE_LOOP

#define CASO 1	// AO MUDAR O CASO MUDAR TAMBÉM AS DISTÂNCIAS !!!!!!!!!!
//#define DEBUG_PRINTS

int32_t erro = 0, erro_a = 0;
int32_t dist_percorrida = 0;
int32_t dist_aux = 0;
bool run = false;
uint32_t trecho = 0, curva_90 = 0, curva_90_aux = 0;
uint32_t desv_cnt = 0, rampa_cnt = 0, loop_cnt = 0;

uint32_t param_speedX_med, param_speedX_min, param_speedX_max;
int32_t param_pid_kp, param_pid_ki, param_pid_kd, param_pid_offset;
uint32_t param_desv_d0, param_desv_sX, param_desv_sW;
uint32_t param_rampa_d1, param_rampa_d2, param_rampa_d3, param_rampa_d4, param_rampa_t1;
uint32_t param_90_v1, param_90_v2, param_90_cnt, param_90_erro;
uint32_t param_loop_d1, param_loop_d2, param_loop_d3, param_loop_d4, param_loop_d5;

uint32_t desvio_d[N_TRECHOS] = {0};
int32_t desvio_sW[N_TRECHOS] = {0};



/**
  * @brief Programa Principal
  */
int main(void)
{
	uint32_t buf[N_PARAMETROS];
	uint32_t rx_param = 0;

	// Inicialização e configuração dos periféricos
	HAL_Init();
	SystemClock_Config();

	buzzerConfig();
	ledsConfig();
	motoresConfig();
	botaoConfig();
	sensoresConfig();
	encodersConfig();
	usart1Config();

#ifdef TRATAMENTO_POS_RAMPA
	desv_cnt = 2;
	rampa_cnt = 3;
#endif

	// Mensagem de inicialização
	printf("Programa TUR5 - uMaRT LITE+ V1.1\r\n");
	delay_ms(100);

	// Leitura da Bateria, gera um alerta quando a tensão for menor que 7,00V
	if(getTensao() > VBAT_ALERTA)
	{
		printf("Bateria: %d mV\r\n", getTensao());
		beeps(1, 50, 50);
		//allLEDs(HIGH);
	}
	else
	{
		printf("Bateria BAIXA!\r\n");
		beeps(5, 50, 50);
		beep(500);
		while(1) delay_ms(1000);
	}


	// Inicialização dos parâmetros guardados na Flash
	//parametros_default();	// Tirar o comentário para carregar os valores padrões
	init_parametros();

	HAL_UART_Receive_IT(&huart1, &RxByte, 1);
	// Loop enquanto o botão de start não é pressionado
	// Neste momento que é realizada a leitura/gravação dos parâmetros do robô
	while(getSW1() == LOW)
	{
		if(rx_available > 0)
		{
			printf("%s\n", RxBuffer);

			if (strcmp(RxBuffer, "GET\n") == 0)
			{
				//printf("Envia os parametros ao celular!\r\n");

				readFlash(buf, N_PARAMETROS);

				printf("%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d"
						"_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d\n",
						buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6],
						buf[7], buf[8], buf[9], buf[10], buf[11], buf[12],
						buf[13], buf[14], buf[15], buf[16], buf[17], buf[18],
						buf[19], buf[20], buf[21], buf[22], buf[23], buf[24],
						buf[25], buf[26], buf[27], buf[28], buf[29], buf[30],
						buf[31], buf[32]);
			}
			else if (strcmp(RxBuffer, "SET\n") == 0)
			{
				printf("Recebe os parametros do celular!\r\n");

				rx_param = 1;
			}
			else if (rx_param > 0)
			{
				buf[rx_param - 1] = atoi(RxBuffer);
				rx_param++;

				if(rx_param == N_PARAMETROS + 1)
				{
					rx_param = 0;
					writeFlash(buf, N_PARAMETROS);
					init_parametros();
					printf("Parametros recebidos com sucesso!\r\n");
					beep(100);
				}
			}
			else
			{
				printf("ERRO\r\n");
			}

			rx_available = 0;
			memset(RxBuffer, 0, BUFFER_SIZE);
		}

		delay_ms(1);
	}

	//param_speedX_med = 1200;
	// Inicialização das variáveis de velocidade e aceleração
	targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX_med);
	targetSpeedW = 0;
	accX = 50; decX = 50;
	accW = 50; decW = 50;


	printf("Inicio em: %d us\r\n", micros());
	beep(200);
	delay_ms(1000);
	allLEDs(LOW);

	resetLeftEncCount();
	resetRightEncCount();
	speedProfileConfig();
	run = true;

	// Loop infinito ----------------------------------------------------------- LOOP PRINCIPAL DA MAIN
	while (1)
	{
		HAL_UART_Receive_IT(&huart1, &RxByte, 1);

		if(rx_available > 0)	// --------------------------------------------- STOP/START
		{
			printf("%s\n", RxBuffer);

			if (strcmp(RxBuffer, "STOP\n") == 0)
			{
				printf("Freia o robo!\r\n");
				run = false;
				setMotores(0, 0);
			}
			else if (strcmp(RxBuffer, "START\n") == 0)
			{
				printf("Reinicia o robo!\r\n");
				run = true;
				targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX_med);
			}
			else
			{
				printf("ERRO\r\n");
			}

			rx_available = 0;
			memset(RxBuffer, 0, BUFFER_SIZE);
		}


		if (distance_mm >= desvio_d[trecho] && (desv_cnt == 1))	// ------------- SPEEDPROFILE DO DESVIO
		{
			beep(50);
			trecho++;
			if(trecho < N_TRECHOS - 1)
			{
				targetSpeedX = SPEED_TO_COUNTS(2 * param_desv_sX);
				targetSpeedW = SPEED_TO_COUNTS(desvio_sW[trecho]);
			}
			else
			{
				// Ao acabar o desvio volta para o controle normal
				//targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX_med);
				desv_cnt = 2;
				rampa_cnt = 1;
				dist_aux = distance_mm;
				//setMotores(0, 0);
				erro = 0;
				run = true;
			}
		}


		if (curva_90 == ESQUERDA)	// ----------------------------------------- CURVAS DE 90
		{
			setMotores(-param_90_v1, param_90_v2);
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
			setMotores(param_90_v2, -param_90_v1);
			if ((getLeftEncCount() - curva_90_aux) > param_90_cnt)
			{
				erro = param_90_erro;
				run = true;
				curva_90 = 0;
				curva_90_aux = 0;
			}
		}

		if (rampa_cnt == 7)	// ------------------------------------------------- LOMBADA ELETRONICA
		{
			delay_ms(param_rampa_t1);
			targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX_max);	// _med
			run = true;
			rampa_cnt = 8;
		}

		delay_ms(1);
	}
}

// IRQ do Systick a cada 1ms --------------------------------------------------- SYSTICK
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


// IRQ do Timer de controle de velocidade e speedProfile ----------------------- TIMER DO CONTROLE
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

#ifdef TRATAMENTO_GERAL
	if (desv_cnt <= 1) tratamento_desvio();
	else if (rampa_cnt <= 8) tratamento_rampa(); /// ESSE 8 É TRETA
	else tratamento_loop();
#endif

#ifdef TRATAMENTO_POS_RAMPA
	if (rampa_cnt <= 8) tratamento_rampa();
	else tratamento_loop();
#endif


	ganhoX = SPEED_TO_COUNTS((2000 - abs(erro)) / 10);

	// Controlador PID
	if (run == true)
	{
		integral += erro;
		MV = (erro / param_pid_kp) + (integral / param_pid_ki) + ((erro - erro_a) * param_pid_kd);
		targetSpeedW = MV;
		controlMotorPwm();
	}


}


// Tratamento do desvio das barreiras ------------------------------------------ DESVIO DAS BARREIRAS
#ifdef ENABLE_DESVIO
void tratamento_desvio(void)
{
	// Melhora a reta da largada
	if ((distance_mm <= desvio_d[7]) && (desv_cnt == 0))
	{
		erro /= 3;
	}

	// Reseta o encoder (um pouco antes de acabar a linha) para habilitar o speedProfile()
	if ((distance_mm >= param_desv_d0) && (desv_cnt == 0))
	{
		dist_aux = distance_mm;
		run = false;
		printf("-- DESVIO --\r\n");
		beep(500);

		targetSpeedX = SPEED_TO_COUNTS(2 * param_desv_sX);
		//desvioW = SPEED_TO_COUNTS(2 * desvio_d[0]);
		desv_cnt = 1;

		resetSpeedProfile();
		targetSpeedX = SPEED_TO_COUNTS(2 * param_desv_sX);
		targetSpeedW = SPEED_TO_COUNTS(desvio_sW[trecho]);
	}

	// Executa o speedProfile() com os parâmetros do desvio
	if (desv_cnt == 1) speedProfile();
}
#endif


// Tratamento do trecho da rampa ----------------------------------------------- TRECHO DA RAMPA
#ifdef ENABLE_RAMPA
void tratamento_rampa(void)
{
	if (((distance_mm - dist_aux) >= param_rampa_d1) && (rampa_cnt == 1))
	{
		targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX_med);
		printf("-- INICIO DA RETA DA RAMPA --\r\n");
		beep(50);
		rampa_cnt = 2;
	}

	if (((distance_mm - dist_aux) >= param_rampa_d2) && (rampa_cnt == 2))
	{
		printf("-- PREPARA PARA CURVA DE 90 --\r\n");
		beep(50);
		rampa_cnt = 3;
	}

	// Deixa a rampa mais estável
	if (rampa_cnt == 2 || rampa_cnt == 3)
	{
		erro /= 3;
	}

	// 90 graus à esquerda, registra a distância de referência
	if ((readSpecial() == ESQUERDA_90) && (rampa_cnt == 3))
	{
		dist_aux = distance_mm;

		printf("-- 90 GRAUS A ESQUERDA --\r\n");
		beep(50);
		rampa_cnt = 4;

		curva_90 = ESQUERDA;
		curva_90_aux = getRightEncCount();
		run = false;
	}

	// Habilita um offset para garantir que o robô vire para esquerda na bifurcação
	if (((distance_mm - dist_aux) >= param_rampa_d3) && (rampa_cnt == 4))
	{
		printf("-- BIFURCACAO ESQUERDA --\r\n");
		beep(50);
		rampa_cnt = 5;
	}
	if(rampa_cnt == 5)
	{
		// Desloca o robô para esquerda durante um pequeno trecho
		erro -= 800;//param_pid_offset;
		if((distance_mm - dist_aux) >= (param_rampa_d3 + 1000))
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(50);
			rampa_cnt = 6;
		}
	}

	// Lombada eletrônica
	if (((distance_mm - dist_aux) >= param_rampa_d4) && (rampa_cnt == 6))
	{
		dist_aux = distance_mm;
		run = false;
		setMotores(0, 0);
		//targetSpeedX = SPEED_TO_COUNTS(2 * param_speedX_min);
		printf("-- LOMBADA ELETRONICA --\r\n");
		beep(50);
		rampa_cnt = 7;
	}

	// Só habilita leitura do T após esta distância logo após voltar da parada da lombada
	if (((distance_mm - dist_aux) >= 1500) && (rampa_cnt == 8))
	{
		rampa_cnt = 9;
	}
}
#endif


// Tratamento dos 3 casos diferentes do loop ----------------------------------- TRECHO FINAL
#ifdef ENABLE_LOOP
void tratamento_loop(void)
{
#if CASO == 1	// ------------------------------------------------------------- CASO 1
	// param_loop_d1: Distância entre o T e um pouco antes da bifurcação
	// param_loop_d2: Distância entre o T e um pouco depois da bifurcação
	// param_loop_d3: Distância entre o T e a parada final

	if ((rampa_cnt >= 7) && (loop_cnt <= 2))
	{
		erro /= 3;
	}

	// Lê o T, registra a distância e inicializa o contador para realizar o caso 1
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 0) && (rampa_cnt == 9))
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
		erro += param_pid_offset;
		if((distance_mm - dist_aux) >= param_loop_d2)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 3;
		}
	}

	// NÃO VOLTA AO NORMAL
	if(loop_cnt == 3)
	{
		erro += param_pid_offset;
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

#elif CASO == 2	// ------------------------------------------------------------- CASO 2
	// param_loop_d1: Distância entre o T e um pouco antes da curva de 90
	// param_loop_d2: Distância entre a 1a. curva de 90 à direita e um pouco antes da 2a.
	// param_loop_d3: Distância entre 2a. curva de 90 à direita e um pouco depois da bifurcação
	// param_loop_d4: Distância entre 2a. curva de 90 à direita e a parada final

	// 90 graus à esquerda, registra a distância e inicializa o contador para realizar o caso 2
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 0) && (rampa_cnt = 8))
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

	// Habilita um offset para garantir que o robô vire para esquerda na bifurcação
	if (((distance_mm - dist_aux) >= param_loop_d3) && (loop_cnt == 5))
	{
		printf("-- BIFURCACAO A ESQUERDA --\r\n");
		beep(50);
		loop_cnt = 6;
	}
	if(loop_cnt == 6)
	{
		// Desloca o robô para esquerda durante um pequeno trecho (d1 e d2)
		erro -= param_pid_offset;
		if((distance_mm - dist_aux) >= param_loop_d4)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 7;
		}
	}

	// Parada final na linha de chegada
	if (((distance_mm - dist_aux) >= param_loop_d5) && (loop_cnt == 7))
	{
		run = false;
		setMotores(0, 0);
		printf("-- FIM DO PERCURSO --\r\n");
		beep(500);
		loop_cnt = 8;
	}


#elif CASO == 3	// ------------------------------------------------------------- CASO 3
	// Lê o T, registra a distância e inicializa o contador para realizar o caso 3
	if ((readSpecial() == ESQUERDA_90) && (loop_cnt == 0) && (rampa_cnt == 8))
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
		erro -= param_pid_offset;
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
		erro += param_pid_offset;
		if((distance_mm - dist_aux) >= param_loop_d2)
		{
			printf("-- VOLTOU AO NORMAL --\r\n");
			beep(100);
			loop_cnt = 10;
		}
	}

	// NÃO VOLTA AO NORMAL
	if(loop_cnt == 10)
	{
		erro += param_pid_offset;
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
#endif


// Parâmetros da FLASH --------------------------------------------------------- FLASH
void init_parametros(void)
{
	uint32_t buf[N_PARAMETROS];
	readFlash(buf, N_PARAMETROS);

	param_speedX_med = buf[0];
	param_speedX_min = buf[1];
	param_speedX_max = buf[2];

	param_pid_kp = buf[3];
	param_pid_ki = buf[4];
	param_pid_kd = buf[5];
	param_pid_offset = buf[6];

	param_desv_d0 = buf[7];
	for(int32_t i = 0; i < N_TRECHOS; i++) desvio_d[i] = buf[i + 8];
	param_desv_sX = buf[17];
	param_desv_sW = buf[18];

	param_rampa_d1 = buf[19];
	param_rampa_d2 = buf[20];
	param_rampa_d3 = buf[21];
	param_rampa_d4 = buf[22];
	param_rampa_t1 = buf[23];

	param_90_v1 = buf[24];
	param_90_v2 = buf[25];
	param_90_cnt = buf[26];
	param_90_erro = buf[27];

	param_loop_d1 = buf[28];
	param_loop_d2 = buf[29];
	param_loop_d3 = buf[30];
	param_loop_d4 = buf[31];
	param_loop_d5 = buf[32];

	desvio_sW[0] = 0;
	desvio_sW[1] = 830;//param_desv_sW * 3; // param_desv_sW
	desvio_sW[2] = 0;
	desvio_sW[3] = -param_desv_sW;
	desvio_sW[4] = 0;
	desvio_sW[5] = param_desv_sW;
	desvio_sW[6] = 0;
	desvio_sW[7] = -param_desv_sW;
	if (N_TRECHOS == 9) desvio_sW[8] = 0;
}

void parametros_default(void)
{
	uint32_t buf[N_PARAMETROS];

	buf[0] = 1000;
	buf[1] = 800;
	buf[2] = 1200;

	buf[3] = 5;
	buf[4] = 100000;
	buf[5] = 2;
	buf[6] = 700;

	buf[7] = 2500;
	buf[8] = 50;
	buf[9] = 150;
	buf[10] = 550;
	buf[11] = 751;
	buf[12] = 1551;
	buf[13] = 1752;
	buf[14] = 2152;
	buf[15] = 2252;
	buf[16] = 2302;
	buf[17] = 500;
	buf[18] = 415;

	buf[19] = 0;
	buf[20] = 0;
	buf[21] = 0;
	buf[22] = 0;
	buf[23] = 0;

	buf[24] = 100;
	buf[25] = 500;
	buf[26] = 600;
	buf[27] = 0;

	buf[28] = 500;
	buf[29] = 2000;
	buf[30] = 1000;
	buf[31] = 1000;
	buf[32] = 2500;

	writeFlash(buf, N_PARAMETROS);
}
