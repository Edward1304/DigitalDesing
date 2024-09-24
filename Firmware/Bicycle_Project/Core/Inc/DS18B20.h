#ifndef INC_DS18B20_
#define INC_DS18B20_

//#include "main.h"
#include "stm32f0xx_hal.h"

//ROM COMMANDS
#define SEARCH_ROM		0xF0	//When a system is initially powered up, the master must identify the ROM codes of all slave devices on the bus, which allows the master to determine the number of slaves and their device types.
#define READ_ROM		0x33	//This command can only be used when there is one slave on the bus.
#define MATCH_ROM		0x55	//The match ROM command followed by a 64-bit ROM code sequence allows the bus master to address a specific slave device on a multidrop or single-drop bus.
#define SKIP_ROM		0xCC	//The master can use this command to address all devices on the bus simultaneously without sending out any ROM code information.
#define ALARM_SEARCH	0xEC	//The operation of this command is identical to the operation of the Search ROM command except that only slaves with a set alarm flag will respond.

//DS18B20 FUNCTION COMMANDS
#define CONVERT_T			0x44	//This command initiates a single temperature conversion.
#define WRITE_SCRATCHPAD	0x4E	//This command allows the master to write 3 bytes of data to the DS18B20’s scratchpad
#define READ_SCRATCHPAD		0xBE	//This command allows the master to read the contents of the scratchpad.
#define COPY_SCRATCHPAD		0x48	//This command copies the contents of the scratchpad TH, TL and configuration registers (bytes 2, 3 and 4) to EEPROM.
#define RECALL_E_SQUARE		0xB8	//This command recalls the alarm trigger values (TH and TL) and configuration data from EEPROM and places the data in bytes 2, 3, and 4, respectively, in the scratchpad memory.
#define READ_POWER_SUPPLY	0xB4	//The master device issues this command followed by a read time slot to determine if any DS18B20s on the bus are using parasite power.

//FUNCTIONS PROTOTYPE
float DS18B20_Get_temperature( void );
void DS18B20_Init(TIM_HandleTypeDef Timer_DS18B20, GPIO_TypeDef* Port_DS18B20, uint16_t Pin_DS18B20);

/*
static void Set_Pin_Input ( GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN );
static void Set_Pin_Output ( GPIO_TypeDef* GPIOx, uint16_t GPIO_PIN );
static uint8_t DS18B20_Start ( void );
static void DS18B20_Write ( uint8_t data );
static uint8_t DS18B20_Read (void);
static void delay ( uint16_t us );
static float convert_temperature(uint8_t byte_1, uint8_t byte_2);
*/

#endif /* INC_DS18B20_ */
