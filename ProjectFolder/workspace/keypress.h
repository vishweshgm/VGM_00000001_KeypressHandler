/*
 * keypress.h
 *
 *  Created on: Dec 22, 2021
 *      Author: vishweshgm
 */

#ifndef APPLICATION_DRIVER_KEYPRESS_INC_KEYPRESS_H_
#define APPLICATION_DRIVER_KEYPRESS_INC_KEYPRESS_H_


/*! \mainpage Key Press Handling
 *
 * This library is written for easy keypress handling in any microcontroller which has periocdic scheduler tasks.
 * The library allows software to handle one keypress at a time even if user presses undesired multiple presses.
 * Also the library allows press & hold feature for pushbuttons. It allows user to individually set debounce time, 
 * 
 * - \subpage integFiles "How to integrate the files?"
 *
 * - \subpage configButtons "How to configure buttons?"
 *
 */ 
 /*! \page integFiles How to integrate the files?
 *
 * First copy @ref keypress.c and @ref keypress.h files to your project.
 * Call the function @ref CommandLoop from your periodic task and pass the value of raster in ms.
 * @ref SM_KeyPressed variable will be updated if a keypress is detected.
 *
 */

/*! \page configButtons How to configure buttons?
 *
 * Buttons may not need to be actual pushbuttons. It can be anything that is expected to trigger some action.
 * So this library calls keypress actions as Commands.
 * In order to configure new button, update following
 * \ref keypress_EnumButton : Keep @ref CMD_Enum_DEFAULT and @ref CMD_Enum_MAX as it is and add your buttons in between.
 * In the library B1 - Up B2- Down, B3 - Enter/Ok, B4 - Back
 * @ref CMD_Parameter : This structure has many variables. 
 * CmdNum
 * @ref KEYPRESS_INIT : Update this macro with 
 */
 
#include "main.h"

/**
  * @brief DEBOUNCE_MS : Write the debounce value in ms for first press
  */
#define DEBOUNCE_MS	100
/**
  * @brief PRESSNHOLD_MS : Write the debounce value in ms for holding press
  */
#define PRESSNHOLD_MS	100

/*APPLICATION SPECIFIC PART*/
typedef enum keypress_EnumButton_Tag{
	CMD_Enum_DEFAULT	,
	CMD_Enum_B1	,
	CMD_Enum_B2	,
	CMD_Enum_B3,
	CMD_Enum_B4,
	CMD_Enum_MAX
}keypress_EnumButton;

typedef struct CMD_ButtonParameter_Tag{
	const uint16_t CmdNum;
	uint8_t CMD_PrsNHldCntr;
	uint8_t CMD_DebounceCount;
	uint16_t CMD_synchro;
	uint16_t CMD_synchro_old;
	uint8_t CMD_userConfigDebThreshold; /* Initial Press Debounce Threshold*/
	uint8_t CMD_CurrentState; /* 0 - Not Pressed , 1 - Pressed*/
	uint8_t CMD_lastState;
	uint8_t CMD_EnablePrsNHld;
	uint8_t CMD_DebouncedState;
	uint8_t CMD_userConfigPHDebThreshold;/*Press and Hold Debounce Threshold*/
	uint8_t ContinuousCountFlag;
	keypress_EnumButton CMD_enum;
}CMD_Parameter;




#define KEYPRESS_INIT \
{\
	{0x00,0,0,0,0,DEBOUNCE_MS,0,0,0,0,PRESSNHOLD_MS,0,CMD_Enum_DEFAULT},\
	{0xA1,0,0,0,0,DEBOUNCE_MS,0,0,0,0,PRESSNHOLD_MS,0,CMD_Enum_B1},\
	{0xA2,0,0,0,0,DEBOUNCE_MS,0,0,0,0,PRESSNHOLD_MS,0,CMD_Enum_B2},\
	{0xA3,0,0,0,0,DEBOUNCE_MS,0,0,1,0,PRESSNHOLD_MS,0,CMD_Enum_B3},\
	{0xA4,0,0,0,0,DEBOUNCE_MS,0,0,1,0,PRESSNHOLD_MS,0,CMD_Enum_B4}\
}
void keypressInit(void);
void CommandLoop(uint8_t loopms);
void updateCommand(uint16_t Command, uint8_t loopms);
void synchronizeCommands(void);
void acquire_appl_keydemand(void);


#endif /* APPLICATION_DRIVER_KEYPRESS_INC_KEYPRESS_H_ */
