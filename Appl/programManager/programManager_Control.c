/* 
 * File:   programManager_Control.c
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifdef  __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "programManager.h"

#include "eMB.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_1              (uint8_t)20U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_TEMP_PRES                   (uint8_t)40U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_2              (uint8_t)60U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_RELAY                       (uint8_t)80U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_COUNT_MAX                   (uint8_t)100U

#define PROGRAMMANAGER_CONTROL_ANALYZEDATA_DELAYSTART_MAX             (uint8_t)100U
#define PROGRAMMANAGER_CONTROL_ANALYZEDATA_COUNT_MAX                  (uint8_t)100U

#define PROGRAMMANAGER_CONTROL_IIR_FILTER_TEMP_ALPHA                  (float)0.09f
#define PROGRAMMANAGER_CONTROL_IIR_FILTER_PRES_ALPHA                  (float)0.08f

#define PROGRAMMANAGER_CONTROL_IIR_FILTER_TEMP_OFFSET                 (float)0.3f
#define PROGRAMMANAGER_CONTROL_IIR_FILTER_PRES_OFFSET                 (float)0.3f



/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

static uint8_t ProgramManager_Control_RecvSignalCount;

static uint8_t ProgramManager_Control_AnalyzeDataDelayStart;
static uint8_t ProgramManager_Control_AnalyzeDataCount;

static uint16_t ProgramManager_PauseOutput;

static float   ProgramManager_gRealTemperature;
static float   ProgramManager_gRealPressure;



uint8_t ProgramManager_gSensorInverterErr;
uint8_t ProgramManager_gSensorImbalanceErr;
uint8_t ProgramManager_gSensorDoorOpenErr;

uint8_t ProgramManager_gCurrentTemperature;
uint16_t ProgramManager_gCurrentPressure;

uint8_t ProgramManager_gCurrentTempThreshold;
uint16_t ProgramManager_gCurrentPresThreshold;

bool ProgramManager_gTempThresExceeded;
bool ProgramManager_gPresThresExceeded;

uint16_t ProgramManager_gCurrentWashRunTime;
uint16_t ProgramManager_gCurrentWashStopTime;
ProgramManager_MotorSpeedType ProgramManager_gCurrentWashSpeed;

uint8_t ProgramManager_gIsPaused;
uint8_t ProgramManager_gIsError;

uint16_t ProgramManager_gCurrentOutput;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

void ProgramManager_ControlInternal_AnalyzeSensorInput(void);
void ProgramManager_ControlInternal_AnalyzeTemperature(void);
void ProgramManager_ControlInternal_AnalyzePressure(void);

void ProgramManager_ControlInternal_VerifyOutput(void);

void ProgramManager_ControlInternal_UpdateTempThreshold(void);
void ProgramManager_ControlInternal_UpdatePresThreshold(void);
void ProgramManager_ControlInternal_UpdateWashTime(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

void ProgramManager_ControlInternal_AnalyzeSensorInput(void)
{
  uint8_t tempValue;

  /* Get inverter error state */
  eMB_Master_GetDiscretesInput
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_INPUT_SENSOR_INVERTER_ERR_OFFSET,
    &tempValue
  );

  if (tempValue == ProgramManager_gParamConfig.inputStatusCfg.inverterError)
  {
    ProgramManager_gSensorInverterErr = PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ERROR;
  }
  else
  {
    ProgramManager_gSensorInverterErr = PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR;
  }

  eMB_Master_GetDiscretesInput
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_INPUT_SENSOR_IMBALANCE_ERR_OFFSET,
    &tempValue
  );

  if (tempValue == ProgramManager_gParamConfig.inputStatusCfg.extractShock)
  {
    ProgramManager_gSensorImbalanceErr = PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ERROR;
  }
  else
  {
    ProgramManager_gSensorImbalanceErr = PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR;
  }

  eMB_Master_GetDiscretesInput
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_INPUT_SENSOR_DOOROPEN_ERR_OFFSET,
    &tempValue
  );

  if (tempValue != ProgramManager_gParamConfig.inputStatusCfg.doorClosed)
  {
    ProgramManager_gSensorDoorOpenErr = PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ERROR;
  }
  else
  {
    ProgramManager_gSensorDoorOpenErr = PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR;
  }

  ProgramManager_gIsError = ProgramManager_gSensorInverterErr | ProgramManager_gSensorImbalanceErr | ProgramManager_gSensorDoorOpenErr;
}

/*=============================================================================================*/
void ProgramManager_ControlInternal_AnalyzeTemperature(void)
{
  static uint8_t tempDelayCount = (uint8_t)2U;
  uint16_t tempTemperature;
  uint8_t  finalTemperature;

  /* Get temperature value from slave in Celsius */
  eMB_Master_GetInputRegister
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_TEMP_PRES_TEMPERATURE_OFFSET,
    &tempTemperature
  );

  if (tempDelayCount == (uint8_t)0U)
  {
    /* Simple first-order IIR filter to smooth output */
    ProgramManager_gRealTemperature = ((float)1.0f - PROGRAMMANAGER_CONTROL_IIR_FILTER_TEMP_ALPHA) * ProgramManager_gRealTemperature + \
                                      PROGRAMMANAGER_CONTROL_IIR_FILTER_TEMP_ALPHA * tempTemperature + PROGRAMMANAGER_CONTROL_IIR_FILTER_TEMP_OFFSET;

    /* Downcast value */
    finalTemperature = (uint8_t)ProgramManager_gRealTemperature;
  }
  else
  {
    /* First time after initialization get instant value */
    ProgramManager_gRealTemperature = (float)tempTemperature;

    /* Downcast value */
    finalTemperature = (uint8_t)tempTemperature;

    tempDelayCount--;
  }

  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_FAHRENHEIT)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(&finalTemperature, &ProgramManager_gCurrentTemperature);
  }
  else
  {
    ProgramManager_gCurrentTemperature = finalTemperature;
  }
}

/*=============================================================================================*/
void ProgramManager_ControlInternal_AnalyzePressure(void)
{
  static uint8_t presDelayCount = (uint8_t)2U;
  uint16_t tempPressure;

  /* Get pressure value from slave */
  eMB_Master_GetInputRegister
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_TEMP_PRES_PRESSURE_OFFSET,
    &tempPressure
  );

  if (presDelayCount == (uint8_t)0U)
  {
    /* Simple first-order IIR filter to smooth output */
    ProgramManager_gRealPressure = ((float)1.0f - PROGRAMMANAGER_CONTROL_IIR_FILTER_PRES_ALPHA) * ProgramManager_gRealPressure + \
                                      PROGRAMMANAGER_CONTROL_IIR_FILTER_PRES_ALPHA * tempPressure + PROGRAMMANAGER_CONTROL_IIR_FILTER_PRES_OFFSET;
  }
  else
  {
    /* First time after initialization get instant value */
    ProgramManager_gRealPressure = (float)tempPressure;

    presDelayCount--;
  }

  /* Downcast value */
  ProgramManager_gCurrentPressure = (uint16_t)ProgramManager_gRealPressure;
}

/*=============================================================================================*/
void ProgramManager_ControlInternal_VerifyOutput(void)
{
  uint16_t tempOutput;

  /* Get output state value from slave */
  eMB_Master_GetHoldingRegister
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_RELAY_ADDR,
    &tempOutput
  );

  if (ProgramManager_gCurrentOutput != tempOutput)
  {
    /* For now just pass */
  }
}

/*=============================================================================================*/
void ProgramManager_ControlInternal_UpdateTempThreshold(void)
{
  if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == true)
  {
    switch ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].tempMode)
    {
      case PROGRAMMANAGER_TEMP_MODE_DEFAULT:
        ProgramManager_gCurrentTempThreshold = ProgramManager_gParamConfig.heatTempCfg.tempThreshold;
        break;
      case PROGRAMMANAGER_TEMP_MODE_CUSTOM:
        ProgramManager_gCurrentTempThreshold = (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].tempThreshold;
        break;
      default:
        ProgramManager_gCurrentTempThreshold = PROGRAMMANAGER_CONTROL_TEMP_INVALID_VALUE;
        break;
    }
  }
  else
  {
    ProgramManager_gCurrentTempThreshold = PROGRAMMANAGER_CONTROL_TEMP_INVALID_VALUE;
  }
}

/*=============================================================================================*/
void ProgramManager_ControlInternal_UpdatePresThreshold(void)
{
  if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == true)
  {
    switch ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].levelMode)
    {
      case PROGRAMMANAGER_LEVEL_MODE_LOW:
        ProgramManager_gCurrentPresThreshold = ProgramManager_gParamConfig.fillLevelCfg.lowLevel;
        break;
      case PROGRAMMANAGER_LEVEL_MODE_MID:
        ProgramManager_gCurrentPresThreshold = ProgramManager_gParamConfig.fillLevelCfg.midLevel;
        break;
      case PROGRAMMANAGER_LEVEL_MODE_HIGH:
        ProgramManager_gCurrentPresThreshold = ProgramManager_gParamConfig.fillLevelCfg.highLevel;
        break;
      case PROGRAMMANAGER_LEVEL_MODE_CUSTOM:
        ProgramManager_gCurrentPresThreshold = (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].levelThreshold;
        break;
      default:
        ProgramManager_gCurrentPresThreshold = PROGRAMMANAGER_CONTROL_PRES_INVALID_VALUE;
        break;
    }
  }
  else
  {
    ProgramManager_gCurrentPresThreshold = PROGRAMMANAGER_CONTROL_PRES_INVALID_VALUE;
  }
}

/*=============================================================================================*/
void ProgramManager_ControlInternal_UpdateWashTime(void)
{
  if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == true)
  {
    switch ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].washMode)
    {
      case PROGRAMMANAGER_WASH_MODE_STANDARD:
      {
        ProgramManager_gCurrentWashRunTime = ProgramManager_gParamConfig.washCfg.stdWashRunTime;
        ProgramManager_gCurrentWashStopTime = ProgramManager_gParamConfig.washCfg.stdWashStopTime;
        ProgramManager_gCurrentWashSpeed = ProgramManager_gParamConfig.washCfg.stdWashSpeed;

        break;
      }
      case PROGRAMMANAGER_WASH_MODE_DELICATE:
      {
        ProgramManager_gCurrentWashRunTime = ProgramManager_gParamConfig.washCfg.delWashRunTime;
        ProgramManager_gCurrentWashStopTime = ProgramManager_gParamConfig.washCfg.delWashStopTime;
        ProgramManager_gCurrentWashSpeed = ProgramManager_gParamConfig.washCfg.delWashSpeed;

        break;
      }
      case PROGRAMMANAGER_WASH_MODE_HEAVY:
      {
        ProgramManager_gCurrentWashRunTime = ProgramManager_gParamConfig.washCfg.hvyWashRunTime;
        ProgramManager_gCurrentWashStopTime = ProgramManager_gParamConfig.washCfg.hvyWashStopTime;
        ProgramManager_gCurrentWashSpeed = ProgramManager_gParamConfig.washCfg.hvyWashSpeed;

        break;
      }
      case PROGRAMMANAGER_WASH_MODE_CUSTOM:
      {
        ProgramManager_gCurrentWashRunTime = (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].washRunTime;
        ProgramManager_gCurrentWashStopTime = (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].washStopTime;
        ProgramManager_gCurrentWashSpeed = (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].washSpeed;

        break;
      }
      default:
      {
        ProgramManager_gCurrentWashRunTime = (uint32_t)0U;
        ProgramManager_gCurrentWashStopTime = (uint32_t)0U;
        ProgramManager_gCurrentWashSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;

        break;
      }
    }
  }
  else
  {
    ProgramManager_gCurrentWashRunTime = (uint32_t)0U;
    ProgramManager_gCurrentWashStopTime = (uint32_t)0U;
    ProgramManager_gCurrentWashSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
  }
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void ProgramManager_Control_Init(void)
{
  ProgramManager_Control_RecvSignalCount = (uint8_t)0U;

  ProgramManager_Control_AnalyzeDataDelayStart = PROGRAMMANAGER_CONTROL_ANALYZEDATA_DELAYSTART_MAX;
  ProgramManager_Control_AnalyzeDataCount = (uint8_t)0U;

  ProgramManager_PauseOutput = (uint16_t)0U;

  ProgramManager_gRealTemperature = (float)0.0f;
  ProgramManager_gRealPressure = (float)0.0f;

  ProgramManager_gSensorInverterErr = (uint8_t)0U;
  ProgramManager_gSensorImbalanceErr = (uint8_t)0U;
  ProgramManager_gSensorDoorOpenErr = (uint8_t)0U;

  ProgramManager_gCurrentTemperature = (uint8_t)0U;
  ProgramManager_gCurrentPressure = (uint16_t)0U;

  ProgramManager_gCurrentTempThreshold = (uint8_t)0U;
  ProgramManager_gCurrentPresThreshold = (uint16_t)0U;

  ProgramManager_gTempThresExceeded = (bool)false;
  ProgramManager_gPresThresExceeded = (bool)false;

  ProgramManager_gIsPaused = (uint8_t)0U;
  ProgramManager_gIsError = (uint8_t)0U;

  ProgramManager_gCurrentOutput = (uint16_t)0U;
}



/*=============================================================================================*/
void ProgramManager_Control_SetCommand(uint8_t command)
{

}

/*=============================================================================================*/
void ProgramManager_Control_RetrieveCommand(uint8_t *command)
{

}



/*=============================================================================================*/
void ProgramManager_Control_TogglePauseResumeHandler(void)
{
  ProgramManager_gIsPaused ^= (uint8_t)0x01U;

  /* Program is not paused */
  if (ProgramManager_gIsPaused == (uint8_t)0U)
  {
    ProgramManager_gCurrentOutput = ProgramManager_PauseOutput;
  }
  /* Program is paused */
  else
  {
    ProgramManager_PauseOutput = ProgramManager_gCurrentOutput;
    ProgramManager_gCurrentOutput = (uint32_t)0U;
  }
}



/*=============================================================================================*/
bool ProgramManager_Control_CheckNextStepAvailable(void)
{
  bool retVal = false;

  if (ProgramManager_gAutoSeqConfig.currentStep != PROGRAMMANAGER_STEP_NUM_MAX)
  {
    retVal = true;
  }

  return retVal;
}

/*=============================================================================================*/
bool ProgramManager_Control_CheckPrevStepAvailable(void)
{
  bool retVal = false;
  uint8_t stepIndex;

  for (stepIndex = (uint8_t)0U; \
       stepIndex < ProgramManager_gAutoSeqConfig.currentStep; \
       stepIndex++)
  {
    if ((ProgramManager_gAutoSeqConfig.normStep)[stepIndex].isActive == true)
    {
      retVal = true;
      break;
    }
  }

  return retVal;
}



/*=============================================================================================*/
/* Task periodic time is 5ms */
void ProgramManager_Control_TxRxSignalSubMainFunction(void)
{
  ProgramManager_Control_RecvSignalCount++;

  if ( \
      (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_1) || \
      (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_2) \
     )
  {
    (void)eMB_Master_RequestReadDiscreteInputs
    (
      PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
      PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ADDR,
      PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NUM
    );
  }
  else if (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_TEMP_PRES)
  {
    (void)eMB_Master_RequestReadInputRegister
    (
      PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
      PROGRAMMANAGER_CONTROL_TEMP_PRES_ADDR,
      PROGRAMMANAGER_CONTROL_TEMP_PRES_NUM
    );
  }
  else if (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_RELAY)
  {
    (void)eMB_Master_RequestWriteHoldingRegister
    (
      PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
      PROGRAMMANAGER_CONTROL_RELAY_ADDR,
      ProgramManager_gCurrentOutput
    );
  }
  else if (ProgramManager_Control_RecvSignalCount >= PROGRAMMANAGER_CONTROL_RECVSIGNAL_COUNT_MAX)
  {
    ProgramManager_Control_RecvSignalCount = (uint8_t)0U;
  }
}

/*=============================================================================================*/
/* Task periodic time is 5ms */
void ProgramManager_Control_AnalyzeDataSubMainFunction(void)
{
  if (ProgramManager_Control_AnalyzeDataDelayStart == (uint8_t)0U)
  {
    ProgramManager_Control_AnalyzeDataCount++;

    if (ProgramManager_Control_AnalyzeDataCount >= PROGRAMMANAGER_CONTROL_ANALYZEDATA_COUNT_MAX)
    {
      ProgramManager_ControlInternal_AnalyzeSensorInput();
      ProgramManager_ControlInternal_AnalyzeTemperature();
      ProgramManager_ControlInternal_AnalyzePressure();

      ProgramManager_ControlInternal_VerifyOutput();

      ProgramManager_Control_AnalyzeDataCount = (uint8_t)0U;
    }
  }
  else
  {
    ProgramManager_Control_AnalyzeDataDelayStart--;
  }
}

/*=============================================================================================*/
void ProgramManager_Control_UpdateThresholdSubMainFunction(void)
{
  ProgramManager_ControlInternal_UpdateTempThreshold();
  ProgramManager_ControlInternal_UpdatePresThreshold();
  ProgramManager_ControlInternal_UpdateWashTime();
}



#ifdef  __cplusplus
}
#endif
