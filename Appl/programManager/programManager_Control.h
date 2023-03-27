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

#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR                  (uint8_t)0U
#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ERROR                     (uint8_t)1U

#define PROGRAMMANAGER_CONTROL_TEMP_PRES_ADDR                         (uint16_t)0U
#define PROGRAMMANAGER_CONTROL_TEMP_PRES_NUM                          (uint16_t)2U

#define PROGRAMMANAGER_CONTROL_TEMP_PRES_TEMPERATURE_OFFSET           0U
#define PROGRAMMANAGER_CONTROL_TEMP_PRES_PRESSURE_OFFSET              1U

#define PROGRAMMANAGER_CONTROL_TEMP_INVALID_VALUE                     (uint8_t)0xFFU
#define PROGRAMMANAGER_CONTROL_PRES_INVALID_VALUE                     (uint16_t)0xFFFFU

#define PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY                       (uint32_t)200U
#define PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY                       (uint32_t)200U

#define PROGRAMMANAGER_CONTROL_RELAY_ADDR                             (uint16_t)0U
#define PROGRAMMANAGER_CONTROL_RELAY_NUM                              (uint16_t)1U

#define PROGRAMMANAGER_CONTROL_COMMAND_NONE                           (uint8_t)255U
#define PROGRAMMANAGER_CONTROL_COMMAND_START                          (uint8_t)1U
#define PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME                   (uint8_t)2U
#define PROGRAMMANAGER_CONTROL_COMMAND_STOP                           (uint8_t)3U
#define PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP                      (uint8_t)4U
#define PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP                      (uint8_t)5U



#define PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK                  (uint16_t)0x0001U
#define PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_REV_MASK                  (uint16_t)0x0002U
#define PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK                  (uint16_t)0x0003U

#define PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK                (uint16_t)0x001CU
#define PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_OFFSET              (2U)

#define PROGRAMMANAGER_CONTROL_OUTPUT_WATER_MASK                      (uint16_t)0x0060U
#define PROGRAMMANAGER_CONTROL_OUTPUT_WATER_OFFSET                    (5U)

#define PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK                     (uint16_t)0x0080U
#define PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_OFFSET                   (7U)

#define PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK                     (uint16_t)0x0100U
#define PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_OFFSET                   (8U)

#define PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK                     (uint16_t)0x0200U
#define PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_OFFSET                   (9U)

#define PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK                       (uint16_t)0x0400U
#define PROGRAMMANAGER_CONTROL_OUTPUT_DRAIN_VALVE_MASK                (uint16_t)0x0800U
#define PROGRAMMANAGER_CONTROL_OUTPUT_DOOR_MASK                       (uint16_t)0x1000U

#define ProgramManager_Control_SetOutput(mask)                        { ProgramManager_gCurrentOutput |= mask; }
#define ProgramManager_Control_ClearOutput(mask)                      { ProgramManager_gCurrentOutput &= (uint16_t)(~mask); }
#define ProgramManager_Control_ModifyOutput(mask, value)              { ProgramManager_gCurrentOutput &= (uint16_t)(~mask); ProgramManager_gCurrentOutput |= value; }
#define ProgramManager_Control_ClearAllOutput()                       { ProgramManager_gCurrentOutput = (uint16_t)0U; }



typedef struct 
{
  uint32_t dataId;
  uint32_t command;
} ProgramManager_Control_PostRunStruct;

typedef struct 
{
  uint32_t dataId;
  uint32_t oneSecondElapsed;
  uint32_t tempCounter;
  uint32_t presCounter;
  uint32_t motorState;
  uint32_t motorCounter;
  uint32_t motorCounterMax;
} ProgramManager_Control_RunWashStruct;

typedef struct 
{
  uint32_t dataId;
  uint32_t executeStep;
} ProgramManager_Control_PreRunStruct;




/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

extern uint8_t ProgramManager_gSensorInverterErr;
extern uint8_t ProgramManager_gSensorImbalanceErr;
extern uint8_t ProgramManager_gSensorDoorOpenErr;

extern uint8_t ProgramManager_gCurrentTemperature;
extern uint16_t ProgramManager_gCurrentPressure;

extern uint8_t ProgramManager_gCurrentTempThreshold;
extern uint16_t ProgramManager_gCurrentPresThreshold;

extern bool ProgramManager_gTempThresExceeded;
extern bool ProgramManager_gPresThresExceeded;

extern uint8_t ProgramManager_gInitStepIdx;
extern uint16_t ProgramManager_gCurrentWashRunTime;
extern uint16_t ProgramManager_gCurrentWashStopTime;
extern ProgramManager_MotorSpeedType ProgramManager_gCurrentWashSpeed;

extern uint16_t ProgramManager_gCurrentDrainFirstDrainTime;
extern uint16_t ProgramManager_gCurrentDrainForwardDrainTime;
extern uint16_t ProgramManager_gCurrentDrainBalanceDrainTime;
extern uint16_t ProgramManager_gCurrentDrainExtrLvl1DrainTime;
extern uint16_t ProgramManager_gCurrentDrainExtrLvl2DrainTime;
extern uint16_t ProgramManager_gCurrentDrainExtrLvl3DrainTime;
extern ProgramManager_MotorSpeedType ProgramManager_gCurrentDrainFirstDrainSpeed;
extern ProgramManager_MotorSpeedType ProgramManager_gCurrentDrainForwardDrainSpeed;
extern ProgramManager_MotorSpeedType ProgramManager_gCurrentDrainBalanceDrainSpeed;
extern ProgramManager_MotorSpeedType ProgramManager_gCurrentDrainExtrLvl1DrainSpeed;
extern ProgramManager_MotorSpeedType ProgramManager_gCurrentDrainExtrLvl2DrainSpeed;
extern ProgramManager_MotorSpeedType ProgramManager_gCurrentDrainExtrLvl3DrainSpeed;

extern uint8_t ProgramManager_gTimeCountMin;
extern uint8_t ProgramManager_gTimeCountSec;
extern uint8_t ProgramManager_gMotorState;

extern uint8_t ProgramManager_gIsPaused;
extern uint8_t ProgramManager_gIsError;

extern uint16_t ProgramManager_gCurrentOutput;



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

#define ProgramManager_Control_NotPauseAndError()                     ((ProgramManager_gIsPaused == (uint8_t)0U) /* && (ProgramManager_gIsError == (uint8_t)0U) */)

void ProgramManager_Control_Init(void);

void ProgramManager_Control_SetCommand(uint8_t command);
void ProgramManager_Control_RetrieveCommand(uint8_t *command);

void ProgramManager_Control_TogglePauseResumeHandler(void);

bool ProgramManager_Control_CheckNextStepAvailable(void);
bool ProgramManager_Control_CheckPrevStepAvailable(void);
bool ProgramManager_Control_GetNextStepAvailable(uint32_t *nextStep);
bool ProgramManager_Control_GetPrevStepAvailable(uint32_t *prevStep);

void ProgramManager_Control_TxRxSignalSubMainFunction(void);
void ProgramManager_Control_AnalyzeDataSubMainFunction(void);
void ProgramManager_Control_UpdateThresholdSubMainFunction(void);



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_CONTROL_H */
