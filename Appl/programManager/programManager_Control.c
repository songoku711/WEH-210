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



/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

static uint8_t ProgramManager_Control_RecvSignalCount;

static uint8_t ProgramManager_Control_AnalyzeDataDelayStart;
static uint8_t ProgramManager_Control_AnalyzeDataCount;

static float   ProgramManager_gRealTemperature;
static float   ProgramManager_gRealPressure;

uint8_t ProgramManager_gSensorInverterErr;
uint8_t ProgramManager_gSensorImbalanceErr;
uint8_t ProgramManager_gSensorDoorOpenErr;
uint8_t ProgramManager_gCurrentTemperature;
uint8_t ProgramManager_gCurrentPressure;

uint16_t ProgramManager_gCurrentOutput;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

void ProgramManager_ControlInternal_AnalyzeSensorInput(void);
void ProgramManager_ControlInternal_AnalyzeTemperature(void);
void ProgramManager_ControlInternal_AnalyzePressure(void);
void ProgramManager_ControlInternal_VerifiedOutput(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

void ProgramManager_ControlInternal_AnalyzeSensorInput(void)
{
  eMB_Master_GetDiscretesInput
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_INPUT_SENSOR_INVERTER_ERR_OFFSET,
    &ProgramManager_gSensorInverterErr
  );

  eMB_Master_GetDiscretesInput
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_INPUT_SENSOR_IMBALANCE_ERR_OFFSET,
    &ProgramManager_gSensorImbalanceErr
  );

  eMB_Master_GetDiscretesInput
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_INPUT_SENSOR_DOOROPEN_ERR_OFFSET,
    &ProgramManager_gSensorDoorOpenErr
  );
}

void ProgramManager_ControlInternal_AnalyzeTemperature(void)
{
  uint16_t tempTemperature;
  uint8_t  finalTemperature;

  /* Get temperature value from slave in Celsius */
  eMB_Master_GetInputRegister
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_TEMP_PRES_TEMPERATURE_OFFSET,
    &tempTemperature
  );

  /* Simple first-order IIR filter to smooth output */
  ProgramManager_gRealTemperature = ((float)1.0f - PROGRAMMANAGER_CONTROL_IIR_FILTER_TEMP_ALPHA) * ProgramManager_gRealTemperature + \
                                    PROGRAMMANAGER_CONTROL_IIR_FILTER_TEMP_ALPHA * tempTemperature;

  /* Downcast value */
  finalTemperature = (uint8_t)ProgramManager_gRealTemperature;

  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_FAHRENHEIT)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(&finalTemperature, &ProgramManager_gCurrentTemperature);
  }
  else
  {
    ProgramManager_gCurrentTemperature = finalTemperature;
  }
}

void ProgramManager_ControlInternal_AnalyzePressure(void)
{
  uint16_t tempPressure;

  /* Get pressure value from slave */
  eMB_Master_GetInputRegister
  (
    PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
    PROGRAMMANAGER_CONTROL_TEMP_PRES_PRESSURE_OFFSET,
    &tempPressure
  );

  /* Simple first-order IIR filter to smooth output */
  ProgramManager_gRealPressure = ((float)1.0f - PROGRAMMANAGER_CONTROL_IIR_FILTER_PRES_ALPHA) * ProgramManager_gRealPressure + \
                                    PROGRAMMANAGER_CONTROL_IIR_FILTER_PRES_ALPHA * tempPressure;

  /* Downcast value */
  ProgramManager_gCurrentPressure = (uint16_t)ProgramManager_gRealPressure;
}

void ProgramManager_ControlInternal_VerifiedOutput(void)
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



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void ProgramManager_Control_Init(void)
{
  ProgramManager_Control_RecvSignalCount = (uint8_t)0U;

  ProgramManager_Control_AnalyzeDataDelayStart = PROGRAMMANAGER_CONTROL_ANALYZEDATA_DELAYSTART_MAX;
  ProgramManager_Control_AnalyzeDataCount = (uint8_t)0U;
}

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

      ProgramManager_ControlInternal_VerifiedOutput();

      ProgramManager_Control_AnalyzeDataCount = (uint8_t)0U;
    }
  }
  else
  {
    ProgramManager_Control_AnalyzeDataDelayStart--;
  }
}



#ifdef  __cplusplus
}
#endif
