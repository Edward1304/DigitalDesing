#include "RGB_LED.h"

RGB_LEDS_struct_t parameters_LEDS;

/*
 * @brief	Set the ports and pins where the LEDs are connected
 * @param	Structure that contain the pins and ports of the LEDS
 * @retval 	None
 */
void RGB_Init(RGB_LEDS_struct_t parameters)
{
	parameters_LEDS = parameters;
}

/*
 * @brief	Turn on the red LED of ID parameter
 * @param	Identifier of the LED to turn on
 * @retval	None
 */
void RGB_RED_ON(ID_LED ID)
{
	switch(ID)
	{
		case SPEED:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Speed_Port, parameters_LEDS.RED_Speed_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Speed_Port, parameters_LEDS.GREEN_Speed_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Speed_Port, parameters_LEDS.BLUE_Speed_Pin, GPIO_PIN_RESET);
			break;
		case TEMPERATURE:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Temperature_Port, parameters_LEDS.RED_Temperature_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Temperature_Port, parameters_LEDS.GREEN_Temperature_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Temperature_Port, parameters_LEDS.BLUE_Temperature_Pin, GPIO_PIN_RESET);
			break;
		case NONE:
			break;
	}
}

/*
 * @brief	Turn on the green LED of ID parameter
 * @param	Identifier of the LED to turn on
 * @retval	None
 */
void RGB_GREEN_ON(ID_LED ID)
{
	switch(ID)
	{
		case SPEED:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Speed_Port, parameters_LEDS.RED_Speed_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Speed_Port, parameters_LEDS.GREEN_Speed_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Speed_Port, parameters_LEDS.BLUE_Speed_Pin, GPIO_PIN_RESET);
			break;
		case TEMPERATURE:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Temperature_Port, parameters_LEDS.RED_Temperature_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Temperature_Port, parameters_LEDS.GREEN_Temperature_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Temperature_Port, parameters_LEDS.BLUE_Temperature_Pin, GPIO_PIN_RESET);
			break;
		case NONE:
			break;
	}
}

/*
 * @brief	Turn on the blue LED of ID parameter
 * @param	Identifier of the LED to turn on
 * @retval	None
 */
void RGB_BLUE_ON(ID_LED ID)
{
	switch(ID)
	{
		case SPEED:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Speed_Port, parameters_LEDS.RED_Speed_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Speed_Port, parameters_LEDS.GREEN_Speed_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Speed_Port, parameters_LEDS.BLUE_Speed_Pin, GPIO_PIN_SET);
			break;
		case TEMPERATURE:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Temperature_Port, parameters_LEDS.RED_Temperature_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Temperature_Port, parameters_LEDS.GREEN_Temperature_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Temperature_Port, parameters_LEDS.BLUE_Temperature_Pin, GPIO_PIN_SET);
			break;
		case NONE:
			break;
	}
}

/*
 * @brief	Turn off all LEDs of ID parameter
 * @param	Identifier of the LEDs to turn off
 * @retval	None
 */
void RGB_ALL_OFF(ID_LED ID)
{
	switch(ID)
	{
		case SPEED:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Speed_Port, parameters_LEDS.RED_Speed_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Speed_Port, parameters_LEDS.GREEN_Speed_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Speed_Port, parameters_LEDS.BLUE_Speed_Pin, GPIO_PIN_RESET);
			break;
		case TEMPERATURE:
			HAL_GPIO_WritePin(parameters_LEDS.RED_Temperature_Port, parameters_LEDS.RED_Temperature_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.GREEN_Temperature_Port, parameters_LEDS.GREEN_Temperature_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(parameters_LEDS.BLUE_Temperature_Port, parameters_LEDS.BLUE_Temperature_Pin, GPIO_PIN_RESET);
			break;
		case NONE:
			break;
	}
}

