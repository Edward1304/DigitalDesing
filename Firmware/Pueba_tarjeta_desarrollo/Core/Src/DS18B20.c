#include "DS18B20.h"

//Write the timer configured for the SD18B20 sensor. Remember configure timer in micro.seconds.
extern TIM_HandleTypeDef htim3;

uint8_t Presence;	//Variable for the presence byte in initialization of communication
uint8_t Temp_byte1;	//Variable for save the first byte received of DS18B20 sensor
uint8_t Temp_byte2;	//Variable for save the second byte received of DS18B20 sensor
float temp_dec;		//Variable for save the decimal value converted of the two bytes

/*
 * @brief	Wait the micro-seconds of the parameter.
 * @retval	None
 */
static void delay ( uint16_t us )
{
	__HAL_TIM_SET_COUNTER(&htim3, 0);	//set the counter value a 0
	while(__HAL_TIM_GET_COUNTER(&htim3) < us);	//wait for the counter to reach the us input in the parameter us
}

/*
 * @brief	Configure pin as input
 * @retval	None
 */
static void Set_Pin_Input (GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN )
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/*
 * @brief Configure pin as output
 * @retval None
 */
static void Set_Pin_Output (GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN )
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);

}

/*
 * @brief	Indicate to the DS18B20 sensor that will start communication
 * 			with master.
 * @retval	Return value of presence of the DS18B20 sensor. If value=1,
 * 			detected sensor, but valure=0, the not detected sensor.
 */
static uint8_t DS18B20_Start ( void )
{

	uint8_t Response = 0;
	Set_Pin_Output(DS18B20_GPIO_Port, DS18B20_Pin);
	HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
	delay (500);

	Set_Pin_Input(DS18B20_GPIO_Port, DS18B20_Pin);

	delay (80);

	if(!(HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin))) Response = 1;
	else Response = 0;

	delay (400);

	return Response;
}

/*
 * @brief	Write a byte to the DS18B20 for start a process
 * @retval	None
 */
static void DS18B20_Write ( uint8_t data )
{

	Set_Pin_Output(DS18B20_GPIO_Port, DS18B20_Pin);

	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i)) !=0 )	//if the bit is high
		{

			//write 1
			Set_Pin_Output(DS18B20_GPIO_Port, DS18B20_Pin);
			HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
			delay (1);

			Set_Pin_Input(DS18B20_GPIO_Port, DS18B20_Pin);
			delay (60);

		}else //if the bit is low
		{

			//write 0
			Set_Pin_Output(DS18B20_GPIO_Port, DS18B20_Pin);
			HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
			delay (60); //wait for 60 us

			Set_Pin_Input(DS18B20_GPIO_Port, DS18B20_Pin);
		}
	}
}

/*
 * @brief	Read byte of the DS18B20 sensor
 * @retval	Byte returned by the DS18B20 sensor
 */
static uint8_t DS18B20_Read (void)
{

	uint8_t value = 0;
	Set_Pin_Input(DS18B20_GPIO_Port, DS18B20_Pin);

	for(int i=0; i<8; i++)
	{
		Set_Pin_Output(DS18B20_GPIO_Port, DS18B20_Pin);

		HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
		delay (2);

		Set_Pin_Input(DS18B20_GPIO_Port, DS18B20_Pin);
		delay (5);
		if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin))
		{
			value |= 1<<i; //read=1
		}
		delay (60);
	}
	return value;
}

/*
 * @brief	Coveter of bytes to decimal value of temperature
 * @param	byte_1 Byte #1 to convert
 * @param	byte_2 Byte #2 to convert
 * @retval	Decimal value of temperature
 */
static float convert_temperature(uint8_t byte_1, uint8_t byte_2)
{
	uint16_t tempval = byte_2 << 8 | byte_1;
	float result_temp = (128.0/2048)*tempval;

	return result_temp;
}

/*
 * @brief	Get the temperature value
 * @retval	float with the temperature value
 */
float Get_temperature( void )
{
	Presence = DS18B20_Start();
	HAL_Delay(1);
	DS18B20_Write(SKIP_ROM);
	DS18B20_Write(CONVERT_T);
	HAL_Delay(800);

	Presence = DS18B20_Start();
	HAL_Delay(1);
	DS18B20_Write(SKIP_ROM);
	DS18B20_Write(READ_SCRATCHPAD);

	Temp_byte1 = DS18B20_Read();
	Temp_byte2 = DS18B20_Read();

	//HAL_UART_Transmit(&huart1, (const uint8_t *)&read, 6, 1000);
	//size_to_send = sprintf( (char *)&transmit_text, "presence %d el primer %d y el segundo %d \r\n", Presence, Temp_byte1, Temp_byte2);
	//HAL_UART_Transmit(&huart1, (const uint8_t *)&transmit_text, size_to_send, 1000);
	temp_dec = convert_temperature(Temp_byte1, Temp_byte2);

	return temp_dec;
}




