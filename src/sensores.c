/**
  ******************************************************************************
  * @file    umart/lite_plus_v1/src/sensores.c
  * @author  Kleber Lima da Silva (kleber.ufu@hotmail.com)
  * @version V1.0.0
  * @date    17-Fevereiro-2015
  * @brief   Funções para acionamento do Botão SW1
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sensores.h"


/** @defgroup Variáveis privadas
  * @{
  */
GPIO_TypeDef* EMISSORES_PORT[N_EMISSORES] =
			{LF_E_PORT, POWER1_PORT, POWER2_PORT, RF_E_PORT,
			L_LINE_PORT, R_LINE_PORT};
const uint16_t EMISSORES_PIN[N_EMISSORES] =
			{LF_E_PIN, POWER1_PIN, POWER2_PIN, RF_E_PIN,
			L_LINE_PIN, R_LINE_PIN};

GPIO_TypeDef* RECEPTORES_PORT[N_RECEPTORES] =
			{LINE1_PORT, LINE2_PORT, LINE3_PORT, LINE4_PORT,
			LINE5_PORT, LINE6_PORT, LINE7_PORT, LINE8_PORT};
const uint16_t RECEPTORES_PIN[N_RECEPTORES] =
			{LINE1_PIN, LINE2_PIN, LINE3_PIN, LINE4_PIN,
			LINE5_PIN, LINE6_PIN, LINE7_PIN, LINE8_PIN};

GPIO_TypeDef* ANALOGICAS_PORT[N_ANALOGICAS] =
			{LF_R_PORT, L_R_PORT, R_R_PORT, RF_R_PORT,
			G_OUTZ_PORT, G_VREF_PORT, VBAT_PORT};
const uint16_t ANALOGICAS_PIN[N_ANALOGICAS] =
			{LF_R_PIN, L_R_PIN, R_R_PIN, RF_R_PIN,
			G_OUTZ_PIN, G_VREF_PIN, VBAT_PIN};
/**
  * @}
  */


ADC_HandleTypeDef hadc1;


/**
  * @brief Configuração dos GPIOs e ADCs dos sensores
  * @param Nenhum
  * @return Nenhum
  */
void sensoresConfig(void)
{
	SENSORES_CLK;	// Habilita o barramento de clock do GPIO dos Sensores
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_ChannelConfTypeDef sConfig;

	// Configura os GPIOs dos emissores como saída push/pull
	for(int i = 0; i < N_EMISSORES; i++)
	{
		GPIO_InitStructure.Pin = EMISSORES_PIN[i];;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
		HAL_GPIO_Init(EMISSORES_PORT[i], &GPIO_InitStructure);
		HAL_GPIO_WritePin(EMISSORES_PORT[i], EMISSORES_PIN[i], LOW);
	}


	// Configura os GPIOs dos receptores como entrada sem resistor interno
	for(int i = 0; i < N_RECEPTORES; i++)
	{
		GPIO_InitStructure.Pin = RECEPTORES_PIN[i];
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(RECEPTORES_PORT[i], &GPIO_InitStructure);
	}


	// Configura os pinos analógicos
	for(int i = 0; i < N_ANALOGICAS; i++)
	{
		GPIO_InitStructure.Pin = ANALOGICAS_PIN[i];
		GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStructure.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(ANALOGICAS_PORT[i], &GPIO_InitStructure);
	}


	__ADC1_CLK_ENABLE();
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION12b;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = EOC_SINGLE_CONV;
	HAL_ADC_Init(&hadc1);
}


/**
  * @brief Verifca os sensores de parede
  * @param lf Valor proporcional a distância do sensor frontal esquerdo
  * @param l Valor proporcional a distância do sensor diagonal esquerdo
  * @param r Valor proporcional a distância do sensor diagonal direito
  * @param rf Valor proporcional a distância do sensor frontal direito
  * @return paredes
  */
int32_t getSensoresParede(int32_t* lf, int32_t* l, int32_t* r, int32_t* rf)
{
	int32_t paredes = 0;

	(*lf) = getRawADC(LF_R_CH);
	(*l) = getRawADC(L_R_CH);
	(*r) = getRawADC(R_R_CH);
	(*rf) = getRawADC(RF_R_CH);

	// Registra o tempo atual
	uint32_t t0 = micros();

	// Sensor frontal esquerdo
	HAL_GPIO_WritePin(LF_E_PORT, LF_E_PIN, HIGH);
	elapse_us(60, t0);
	(*lf) = getRawADC(LF_R_CH) - (*lf);
	HAL_GPIO_WritePin(LF_E_PORT, LF_E_PIN, LOW);
	if((*lf) < 0) (*lf) = 0;
	elapse_us(140, t0);

	// Sensor frontal direito
	HAL_GPIO_WritePin(RF_E_PORT, RF_E_PIN, HIGH);
	elapse_us(200, t0);
	(*rf) = getRawADC(RF_R_CH) - (*rf);
	HAL_GPIO_WritePin(RF_E_PORT, RF_E_PIN, LOW);
	if((*rf) < 0) (*rf) = 0;
	elapse_us(280, t0);

	// Sensores diagonais
	HAL_GPIO_WritePin(POWER1_PORT, POWER1_PIN, HIGH);
	elapse_us(340, t0);
	(*l) = getRawADC(L_R_CH) - (*l);
	(*r) = getRawADC(R_R_CH) - (*r);
	HAL_GPIO_WritePin(POWER1_PORT, POWER1_PIN, LOW);
	if((*l) < 0) (*l) = 0;
	if((*r) < 0) (*r) = 0;


	return paredes;
}


/**
  * @brief Verifca os sensores frontais de parede
  * @return wall
  */
int32_t readWall()
{
	int32_t wall = 0;

	int32_t lf = getRawADC(LF_R_CH);
	int32_t rf = getRawADC(RF_R_CH);

	// Registra o tempo atual
	uint32_t t0 = micros();

	// Sensor frontal esquerdo
	HAL_GPIO_WritePin(LF_E_PORT, LF_E_PIN, HIGH);
	elapse_us(80, t0);
	lf = getRawADC(LF_R_CH) - lf;
	HAL_GPIO_WritePin(LF_E_PORT, LF_E_PIN, LOW);
	if(lf < 0) lf = 0;
	elapse_us(160, t0);

	// Sensor frontal direito
	HAL_GPIO_WritePin(RF_E_PORT, RF_E_PIN, HIGH);
	elapse_us(240, t0);
	rf = getRawADC(RF_R_CH) - rf;
	HAL_GPIO_WritePin(RF_E_PORT, RF_E_PIN, LOW);
	if(rf < 0) rf = 0;

	wall = (lf + rf) / 2;

	return wall;
}

/**
  * @brief Verifica os sensores de linha
  * @param Nenhum
  * @return erro Valores negativos (delocado para direita), valores positivos
  * (deslocado para esquerda), INFINITO caso não tenha detectado linha
  */
int32_t getSensoresLinha()
{
	int32_t erro = 0, soma = 0, n = 0;
	uint32_t t0 = micros();

	// Habilita os emissores
	HAL_GPIO_WritePin(L_LINE_PORT, L_LINE_PIN, HIGH);
	HAL_GPIO_WritePin(R_LINE_PORT, R_LINE_PIN, HIGH);
	elapse_us(100, t0);

	// Realiza a leitura de todos os sensores de linha, os sensores das
	// extremidades pussuem peso maior, no final é realizada a média ponderada
	if(HAL_GPIO_ReadPin(LINE1_PORT, LINE1_PIN) == LINHA)
	{
		soma += -16;
		n++;
	}
	if(HAL_GPIO_ReadPin(LINE2_PORT, LINE2_PIN) == LINHA)
	{
		soma += -8;
		n++;
	}
	if(HAL_GPIO_ReadPin(LINE3_PORT, LINE3_PIN) == LINHA)
	{
		soma += -4;
		n++;
	}
	if(HAL_GPIO_ReadPin(LINE4_PORT, LINE4_PIN) == LINHA)
	{
		n++;
	}
	if(HAL_GPIO_ReadPin(LINE5_PORT, LINE5_PIN) == LINHA)
	{
		n++;
	}
	if(HAL_GPIO_ReadPin(LINE6_PORT, LINE6_PIN) == LINHA)
	{
		soma += 4;
		n++;
	}
	if(HAL_GPIO_ReadPin(LINE7_PORT, LINE7_PIN) == LINHA)
	{
		soma += 8;
		n++;
	}
	if(HAL_GPIO_ReadPin(LINE8_PORT, LINE8_PIN) == LINHA)
	{
		soma += 16;
		n++;
	}

	// Desabilita os emissores
	HAL_GPIO_WritePin(L_LINE_PORT, L_LINE_PIN, LOW);
	HAL_GPIO_WritePin(R_LINE_PORT, R_LINE_PIN, LOW);


	if(n != 0)
	{
		erro = soma / n;
	}
	else
	{
		erro = INFINITO;
	}

	return erro;
}


/**
  * @brief Realiza várias leituras dos sensores de linha e retorna a média
  * @param Nenhum
  * @return erro Valores negativos (delocado para direita), valores positivos
  * (deslocado para esquerda), INFINITO caso não tenha detectado linha
  */
int32_t readLine()
{
	int32_t erro = 0, soma = 0, n = 0;

	for(int i = 25; i <= 100; i += 5)
	{
		uint32_t t0 = micros();

		// Habilita os emissores
		HAL_GPIO_WritePin(L_LINE_PORT, L_LINE_PIN, HIGH);
		HAL_GPIO_WritePin(R_LINE_PORT, R_LINE_PIN, HIGH);
		elapse_us(i, t0);

		// Realiza a leitura de todos os sensores de linha, os sensores das
		// extremidades pussuem peso maior, no final é realizada a média ponderada
		if(HAL_GPIO_ReadPin(LINE1_PORT, LINE1_PIN) == LINHA)
		{
			soma += -2000;
			n++;
		}
		if(HAL_GPIO_ReadPin(LINE2_PORT, LINE2_PIN) == LINHA)
		{
			soma += -1333;
			n++;
		}
		if(HAL_GPIO_ReadPin(LINE3_PORT, LINE3_PIN) == LINHA)
		{
			soma += -667;
			n++;
		}
		if(HAL_GPIO_ReadPin(LINE4_PORT, LINE4_PIN) == LINHA)
		{
			soma += -167;
			n++;
		}
		if(HAL_GPIO_ReadPin(LINE5_PORT, LINE5_PIN) == LINHA)
		{
			soma += 167;
			n++;
		}
		if(HAL_GPIO_ReadPin(LINE6_PORT, LINE6_PIN) == LINHA)
		{
			soma += 667;
			n++;
		}
		if(HAL_GPIO_ReadPin(LINE7_PORT, LINE7_PIN) == LINHA)
		{
			soma += 1333;
			n++;
		}
		if(HAL_GPIO_ReadPin(LINE8_PORT, LINE8_PIN) == LINHA)
		{
			soma += 2000;
			n++;
		}

		// Desabilita os emissores
		HAL_GPIO_WritePin(L_LINE_PORT, L_LINE_PIN, LOW);
		HAL_GPIO_WritePin(R_LINE_PORT, R_LINE_PIN, LOW);
		elapse_us(i * 2, t0);
	}


	if(n != 0)
	{
		erro = soma / n;
	}
	else
	{
		erro = INFINITO;
	}

	return erro;
}


/**
  * @brief Realiza a leitura dos sensores para identificar os casos especiais
  * de curva de 90 graus, bifurcação e entroncamento
  * @param Nenhum
  * @return estado: ESQUERDA_90, DIREITA_90, AMBOS_90
  */
int32_t readSpecial()
{
	int32_t estado = 0;
	int32_t n_e = 0, n_d = 0;
	uint32_t t0 = micros();

	// Habilita os emissores
	HAL_GPIO_WritePin(L_LINE_PORT, L_LINE_PIN, HIGH);
	HAL_GPIO_WritePin(R_LINE_PORT, R_LINE_PIN, HIGH);
	elapse_us(100, t0);

	// Realiza a leitura de todos os sensores de linha, os sensores das
	// extremidades pussuem peso maior, no final é realizada a média ponderada
	if(HAL_GPIO_ReadPin(LINE1_PORT, LINE1_PIN) == LINHA) n_e += 10;
	if(HAL_GPIO_ReadPin(LINE2_PORT, LINE2_PIN) == LINHA) n_e++;
	if(HAL_GPIO_ReadPin(LINE3_PORT, LINE3_PIN) == LINHA) n_e++;
	if(HAL_GPIO_ReadPin(LINE4_PORT, LINE4_PIN) == LINHA) n_e++;
	if(HAL_GPIO_ReadPin(LINE5_PORT, LINE5_PIN) == LINHA) n_d++;
	if(HAL_GPIO_ReadPin(LINE6_PORT, LINE6_PIN) == LINHA) n_d++;
	if(HAL_GPIO_ReadPin(LINE7_PORT, LINE7_PIN) == LINHA) n_d++;
	if(HAL_GPIO_ReadPin(LINE8_PORT, LINE8_PIN) == LINHA) n_d += 10;

	// Desabilita os emissores
	HAL_GPIO_WritePin(L_LINE_PORT, L_LINE_PIN, LOW);
	HAL_GPIO_WritePin(R_LINE_PORT, R_LINE_PIN, LOW);

	if(n_e >= 12 && n_d >= 12) estado = AMBOS_90;
	else if(n_e >= 12) estado = ESQUERDA_90;
	else if(n_d >= 12) estado = DIREITA_90;

	//if(estado != 0) beep(200);

	return estado;
}


/**
  * @brief Verifica a leitura do giroscópio
  * @param Nenhum
  * @return w Velocidade angular
  */
int32_t getGyro()
{
	int32_t w = 0;

	w = getRawADC(G_OUTZ_CH) - getRawADC(G_VREF_CH);

	/*int i;
	int sampleNum = 20;
	aSpeed = 0;
	for(i=0;i<sampleNum;i++)
		aSpeed += read_Outz;
    aSpeed *= 50000/sampleNum;
	aSpeed -= 92980000;
	aSpeed /= 50000;
	aSpeed /= 4;
	angle += aSpeed;*/

	return w;
}


/**
  * @brief Verifica a tensão da bateria
  * @param Nenhum
  * @return Tensão da bateria em mV
  */
int32_t getTensao()
{
	return ((getRawADC(VBAT_CH) * VBAT_V) / VBAT_RAW);
}


/**
  * @brief Realiza a conversão de um canal analógico
  * @param canal Canal analógico a ser realizado a conversão
  * @return rawADC Resultado da conversão (valor de 12 bits)
  */
uint32_t getRawADC(uint32_t canal)
{
	uint32_t rawADC;
	ADC_ChannelConfTypeDef sConfig;

	sConfig.Channel = canal;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 10);
	rawADC = HAL_ADC_GetValue(&hadc1);
	//HAL_ADC_Stop(&hadc1);

	return rawADC;
}
