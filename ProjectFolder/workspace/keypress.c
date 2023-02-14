/*
 * keypress.c
 *
 *  Created on: Dec 22, 2021
 *      Author: vishweshgm
 */
#define KEYPRESS_C 1
#if KEYPRESS_C == 1

#include "keypress.h"


keypress_EnumButton SM_KeyPressed;
CMD_Parameter CMD_Structure[CMD_Enum_MAX] = KEYPRESS_INIT;
extern uint8_t switchOn;

void keypressInit(void){
	/*If any initializations required, do it here*/
}
/*! \brief This is the function to be called from periodic task.
 * 1. This fucntion is user defined partially.
 * 2. CMD_userConfigDebThreshold & CMD_userConfigPHDebThreshold can be configured based on requirement.
 * 3. Event that triggers the command is written. In this case GPIO read. Notice the use of API @ref updateCommand and change it as per your need.
 * 4. @ref synchronizeCommands is called to reset debounce variables when key is not pressed.
 * 5. @ref acquire_appl_keydemand function finally updates @ref SM_KeyPressed variable based on debounced key
 * \param loopms raster in which this function is running in ms.
 * \return None
 */
void CommandLoop(uint8_t loopms){
	uint8_t command = 0;

	for(uint8_t i = 0; i<(sizeof(CMD_Structure)/sizeof(CMD_Parameter)); i++){
		CMD_Structure[i].CMD_userConfigDebThreshold = DEBOUNCE_MS/loopms;
		CMD_Structure[i].CMD_userConfigPHDebThreshold = CONTINUOUSMODE_DEBOUNCE_MS/loopms;
	}

	if(HAL_GPIO_ReadPin(uC_B1_GPIO_Port, uC_B1_Pin) == GPIO_PIN_SET){
		command = 0xA4;
		updateCommand(command,loopms);
	}

	if(HAL_GPIO_ReadPin(uC_B2_GPIO_Port, uC_B2_Pin) == GPIO_PIN_SET){
		command = 0xA3;
		updateCommand(command,loopms);
	}

	if(HAL_GPIO_ReadPin(uC_B3_GPIO_Port, uC_B3_Pin) == GPIO_PIN_SET){
		command = 0xA2;
		updateCommand(command,loopms);
	}

	if(HAL_GPIO_ReadPin(uC_B4_GPIO_Port, uC_B4_Pin) == GPIO_PIN_SET){
		command = 0xA1;
		updateCommand(command,loopms);
	}
	synchronizeCommands();
	acquire_appl_keydemand();

}

/*! \brief This is the keypress state handler.
 * 1. Detect which button is pressed, and update its CurrentState
 * 2. If the same command was detected as pressed in last loop, Go for further processing (jump to step 4)
 * 3. If lastState != 1. Update lastState as 1 and reset all debounce counters. But if the CMD_userConfigDebThreshold is 1, it must be considered as press activated. So make  DebouncedState = 1
 * 4. If lastState was 1, 
 * 4.1 If PressNHold feature is not enabled for the key, increment CMD_DebounceCount everyloop until CMD_userConfigDebThreshold is reached. Make DebouncedState = 1 & use CMD_PrsNHldCntr = 0xFF as flag to stop it from debouncing again.
 * 4.2 If PressNHold feature is enabled for the key, increment CMD_DebounceCount everyloop until CMD_userConfigDebThreshold is reached. Make DebouncedState = 1 & use CMD_PrsNHldCntr = 0xFE as flag to enable it to track pressNHold action.
 * 4.3 If PressNHold tracking has started, wait for 1 second before starting to run automatic pressnHold debouncing
 * 4.4 During automatic pressNHold debouncing, CMD_userConfigPHDebThreshold is used as debounce threshold.
 * 4.5 At any point of time, if the user releases button, CMD_synchro will not get incremented causing all debounce and logic parameters to reset to 0 in function  @ref synchronizeCommands.
 * \param Command Command number of keypress/event detected.
 * \param loopms raster in which this function is running in ms.
 * \return None
 */
void updateCommand(uint16_t Command, uint8_t loopms){
	for(uint8_t i = 0; i<(sizeof(CMD_Structure)/sizeof(CMD_Parameter)); i++){
		if(CMD_Structure[i].CmdNum == Command){
			/*Detect which button is pressed, and update its CurrentState*/
			CMD_Structure[i].CMD_CurrentState = 1;
			CMD_Structure[i].CMD_synchro++;
			if(CMD_Structure[i].CMD_lastState == 1){
				/*2. If the same command was detected as pressed in last loop (lastState = 1), Go for further processing*/
				if((CMD_Structure[i].CMD_DebouncedState == 0) && (CMD_Structure[i].CMD_PrsNHldCntr == 0) && (CMD_Structure[i].CMD_EnablePrsNHld == 0)){
					CMD_Structure[i].CMD_DebounceCount++;
					if(CMD_Structure[i].CMD_DebounceCount >= CMD_Structure[i].CMD_userConfigDebThreshold){
						CMD_Structure[i].CMD_DebounceCount = CMD_Structure[i].CMD_userConfigDebThreshold;
						CMD_Structure[i].CMD_DebouncedState = 1;
						CMD_Structure[i].CMD_PrsNHldCntr = 0xFF;
					}
				}
				else if((CMD_Structure[i].CMD_DebouncedState == 0) && (CMD_Structure[i].CMD_PrsNHldCntr == 0) && (CMD_Structure[i].CMD_EnablePrsNHld == 1)){
					CMD_Structure[i].CMD_DebounceCount++;
					if(CMD_Structure[i].CMD_DebounceCount >= CMD_Structure[i].CMD_userConfigDebThreshold){
						CMD_Structure[i].CMD_DebounceCount = CMD_Structure[i].CMD_userConfigDebThreshold;
						CMD_Structure[i].CMD_DebouncedState = 1;
						CMD_Structure[i].CMD_PrsNHldCntr = 0xFE;
					}
				}
				else if((CMD_Structure[i].CMD_DebouncedState == 0) && (CMD_Structure[i].CMD_PrsNHldCntr > 0)){
					if((CMD_Structure[i].CMD_EnablePrsNHld == 1) && (CMD_Structure[i].CMD_PrsNHldCntr == 0xFF)){
						CMD_Structure[i].CMD_PrsNHldCntr = 1;
					}
					else if((CMD_Structure[i].CMD_EnablePrsNHld == 1) && (CMD_Structure[i].CMD_PrsNHldCntr >= 1) && (CMD_Structure[i].ContinuousCountFlag == 0)){
						CMD_Structure[i].CMD_PrsNHldCntr++;
						if(CMD_Structure[i].CMD_PrsNHldCntr >= (1000/loopms))CMD_Structure[i].ContinuousCountFlag = 1;
					}

					if(CMD_Structure[i].ContinuousCountFlag == 1){
						if((CMD_Structure[i].CMD_EnablePrsNHld == 1) && (CMD_Structure[i].CMD_PrsNHldCntr > 0)){
							CMD_Structure[i].CMD_PrsNHldCntr++;
							if(CMD_Structure[i].CMD_PrsNHldCntr>= CMD_Structure[i].CMD_userConfigPHDebThreshold){
								CMD_Structure[i].CMD_PrsNHldCntr = 0;
								CMD_Structure[i].CMD_DebouncedState = 1;
							}
						}
					}
				}
				else if(CMD_Structure[i].CMD_DebouncedState == 1){
					CMD_Structure[i].CMD_DebouncedState = 0;
					if(CMD_Structure[i].CMD_EnablePrsNHld == 1){
						CMD_Structure[i].CMD_PrsNHldCntr++;
					}
				}
			}
			else{
				/*3. If lastState != 1. Update lastState as 1 and reset all debounce counters. But if the CMD_userConfigDebThreshold is 1, it must be considered as press activated. So make  DebouncedState = 1*/
				CMD_Structure[i].CMD_PrsNHldCntr = 0;
				CMD_Structure[i].CMD_DebounceCount = 0;
				if(CMD_Structure[i].CMD_userConfigDebThreshold == 1)CMD_Structure[i].CMD_DebouncedState = 1;
				CMD_Structure[i].CMD_lastState = 1;
				CMD_Structure[i].ContinuousCountFlag = 0;
			}
			break;
		}
	}
	return;
}

/*! \brief This function resets all counters if no key pressed
 * \param None
 * \return None
 */
void synchronizeCommands(void){
	for(uint8_t i = 0; i<(sizeof(CMD_Structure)/sizeof(CMD_Parameter)); i++){
		if(CMD_Structure[i].CMD_synchro_old == CMD_Structure[i].CMD_synchro){
			CMD_Structure[i].CMD_synchro = 0;
			CMD_Structure[i].CMD_synchro_old = 0;
			CMD_Structure[i].CMD_DebouncedState = 0;
			CMD_Structure[i].CMD_PrsNHldCntr = 0;
			CMD_Structure[i].CMD_CurrentState = 0;
			CMD_Structure[i].CMD_lastState = 0;
			CMD_Structure[i].CMD_DebounceCount = 0;
			CMD_Structure[i].ContinuousCountFlag = 1;
		}
		else{
			CMD_Structure[i].CMD_synchro_old = CMD_Structure[i].CMD_synchro;
		}
	}
}
/*! \brief This function updates @ref SM_KeyPressed available as global variable to Menu Handling.
 * \param None
 * \return None
 */
void acquire_appl_keydemand(void){
	if(CMD_Structure[CMD_Enum_B1].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B1;
	}
	else if(CMD_Structure[CMD_Enum_B2].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B2;
	}
	else if(CMD_Structure[CMD_Enum_B3].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B3;
	}
	else if(CMD_Structure[CMD_Enum_B4].CMD_DebouncedState == 1){
		SM_KeyPressed = CMD_Enum_B4;
	}
	else{
		SM_KeyPressed = CMD_Enum_DEFAULT;
	}
}


#endif
