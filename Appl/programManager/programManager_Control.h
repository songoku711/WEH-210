/* 
 * File:   programManager_Control.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef PROGRAMMANAGER_CONTROL_H
#define PROGRAMMANAGER_CONTROL_H

#ifdef  __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "main.h"

#include "ProgramManager_Types.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define PROGRAMMANAGER_CONTROL_SLAVE_ADDR                             (uint8_t)1U

#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ADDR                      (uint16_t)0U
#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NUM                       (uint16_t)4U

#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_INVERTER_ERR_OFFSET       0U
#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_IMBALANCE_ERR_OFFSET      1U
#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_DOOROPEN_ERR_OFFSET       2U

#define PROGRAMMANAGER_CONTROL_TEMP_PRES_ADDR                         (uint16_t)0U
#define PROGRAMMANAGER_CONTROL_TEMP_PRES_NUM                          (uint16_t)2U

#define PROGRAMMANAGER_CONTROL_TEMP_PRES_TEMPERATURE_OFFSET           0U
#define PROGRAMMANAGER_CONTROL_TEMP_PRES_PRESSURE_OFFSET              1U

#define PROGRAMMANAGER_CONTROL_RELAY_ADDR                             (uint16_t)0U
#define PROGRAMMANAGER_CONTROL_RELAY_NUM                              (uint16_t)1U

#define PROGRAMMANAGER_CONTROL_COMMAND_NONE                           (uint8_t)255U
#define PROGRAMMANAGER_CONTROL_COMMAND_START                          (uint8_t)1U
#define PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME                   (uint8_t)2U
#define PROGRAMMANAGER_CONTROL_COMMAND_STOP                           (uint8_t)3U
#define PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP                      (uint8_t)4U
#define PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP                      (uint8_t)5U



/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

extern uint8_t ProgramManager_gSensorInverterErr;
extern uint8_t ProgramManager_gSensorImbalanceErr;
extern uint8_t ProgramManager_gSensorDoorOpenErr;
extern uint8_t ProgramManager_gCurrentTemperature;
extern uint8_t ProgramManager_gCurrentPressure;

extern uint8_t ProgramManager_gIsPaused;
extern uint8_t ProgramManager_gIsError;



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void ProgramManager_Control_Init(void);

void ProgramManager_Control_SetCommand(uint8_t command);
void ProgramManager_Control_RetrieveCommand(uint8_t *command);

void ProgramManager_Control_TxRxSignalSubMainFunction(void);
void ProgramManager_Control_AnalyzeDataSubMainFunction(void);



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_CONTROL_H */
