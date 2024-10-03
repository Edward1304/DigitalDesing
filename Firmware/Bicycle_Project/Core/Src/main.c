/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
ring_buffer_t rb_usart1;
uint8_t rb_buffer_usart1[CAPACITY_USART1];
uint8_t data_usart1_rx;
uint8_t data_rb_usart1;
uint8_t buffer_usart1_rx[CAPACITY_BUFFER_USART1_RX];
uint8_t read_buffer_ring_counter;

uint8_t key[CAPACITY_KEY];
uint8_t value_str[CAPACITY_VALUE];
double value_decimal;

uint8_t transmit_text[CAPACITY_TRANSMIT_TEXT];
uint8_t size_to_send;

uint32_t temperature_printing_control;

RGB_LEDS_struct_t parameter_RGB_leds;

boolean_enum key_flag;
boolean_enum new_parameter_to_set_flag;

float temperature;
float temperature_mean;
uint8_t counter_for_mean;
float values_for_mean[MAX_MEASURES_OF_TEMPERATURE_FOR_MEAN];
int8_t min_red_temperature;
int8_t max_red_temperature;
int8_t min_green_temperature;
int8_t max_green_temperature;
int8_t min_blue_temperature;
int8_t max_blue_temperature;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */
void print_temperature(void);
void update_temperature_indicator(void);
void set_value_received(void);
void calculate_mean_temperature(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//int _write(int file, char *ptr, int len)
//{
//  HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, MAX_TIME_WAIT_TO_TRANSMIT);
//  return len;
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1) {
		data_usart1_rx = USART1->RDR;
		ring_buffer_write(&rb_usart1, data_usart1_rx);
		HAL_UART_Receive_IT(&huart1, &data_usart1_rx, 1);
	}
}

void set_value_received()
{
	boolean_enum error_value = False;

	if(!strcmp((const char *)&key, (const char *)ID_TEMP_MIN_RED_LED))
	{
		if((value_decimal >= max_green_temperature) && (value_decimal < max_red_temperature))
		{
			min_red_temperature = (int8_t)value_decimal;
		}else{error_value = True;}

	}else if(!strcmp((const char *)&key, (const char *)ID_TEMP_MAX_RED_LED))
		{
			if((value_decimal > min_red_temperature) && (value_decimal <= MAX_TEMPERATURE_MEASURE_DS18B20_SENSOR))
			{
				max_red_temperature = (int8_t)value_decimal;
			}else{error_value = True;}
		}else if(!strcmp((const char *)&key, (const char *)ID_TEMP_MIN_GREEN_LED))
			{
				if((value_decimal >= max_blue_temperature) && (value_decimal < max_green_temperature))
				{
					min_green_temperature = (int8_t)value_decimal;
				}else{error_value = True;}
			}else if(!strcmp((const char *)&key, (const char *)ID_TEMP_MAX_GREEN_LED))
				{
					if((value_decimal > min_green_temperature) && (value_decimal <= min_red_temperature))
					{
						max_green_temperature = (int8_t)value_decimal;
					}else{error_value = True;}
				}else if(!strcmp((const char *)&key, (const char *)ID_TEMP_MIN_BLUE_LED))
					{
						if((value_decimal >= MIN_TEMPERATURE_MEASURE_DS18B20_SENSOR) && (value_decimal < max_blue_temperature))
						{
							min_blue_temperature = (int8_t)value_decimal;
						}else{error_value = True;}
					}else if(!strcmp((const char *)&key, (const char *)ID_TEMP_MAX_BLUE_LED))
						{
							if((value_decimal > min_blue_temperature) && (value_decimal <= min_green_temperature))
							{
								max_blue_temperature = (int8_t)value_decimal;
							}else{error_value = True;}
						}

	if(error_value == True)
	{
		size_to_send = sprintf((char *)&transmit_text, "\r\n---------------\r\n!Error configuring the value, verify the ranges!\r\n---------------\r\n");
	}else{
		size_to_send = sprintf((char *)&transmit_text, "\r\n---------------\r\n%s=%d configured successfully!\r\n---------------\r\n", (char *)&key, (int)value_decimal);
	}

	HAL_UART_Transmit(&huart1, (const uint8_t *)&transmit_text, size_to_send, MAX_TIME_WAIT_TO_TRANSMIT);
	new_parameter_to_set_flag = False;
}

void print_temperature()
{
	temperature = DS18B20_Get_temperature();
	size_to_send = sprintf((char *)&transmit_text, "Temperature = %0.2f °C \r\n", temperature);
	HAL_UART_Transmit(&huart1, (const uint8_t *)&transmit_text, size_to_send, 500);

	values_for_mean[counter_for_mean-1] = temperature;

	temperature_printing_control = HAL_GetTick();
}

void update_temperature_indicator()
{
	if((temperature >= min_blue_temperature) && (temperature <= max_blue_temperature)){
		RGB_BLUE_ON(TEMPERATURE);
	}else if((temperature > min_green_temperature) && (temperature <= max_green_temperature)){
		RGB_GREEN_ON(TEMPERATURE);
	}else if((temperature > min_red_temperature) && (temperature <= max_red_temperature)){
		RGB_RED_ON(TEMPERATURE);
	}else{
		RGB_ALL_OFF(TEMPERATURE);
	}
}

void calculate_mean_temperature()
{
	float temporal_temperature_mean = 0;

	for(int i=0; i<MAX_MEASURES_OF_TEMPERATURE_FOR_MEAN; i++)
	{
	 temporal_temperature_mean += values_for_mean[i];
	}
	temperature_mean = temporal_temperature_mean/MAX_MEASURES_OF_TEMPERATURE_FOR_MEAN;

	size_to_send = sprintf((char *)&transmit_text, "Mean temperature= %0.2f\r\n", temperature_mean);
	HAL_UART_Transmit(&huart1, (const uint8_t *)&transmit_text, size_to_send, MAX_TIME_WAIT_TO_TRANSMIT);
	counter_for_mean = 0;
	memset(&values_for_mean, 0, sizeof(values_for_mean));
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start_IT(&htim3);
  DS18B20_Init(htim3, DS18B20_GPIO_Port, DS18B20_Pin);

  parameter_RGB_leds.RED_Speed_Port = R_SPEED_LED_GPIO_Port;
  parameter_RGB_leds.RED_Speed_Pin = R_SPEED_LED_Pin;
  parameter_RGB_leds.GREEN_Speed_Port = G_SPEED_LED_GPIO_Port;
  parameter_RGB_leds.GREEN_Speed_Pin = G_SPEED_LED_Pin;
  parameter_RGB_leds.BLUE_Speed_Port = B_SPEED_LED_GPIO_Port;
  parameter_RGB_leds.BLUE_Speed_Pin = B_SPEED_LED_Pin;

  parameter_RGB_leds.RED_Temperature_Port = R_TEMP_LED_GPIO_Port;
  parameter_RGB_leds.RED_Temperature_Pin = R_TEMP_LED_Pin;
  parameter_RGB_leds.GREEN_Temperature_Port = G_TEMP_LED_GPIO_Port;
  parameter_RGB_leds.GREEN_Temperature_Pin = G_TEMP_LED_Pin;
  parameter_RGB_leds.BLUE_Temperature_Port = B_TEMP_LED_GPIO_Port;
  parameter_RGB_leds.BLUE_Temperature_Pin = B_TEMP_LED_Pin;

  RGB_Init(parameter_RGB_leds);

  ring_buffer_init(&rb_usart1, rb_buffer_usart1, CAPACITY_USART1);
  HAL_UART_Receive_IT(&huart1, &data_usart1_rx, 1);

  key_flag = False;
  new_parameter_to_set_flag = False;
  read_buffer_ring_counter = 0;
  value_decimal = 0;

  temperature = 0;
  temperature_mean = 0;
  counter_for_mean = 0;
  min_blue_temperature = 0;
  max_blue_temperature = 10;
  min_green_temperature = 10;
  max_green_temperature = 20;
  min_red_temperature = 20;
  max_red_temperature = 30;


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  /*
	   *Check if UART have any data and separate the key and
	   *value for validation
	   */
	  if(ring_buffer_is_empty(&rb_usart1) != 1)						//Si el buffer de rb_usart1 tiene datos para leer
	  {
		  ring_buffer_read(&rb_usart1, &data_rb_usart1);			//lee un dato del buffer


		  /*
		   * Si detecta que el dato leído es igual al caracter "="
		   * Porcede a guardar lo que se encuentre en el arreglo
		   * buffer_usart1_rx en la variable "key"
		   */
		  if((data_rb_usart1 == '=') && (key_flag == False))
		  {
			  memset(&key, 0, sizeof(key));
			  strcpy((char *)&key, (const char *)&buffer_usart1_rx);

			  memset(&buffer_usart1_rx, 0, sizeof(buffer_usart1_rx));
			  read_buffer_ring_counter = 0;
			  key_flag = True;

			  size_to_send = sprintf((char *)&transmit_text, "UART received: Key= %s\t", key);
	  		  HAL_UART_Transmit(&huart1, (const uint8_t *)&transmit_text, size_to_send, MAX_TIME_WAIT_TO_TRANSMIT);

			  continue;
		  }

		  buffer_usart1_rx[read_buffer_ring_counter] = data_rb_usart1;	//Va llenando un nuevo arreglo con el dato leído dle buffer


		  /*
		   * Cuando ya no hay datos para leer, procede a guardar el arrelo
		   * buffer_usart1_rx en la variable value
		   */
		  if(ring_buffer_size(&rb_usart1) == 0 && key_flag == True)
		  {
			  value_decimal = 0;
			  memset(&value_str, 0, sizeof(value_str));
			  strcpy((char *)&value_str, (const char *)&buffer_usart1_rx);

			  for(int i = 0; i<strlen((const char *)value_str); i++)
			  {
				  value_decimal += (value_str[i]-'0') * pow(10, (strlen((const char *)value_str)-1-i));
			  }

			  memset(&buffer_usart1_rx, 0, sizeof(buffer_usart1_rx));
			  read_buffer_ring_counter = 0;
			  key_flag = False;

			  size_to_send = sprintf((char *)&transmit_text, "Value= %s\r\n", value_str);
			  HAL_UART_Transmit(&huart1, (const uint8_t *)&transmit_text, size_to_send, MAX_TIME_WAIT_TO_TRANSMIT);

			  new_parameter_to_set_flag = True;
			  continue;
		  }

		  read_buffer_ring_counter ++; //Va aumentando el contador de control del arreglo buffer_usart1_rx
	  }

	  /*
	   * After receive a new value, it sets it in the corresponding variable
	   */
	  if(new_parameter_to_set_flag == True)
	  {
		  set_value_received();
	  }

	  /*
	  *Wait a time specified in WAIT_TEMPERATURE_PRINT and
	  *temperature print
	  */
	  if(HAL_GetTick() - temperature_printing_control >= WAIT_TEMPERATURE_PRINT)
	  {
		 counter_for_mean++;

		 print_temperature();
		 update_temperature_indicator();

		 if(counter_for_mean == MAX_MEASURES_OF_TEMPERATURE_FOR_MEAN)
		 {
			 calculate_mean_temperature();
		 }

	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 48-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 0xFFFF-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DS18B20_Pin|B_TEMP_LED_Pin|SETTING_LED_Pin|FLASH_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, G_TEMP_LED_Pin|R_TEMP_LED_Pin|B_SPEED_LED_Pin|G_SPEED_LED_Pin
                          |R_SPEED_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LOAD_GPIO_Port, LOAD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : DS18B20_Pin B_TEMP_LED_Pin SETTING_LED_Pin LOAD_Pin
                           FLASH_CS_Pin */
  GPIO_InitStruct.Pin = DS18B20_Pin|B_TEMP_LED_Pin|SETTING_LED_Pin|LOAD_Pin
                          |FLASH_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SPEED_SENSOR_DIGITAL_Pin */
  GPIO_InitStruct.Pin = SPEED_SENSOR_DIGITAL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPEED_SENSOR_DIGITAL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : G_TEMP_LED_Pin R_TEMP_LED_Pin B_SPEED_LED_Pin G_SPEED_LED_Pin
                           R_SPEED_LED_Pin */
  GPIO_InitStruct.Pin = G_TEMP_LED_Pin|R_TEMP_LED_Pin|B_SPEED_LED_Pin|G_SPEED_LED_Pin
                          |R_SPEED_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DOWN_BUTTON_Pin MODE_BUTTON_Pin UP_BUTTON_Pin */
  GPIO_InitStruct.Pin = DOWN_BUTTON_Pin|MODE_BUTTON_Pin|UP_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
