/* 
 * File:   ProgramManager_Mem.c
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



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#define PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE             (uint8_t)16U
#define PROGRAMMANAGER_CONFIG_BLOCK_SIZE                  (uint8_t)32U



/*===============================================================================================
*                                        ENUMERATIONS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

/* Default factory parameter configurations */
const ProgramManager_ParamConfigSetupStruct ProgramManager_gParamDefConfig = 
{
  .machineFuncCfg =                                                         /* Machine function setup configuration */
  {
    .drainWhileDoorOpen         = true,                                     /* Open drain valve automatically after open the door for the safety of the operator */
    .heatUseTimeout             = true,                                     /* Automatically switch to wash if timeout expired */
    .fillUseTimeout             = true,                                     /* Automatically switch to wash if timeout expired */
    .manOperateWhenAuto         = true,                                     /* Allow manually change the level, temperature and action when program in AUTO mode */
    .tempUnit                   = PROGRAMMANAGER_TEMP_UNIT_CELSIUS,         /* Temperature Unit is Celsius */
    .drainValveStatus           = PROGRAMMANAGER_RELAY_ENABLE_STAT_NC,      /* Drain valve relay enable status while draining is normal close */
  },
  .inputStatusCfg =                                                         /* Input status setup configuration */
  {
    .doorClosed                 = PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW,     /* Door closed input signal status */
    .extractShock               = PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW,     /* Extract shock input signal status */
    .emergencyStop              = PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW,     /* Emergency stop input signal status */
    .inverterError              = PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW      /* Inverter error input signal status */
  },
  .fillLevelCfg =                                                           /* Fill level setup configuration */
  {
    .autoRefillWhenLow          = true,                                     /* Automatically refill water when level is lower than set value */
    .zeroLevel                  = (uint16_t)5U,                             /* Set the relative zero level */
    .lowLevel                   = (uint16_t)20U,                            /* Set the low level for water level LOW mode */
    .midLevel                   = (uint16_t)50U,                            /* Set the middle level for water level MID mode */
    .highLevel                  = (uint16_t)80U,                            /* Set the high level for water level HIGH mode */
    .overbrimLevel              = (uint16_t)85U,                            /* Set the overbrim level. Drain valve will open when water higher than the overbrim level */
    .levelDiffRefill            = (uint16_t)3U,                             /* Automatically refill water when difference between current and set is larger than this value */
    .maxTimeFill                = (uint16_t)5U                              /* If level is lower than set value after this amount of time, controller will alarm (minutes) */
  },
  .heatTempCfg =                                                            /* Heat temperature setup configuration */
  {
    .autoReheatWhenLow          = true,                                     /* Automatically re-heat when temperature is lower than set value */
    .minWaterTemp               = (uint16_t)20U,                            /* The minimum value of water set temperature */
    .maxWaterTemp               = (uint16_t)50U,                            /* The maxumum value of water set temperature */
    .tempThreshold              = (uint16_t)30U,                            /* The water temperature threshold */
    .tempDiffReheat             = (uint16_t)2U,                             /* Auto re-heat when difference between current and set is larger than this value */
    .maxTimeHeat                = (uint16_t)5U,                             /* If temperature is lower than set value after this amount of time, controller will alarm (minutes) */
  },
  .soapCfg =                                                                /* Soap setup configuration */
  {
    .stopFillWhenSoap           = true,                                     /* Stop fill cold water while soaping to avoid low water pressure (1) */
    .timeSoap1                  = (uint16_t)20U,                            /* Default set time of soap 1 (seconds) */
    .timeSoap2                  = (uint16_t)20U,                            /* Default set time of soap 2 (seconds) */
    .timeSoap3                  = (uint16_t)20U                             /* Default set time of soap 3 (seconds) */
  },
  .washCfg =                                                                /* Wash setup configuration */
  {
    .minPauseFwdRev             = (uint16_t)5U,                             /* Minimum time pause between forward and reverse to avoid motor overload (seconds) */
    .stdWashRunTime             = (uint16_t)20U,                            /* Standard wash run time (seconds) */
    .stdWashStopTime            = (uint16_t)5U,                             /* Standard wash stop time (seconds) */
    .delWashRunTime             = (uint16_t)20U,                            /* Delicate wash run time (seconds) */
    .delWashStopTime            = (uint16_t)5U,                             /* Delicate wash stop time (seconds) */
    .hvyWashRunTime             = (uint16_t)20U,                            /* Heavy wash run time (seconds) */
    .hvyWashStopTime            = (uint16_t)5U,                             /* Heavy wash stop time (seconds). When is set to 0, it becomes single wash */
    .stdWashSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Default speed of standard wash */
    .delWashSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1,       /* Default speed of delicate wash */
    .hvyWashSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3,       /* Default speed of heavy wash */
    .maxWashSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3        /* Max speed of wash. Wash at max speed when the set value is larger than max speed */
  },
  .extractCfg =                                                             /* Extract setup configuration */
  {
    .balanceLevel               = (uint16_t)5U,                             /* Auto fill to level at least before extract to avoid shock */
    .balanceDrainLevel          = (uint16_t)20U,                            /* If the level is higher than level at most after balance drain, controller will alarm */
    .balanceWithWaterTime       = (uint16_t)10U,                            /* Balance with water time at extract (seconds) */
    .balanceDrainWaterTime      = (uint16_t)10U,                            /* Balance drain water time at extract (seconds) */
    .balanceDelayTime           = (uint16_t)5U,                             /* Delay time from end of balance (seconds) */
    .fwdRunTime                 = (uint16_t)10U,                            /* Forward run time at beginning of extract (seconds) */
    .midExtractTime             = (uint16_t)30U,                            /* Time of middle extract (seconds) */
    .midExtractDelayTime        = (uint16_t)5U,                             /* Delay time from end of mid extract (seconds) */
    .highExtractTime1           = (uint16_t)20U,                            /* Time of high extract 1 (seconds) */
    .highExtractTime2           = (uint16_t)40U,                            /* Time of high extract 2 (seconds) */
    .highExtractTime3           = (uint16_t)40U,                            /* Time of high extract 3 (seconds) */
    .highExtractDelayTime       = (uint16_t)5U,                             /* Delay time from end of high extract (seconds) */
    .maxExtractTime             = (uint16_t)60U,                            /* Max time of extract at last phase (seconds) */
    .reextractTime              = (uint16_t)3U,                             /* Re-extract times when shock. If the times is larger than the set value, controller will alarm (seconds) */
    .reextractWashTime          = (uint16_t)10U,                            /* Wash time before re-extract to unroll the clothes (seconds) */
    .balanceSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Balance speed at extract */
    .fwdRunSpeed                = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Forward run speed at beginning of extract */
    .midExtractSpeed            = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1,       /* Speed of mid extract */
    .highExtractSpeed1          = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3,       /* Speed of high extract 1 */
    .highExtractSpeed2          = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3,       /* Speed of high extract 2 */
    .highExtractSpeed3          = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3,       /* Speed of high extract 3 */
    .maxMidExtractSpeed         = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_2,       /* Max speed of mid extract */
    .maxHighExtractSpeed        = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3        /* Max speed of high extract */
  },
  .doorLockCfg =                                                            /* Door lock setup configuration */
  {
    .useLockStopButton          = true,                                     /* Use door lock controlled by pressing STOP button */
    .doorLockValveStatus        = PROGRAMMANAGER_RELAY_ENABLE_STAT_NC,      /* Door lock valve status when door is closed */
    .unlockDoorTemp             = (uint16_t)30U,                            /* Can't unlock the door when temperature is higher than safety temperature */
    .unlockDoorLevel            = (uint16_t)5U                              /* Can't unlock the door when level is higher than safety level */
  }
};

/* Default factory automatic mode sequence step configurations */
const ProgramManager_AutoSeqConfigStruct ProgramManager_gAutoSeqDefConfig =
{
  .normStep[0] =
  {
    .isActive                   = false,                                    /* This step is active during AUTO mode */
    .waterMode                  = (uint8_t)0x01U,                           /* Water mode - cold water */
    .soapMode                   = (uint8_t)0x00U,                           /* Soap mode - no soap */
    .washMode                   = PROGRAMMANAGER_WASH_MODE_STANDARD,        /* Wash mode - standard */
    .tempMode                   = PROGRAMMANAGER_TEMP_MODE_DEFAULT,         /* Soap mode - default */
    .levelMode                  = PROGRAMMANAGER_LEVEL_MODE_LOW,            /* Water level mode - low */
    .washNum                    = (uint8_t)3U,                              /* Number of wash time */
    .washRunTime                = (uint16_t)20U,                            /* Wash run time */
    .washStopTime               = (uint16_t)5U,                             /* Wash stop time */
    .washSpeed                  = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Wash speed */
    .tempThreshold              = (uint16_t)30U,                            /* Water temperature threshold */
    .levelThreshold             = (uint16_t)50U,                            /* Water level threshold */
    .balanceTime                = (uint16_t)10U,                            /* Balance time at extract */
    .midExtractTime             = (uint16_t)20U,                            /* Time of middle extract */
    .highExtractTime1           = (uint16_t)20U,                            /* Time of high extract 1 */
    .highExtractTime2           = (uint16_t)40U,                            /* Time of high extract 2 */
    .highExtractTime3           = (uint16_t)40U                             /* Time of high extract 3 */
  },
  .unloadStep =
  {
    .reverseTime                = (uint16_t)10U                             /* Time of reverse spin */
  }
};

const ProgramManager_ManualSeqConfigStruct ProgramManager_gManualSeqDefConfig =
{
  .normStep =
  {
    .isActive                   = false,                                    /* This step is active during AUTO mode */
    .waterMode                  = (uint8_t)0x01U,                           /* Water mode - cold water */
    .soapMode                   = (uint8_t)0x00U,                           /* Soap mode - no soap */
    .washMode                   = PROGRAMMANAGER_WASH_MODE_STANDARD,        /* Wash mode - standard */
    .tempMode                   = PROGRAMMANAGER_TEMP_MODE_DEFAULT,         /* Soap mode - default */
    .levelMode                  = PROGRAMMANAGER_LEVEL_MODE_LOW,            /* Water level mode - low */
    .washNum                    = (uint8_t)3U,                              /* Number of wash time */
    .washRunTime                = (uint16_t)20U,                            /* Wash run time */
    .washStopTime               = (uint16_t)5U,                             /* Wash stop time */
    .washSpeed                  = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Wash speed */
    .tempThreshold              = (uint16_t)30U,                            /* Water temperature threshold */
    .levelThreshold             = (uint16_t)50U,                            /* Water level threshold */
    .balanceTime                = (uint16_t)10U,                            /* Balance time at extract */
    .midExtractTime             = (uint16_t)20U,                            /* Time of middle extract */
    .highExtractTime1           = (uint16_t)20U,                            /* Time of high extract 1 */
    .highExtractTime2           = (uint16_t)40U,                            /* Time of high extract 2 */
    .highExtractTime3           = (uint16_t)40U                             /* Time of high extract 3 */
  }
};

ProgramManager_ParamConfigSetupStruct ProgramManager_gParamConfig;
ProgramManager_AutoSeqConfigStruct ProgramManager_gAutoSeqConfig;
ProgramManager_ManualSeqConfigStruct ProgramManager_gManualSeqConfig;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

HAL_StatusTypeDef ProgramManager_ParamConfigSetup_GetData(ProgramManager_ParamConfigSetupStruct *data)
{
  (void)ProgramManager_MachineFuncSetup_GetData (&(data->machineFuncCfg));
  (void)ProgramManager_InputStatusSetup_GetData (&(data->inputStatusCfg));
  (void)ProgramManager_FillLevelSetup_GetData   (&(data->fillLevelCfg));
  (void)ProgramManager_HeatTempSetup_GetData    (&(data->heatTempCfg), (data->machineFuncCfg).tempUnit);
  (void)ProgramManager_SoapSetup_GetData        (&(data->soapCfg));
  (void)ProgramManager_WashSetup_GetData        (&(data->washCfg));
  (void)ProgramManager_ExtractSetup_GetData     (&(data->extractCfg));
  (void)ProgramManager_DoorLockSetup_GetData    (&(data->doorLockCfg));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ParamConfigSetup_SetData(ProgramManager_ParamConfigSetupStruct *data)
{
  (void)ProgramManager_MachineFuncSetup_SetData (&(data->machineFuncCfg));
  (void)ProgramManager_InputStatusSetup_SetData (&(data->inputStatusCfg));
  (void)ProgramManager_FillLevelSetup_SetData   (&(data->fillLevelCfg));
  (void)ProgramManager_HeatTempSetup_SetData    (&(data->heatTempCfg), (data->machineFuncCfg).tempUnit);
  (void)ProgramManager_SoapSetup_SetData        (&(data->soapCfg));
  (void)ProgramManager_WashSetup_SetData        (&(data->washCfg));
  (void)ProgramManager_ExtractSetup_SetData     (&(data->extractCfg));
  (void)ProgramManager_DoorLockSetup_SetData    (&(data->doorLockCfg));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_GetData(ProgramManager_MachineFuncSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_MACHINEFUNCSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  data->drainWhileDoorOpen    = (bool)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINWHILEDOOROPEN_OFFSET];
  data->heatUseTimeout        = (bool)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_HEATUSETIMEOUT_OFFSET];
  data->fillUseTimeout        = (bool)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_FILLUSETIMEOUT_OFFSET];
  data->manOperateWhenAuto    = (bool)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_OFFSET];
  data->tempUnit              = (ProgramManager_TempUnitType)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_OFFSET];
  data->drainValveStatus      = (ProgramManager_RelayEnableStatusType)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_OFFSET];
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_SetData(ProgramManager_MachineFuncSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINWHILEDOOROPEN_OFFSET]  = (uint8_t)(data->drainWhileDoorOpen);
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_HEATUSETIMEOUT_OFFSET]      = (uint8_t)(data->heatUseTimeout    );
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_FILLUSETIMEOUT_OFFSET]      = (uint8_t)(data->fillUseTimeout    );
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_OFFSET]  = (uint8_t)(data->manOperateWhenAuto);
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_OFFSET]            = (uint8_t)(data->tempUnit          );
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_OFFSET]    = (uint8_t)(data->drainValveStatus  );

  extMemIf.writeByteArray(PROGRAMMANAGER_MACHINEFUNCSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainWhileDoorOpen_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINWHILEDOOROPEN_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainWhileDoorOpen_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINWHILEDOOROPEN_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_HeatUseTimeout_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_MACHINEFUNCSETUP_HEATUSETIMEOUT_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_HeatUseTimeout_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_MACHINEFUNCSETUP_HEATUSETIMEOUT_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_FillUseTimeout_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_MACHINEFUNCSETUP_FILLUSETIMEOUT_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_FillUseTimeout_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_MACHINEFUNCSETUP_FILLUSETIMEOUT_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_ManOperateWhenAuto_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_ManOperateWhenAuto_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_TempUnit_GetData(ProgramManager_TempUnitType *data)
{
  *data = (ProgramManager_TempUnitType)(extMemIf.readByte(PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_TempUnit_SetData(ProgramManager_TempUnitType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainValveStatus_GetData(ProgramManager_RelayEnableStatusType *data)
{
  *data = (ProgramManager_RelayEnableStatusType)(extMemIf.readByte(PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainValveStatus_SetData(ProgramManager_RelayEnableStatusType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_InputStatusSetup_GetData(ProgramManager_InputStatusSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_INPUTSTATUSSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  data->doorClosed    = (ProgramManager_InputActiveStatusType)recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_OFFSET];
  data->extractShock  = (ProgramManager_InputActiveStatusType)recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_OFFSET];
  data->emergencyStop = (ProgramManager_InputActiveStatusType)recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_OFFSET];
  data->inverterError = (ProgramManager_InputActiveStatusType)recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_OFFSET];
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_SetData(ProgramManager_InputStatusSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_OFFSET]    = (uint8_t)(data->doorClosed   );
  recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_OFFSET]  = (uint8_t)(data->extractShock );
  recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_OFFSET] = (uint8_t)(data->emergencyStop);
  recvArr[PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_OFFSET] = (uint8_t)(data->inverterError);
  
  extMemIf.writeByteArray(PROGRAMMANAGER_INPUTSTATUSSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_DoorClosed_GetData(ProgramManager_InputActiveStatusType *data)
{
  *data = (ProgramManager_InputActiveStatusType)(extMemIf.readByte(PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_DoorClosed_SetData(ProgramManager_InputActiveStatusType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_ExtractShock_GetData(ProgramManager_InputActiveStatusType *data)
{
  *data = (ProgramManager_InputActiveStatusType)(extMemIf.readByte(PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_ExtractShock_SetData(ProgramManager_InputActiveStatusType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_EmergencyStop_GetData(ProgramManager_InputActiveStatusType *data)
{
  *data = (ProgramManager_InputActiveStatusType)(extMemIf.readByte(PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_EmergencyStop_SetData(ProgramManager_InputActiveStatusType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_InverterError_GetData(ProgramManager_InputActiveStatusType *data)
{
  *data = (ProgramManager_InputActiveStatusType)(extMemIf.readByte(PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_InputStatusSetup_InverterError_SetData(ProgramManager_InputActiveStatusType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_FillLevelSetup_GetData(ProgramManager_FillLevelSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_FILLLEVELSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  data->autoRefillWhenLow   = (bool)recvArr[PROGRAMMANAGER_FILLLEVELSETUP_AUTOREFILLWHENLOW_OFFSET];

  data->zeroLevel           = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET]) << 8U;
  data->zeroLevel          |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET + 1]);

  data->lowLevel            = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET]) << 8U;
  data->lowLevel           |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET + 1]);

  data->midLevel            = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET]) << 8U;
  data->midLevel           |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET + 1]);

  data->highLevel           = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET]) << 8U;
  data->highLevel          |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET + 1]);

  data->overbrimLevel       = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_OFFSET]) << 8U;
  data->overbrimLevel      |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_OFFSET + 1]);

  data->levelDiffRefill     = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET]) << 8U;
  data->levelDiffRefill    |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET + 1]);

  data->maxTimeFill         = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_OFFSET]) << 8U;
  data->maxTimeFill        |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_OFFSET + 1]);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_SetData(ProgramManager_FillLevelSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_AUTOREFILLWHENLOW_OFFSET]   = (uint8_t)(data->autoRefillWhenLow);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET]           = (uint8_t)(data->zeroLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET + 1]       = (uint8_t)(data->zeroLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET]            = (uint8_t)(data->lowLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET + 1]        = (uint8_t)(data->lowLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET]            = (uint8_t)(data->midLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET + 1]        = (uint8_t)(data->midLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET]           = (uint8_t)(data->highLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET + 1]       = (uint8_t)(data->highLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_OFFSET]       = (uint8_t)(data->overbrimLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_OFFSET + 1]   = (uint8_t)(data->overbrimLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET]     = (uint8_t)(data->levelDiffRefill >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET + 1] = (uint8_t)(data->levelDiffRefill & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_OFFSET]         = (uint8_t)(data->maxTimeFill >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_OFFSET + 1]     = (uint8_t)(data->maxTimeFill & (uint16_t)0x00FFU);

  extMemIf.writeByteArray(PROGRAMMANAGER_FILLLEVELSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_AutoRefillWhenLow_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_FILLLEVELSETUP_AUTOREFILLWHENLOW_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_AutoRefillWhenLow_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_FILLLEVELSETUP_AUTOREFILLWHENLOW_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_ZeroLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_ZeroLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_LowLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_LowLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_MidLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_MidLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_HighLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_HighLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_OverbrimLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_OverbrimLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_LevelDiffRefill_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_LevelDiffRefill_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_MaxTimeFill_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_MaxTimeFill_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_BASE_ADDR, *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_HeatTempSetup_GetData(ProgramManager_HeatTempSetupStruct *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_HEATTEMPSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  data->autoReheatWhenLow     = (bool)recvArr[PROGRAMMANAGER_HEATTEMPSETUP_AUTOREHEATWHENLOW_OFFSET];

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    data->minWaterTemp        = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET];
    data->maxWaterTemp        = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET];
    data->tempThreshold       = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET];
    data->tempDiffReheat      = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET];
  }
  else
  {
    data->minWaterTemp        = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET + 1];
    data->maxWaterTemp        = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET + 1];
    data->tempThreshold       = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET + 1];
    data->tempDiffReheat      = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET + 1];
  }

  data->maxTimeHeat           = (uint16_t)(recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_OFFSET]) << 8U;
  data->maxTimeHeat          |= (uint16_t)(recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_OFFSET + 1]);

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_SetData(ProgramManager_HeatTempSetupStruct *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_HEATTEMPSETUP_AUTOREHEATWHENLOW_OFFSET]      = (uint8_t)(data->autoReheatWhenLow);

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET]         = data->minWaterTemp;
    ProgramManager_Common_CelsiusToFahrenheitConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET + 1)
    );

    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET]         = data->maxWaterTemp;
    ProgramManager_Common_CelsiusToFahrenheitConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET + 1)
    );

    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET]        = data->tempThreshold;
    ProgramManager_Common_CelsiusToFahrenheitConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET + 1)
    );

    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET]       = data->tempDiffReheat;
    ProgramManager_Common_CelsiusToFahrenheitConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET + 1)
    );
  }
  else
  {
    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET + 1]     = data->minWaterTemp;
    ProgramManager_Common_FahrenheitToCelsiusConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET + 1),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET)
    );

    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET + 1]     = data->maxWaterTemp;
    ProgramManager_Common_FahrenheitToCelsiusConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET + 1),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET)
    );

    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET + 1]    = data->tempThreshold;
    ProgramManager_Common_FahrenheitToCelsiusConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET + 1),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET)
    );

    recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET + 1]   = data->tempDiffReheat;
    ProgramManager_Common_FahrenheitToCelsiusConv
    (
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET + 1),
      (recvArr + PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET)
    );
  }

  recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_OFFSET]        = (uint8_t)(data->maxTimeHeat >> 8U);
  recvArr[PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_OFFSET + 1]    = (uint8_t)(data->maxTimeHeat & (uint16_t)0x00FFU);

  extMemIf.writeByteArray(PROGRAMMANAGER_HEATTEMPSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_AutoReheatWhenLow_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_AUTOREHEATWHENLOW_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_AutoReheatWhenLow_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_AUTOREHEATWHENLOW_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_MinWaterTemp_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_BASE_ADDR);
  }
  else
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_BASE_ADDR + 1);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_MinWaterTemp_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t temp;

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_BASE_ADDR, *data);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_BASE_ADDR + 1, temp);
  }
  else
  {
    ProgramManager_Common_FahrenheitToCelsiusConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_BASE_ADDR, temp);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_BASE_ADDR + 1, *data);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxWaterTemp_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_BASE_ADDR);
  }
  else
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_BASE_ADDR + 1);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxWaterTemp_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t temp;

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_BASE_ADDR, *data);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_BASE_ADDR + 1, temp);
  }
  else
  {
    ProgramManager_Common_FahrenheitToCelsiusConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_BASE_ADDR, temp);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_BASE_ADDR + 1, *data);
  }

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempThreshold_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR);
  }
  else
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR + 1);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempThreshold_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t temp;

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR, *data);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR + 1, temp);
  }
  else
  {
    ProgramManager_Common_FahrenheitToCelsiusConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR, temp);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR + 1, *data);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempDiffReheat_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR);
  }
  else
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR + 1);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempDiffReheat_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t temp;

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR, *data);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR + 1, temp);
  }
  else
  {
    ProgramManager_Common_FahrenheitToCelsiusConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR, temp);
    extMemIf.writeByte(PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR + 1, *data);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxTimeHeat_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxTimeHeat_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_BASE_ADDR, *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_SoapSetup_GetData(ProgramManager_SoapSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_SOAPSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  data->stopFillWhenSoap  = (bool)recvArr[PROGRAMMANAGER_SOAPSETUP_STOPFILLWHENSOAP_OFFSET];

  data->timeSoap1         = (uint16_t)(recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_OFFSET]) << 8U;
  data->timeSoap1        |= (uint16_t)(recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_OFFSET + 1]);

  data->timeSoap2         = (uint16_t)(recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_OFFSET]) << 8U;
  data->timeSoap2        |= (uint16_t)(recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_OFFSET + 1]);

  data->timeSoap3         = (uint16_t)(recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_OFFSET]) << 8U;
  data->timeSoap3        |= (uint16_t)(recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_OFFSET + 1]);

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_SetData(ProgramManager_SoapSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_SOAPSETUP_STOPFILLWHENSOAP_OFFSET]  = (uint8_t)(data->stopFillWhenSoap);

  recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_OFFSET]         = (uint8_t)(data->timeSoap1 >> 8U);
  recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_OFFSET + 1]     = (uint8_t)(data->timeSoap1 & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_OFFSET]         = (uint8_t)(data->timeSoap2 >> 8U);
  recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_OFFSET + 1]     = (uint8_t)(data->timeSoap2 & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_OFFSET]         = (uint8_t)(data->timeSoap3 >> 8U);
  recvArr[PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_OFFSET + 1]     = (uint8_t)(data->timeSoap3 & (uint16_t)0x00FFU);

  extMemIf.writeByteArray(PROGRAMMANAGER_SOAPSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_StopFillWhenSoap_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_SOAPSETUP_STOPFILLWHENSOAP_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_StopFillWhenSoap_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_SOAPSETUP_STOPFILLWHENSOAP_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap1_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap1_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap2_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap2_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap3_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap3_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_BASE_ADDR, *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_WashSetup_GetData(ProgramManager_WashSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_WASHSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  data->minPauseFwdRev    = (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_OFFSET]) << 8U;
  data->minPauseFwdRev   |= (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_OFFSET + 1]);

  data->stdWashRunTime    = (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_OFFSET]) << 8U;
  data->stdWashRunTime   |= (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_OFFSET + 1]);

  data->stdWashStopTime   = (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_OFFSET]) << 8U;
  data->stdWashStopTime  |= (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_OFFSET + 1]);

  data->delWashRunTime    = (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_OFFSET]) << 8U;
  data->delWashRunTime   |= (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_OFFSET + 1]);

  data->delWashStopTime   = (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_OFFSET]) << 8U;
  data->delWashStopTime  |= (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_OFFSET + 1]);

  data->hvyWashRunTime    = (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_OFFSET]) << 8U;
  data->hvyWashRunTime   |= (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_OFFSET + 1]);

  data->hvyWashStopTime   = (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_OFFSET]) << 8U;
  data->hvyWashStopTime  |= (uint16_t)(recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_OFFSET + 1]);

  data->stdWashSpeed      = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_OFFSET];
  data->delWashSpeed      = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_OFFSET];
  data->hvyWashSpeed      = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_OFFSET];
  data->maxWashSpeed      = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_WASHSETUP_MAXWASHSPEED_OFFSET];

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_SetData(ProgramManager_WashSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_OFFSET]       = (uint8_t)(data->minPauseFwdRev >> 8U);
  recvArr[PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_OFFSET + 1]   = (uint8_t)(data->minPauseFwdRev & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_OFFSET]       = (uint8_t)(data->stdWashRunTime >> 8U);
  recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_OFFSET + 1]   = (uint8_t)(data->stdWashRunTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_OFFSET]      = (uint8_t)(data->stdWashStopTime >> 8U);
  recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_OFFSET + 1]  = (uint8_t)(data->stdWashStopTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_OFFSET]       = (uint8_t)(data->delWashRunTime >> 8U);
  recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_OFFSET + 1]   = (uint8_t)(data->delWashRunTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_OFFSET]      = (uint8_t)(data->delWashStopTime >> 8U);
  recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_OFFSET + 1]  = (uint8_t)(data->delWashStopTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_OFFSET]       = (uint8_t)(data->hvyWashRunTime >> 8U);
  recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_OFFSET + 1]   = (uint8_t)(data->hvyWashRunTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_OFFSET]      = (uint8_t)(data->hvyWashStopTime >> 8U);
  recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_OFFSET + 1]  = (uint8_t)(data->hvyWashStopTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_OFFSET]         = (uint8_t)(data->stdWashSpeed);
  recvArr[PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_OFFSET]         = (uint8_t)(data->delWashSpeed);
  recvArr[PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_OFFSET]         = (uint8_t)(data->hvyWashSpeed);
  recvArr[PROGRAMMANAGER_WASHSETUP_MAXWASHSPEED_OFFSET]         = (uint8_t)(data->maxWashSpeed);

  extMemIf.writeByteArray(PROGRAMMANAGER_WASHSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_MinPauseFwdRev_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_MinPauseFwdRev_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_StdWashRunTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_StdWashRunTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_StdWashStopTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_StdWashStopTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_DelWashRunTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_DelWashRunTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_DelWashStopTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_DelWashStopTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashRunTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashRunTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashStopTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashStopTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_StdWashSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_StdWashSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_DelWashSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_DelWashSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_MaxWashSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_WASHSETUP_MAXWASHSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_MaxWashSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_WASHSETUP_MAXWASHSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_ExtractSetup_GetData(ProgramManager_ExtractSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_EXTRACTSETUP_PART1_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  data->balanceLevel            = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_OFFSET]) << 8U;
  data->balanceLevel           |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_OFFSET + 1]);

  data->balanceDrainLevel       = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_OFFSET]) << 8U;
  data->balanceDrainLevel      |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_OFFSET + 1]);

  data->balanceWithWaterTime    = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_OFFSET]) << 8U;
  data->balanceWithWaterTime   |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_OFFSET + 1]);

  data->balanceDrainWaterTime   = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_OFFSET]) << 8U;
  data->balanceDrainWaterTime  |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_OFFSET + 1]);

  data->balanceDelayTime        = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_OFFSET]) << 8U;
  data->balanceDelayTime       |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_OFFSET + 1]);

  data->fwdRunTime              = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_OFFSET]) << 8U;
  data->fwdRunTime             |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_OFFSET + 1]);

  data->midExtractTime          = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_OFFSET]) << 8U;
  data->midExtractTime         |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_OFFSET + 1]);

  data->midExtractDelayTime     = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_OFFSET]) << 8U;
  data->midExtractDelayTime    |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_OFFSET + 1]);

  data->highExtractTime1        = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_OFFSET]) << 8U;
  data->highExtractTime1       |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_OFFSET + 1]);

  data->highExtractTime2        = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_OFFSET]) << 8U;
  data->highExtractTime2       |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_OFFSET + 1]);

  data->highExtractTime3        = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_OFFSET]) << 8U;
  data->highExtractTime3       |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_OFFSET + 1]);

  data->highExtractDelayTime    = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_OFFSET]) << 8U;
  data->highExtractDelayTime   |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_OFFSET + 1]);

  data->maxExtractTime          = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_OFFSET]) << 8U;
  data->maxExtractTime         |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_OFFSET + 1]);

  data->reextractTime           = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_OFFSET]) << 8U;
  data->reextractTime          |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_OFFSET + 1]);

  data->reextractWashTime       = (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_OFFSET]) << 8U;
  data->reextractWashTime      |= (uint16_t)(recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_OFFSET + 1]);

  extMemIf.readByteArray(PROGRAMMANAGER_EXTRACTSETUP_PART2_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  data->balanceSpeed            = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCESPEED_OFFSET];
  data->fwdRunSpeed             = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_FWDRUNSPEED_OFFSET];
  data->midExtractSpeed         = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTSPEED_OFFSET];
  data->highExtractSpeed1       = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED1_OFFSET];
  data->highExtractSpeed2       = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED2_OFFSET];
  data->highExtractSpeed3       = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED3_OFFSET];
  data->maxMidExtractSpeed      = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXMIDEXTRACTSPEED_OFFSET];
  data->maxHighExtractSpeed     = (ProgramManager_MotorSpeedType)recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXHIGHEXTRACTSPEED_OFFSET];

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_SetData(ProgramManager_ExtractSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_OFFSET]              = (uint8_t)(data->balanceLevel >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_OFFSET + 1]          = (uint8_t)(data->balanceLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_OFFSET]         = (uint8_t)(data->balanceDrainLevel >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_OFFSET + 1]     = (uint8_t)(data->balanceDrainLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_OFFSET]      = (uint8_t)(data->balanceWithWaterTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_OFFSET + 1]  = (uint8_t)(data->balanceWithWaterTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_OFFSET]     = (uint8_t)(data->balanceDrainWaterTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_OFFSET + 1] = (uint8_t)(data->balanceDrainWaterTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_OFFSET]          = (uint8_t)(data->balanceDelayTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_OFFSET + 1]      = (uint8_t)(data->balanceDelayTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_OFFSET]                = (uint8_t)(data->fwdRunTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_OFFSET + 1]            = (uint8_t)(data->fwdRunTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_OFFSET]            = (uint8_t)(data->midExtractTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_OFFSET + 1]        = (uint8_t)(data->midExtractTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_OFFSET]       = (uint8_t)(data->midExtractDelayTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_OFFSET + 1]   = (uint8_t)(data->midExtractDelayTime & (uint16_t)0x00FFU);
  
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_OFFSET]          = (uint8_t)(data->highExtractTime1 >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_OFFSET + 1]      = (uint8_t)(data->highExtractTime1 & (uint16_t)0x00FFU);
  
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_OFFSET]          = (uint8_t)(data->highExtractTime2 >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_OFFSET + 1]      = (uint8_t)(data->highExtractTime2 & (uint16_t)0x00FFU);
  
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_OFFSET]          = (uint8_t)(data->highExtractTime3 >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_OFFSET + 1]      = (uint8_t)(data->highExtractTime3 & (uint16_t)0x00FFU);
  
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_OFFSET]      = (uint8_t)(data->highExtractDelayTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_OFFSET + 1]  = (uint8_t)(data->highExtractDelayTime & (uint16_t)0x00FFU);
  
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_OFFSET]            = (uint8_t)(data->maxExtractTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_OFFSET + 1]        = (uint8_t)(data->maxExtractTime & (uint16_t)0x00FFU);
  
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_OFFSET]             = (uint8_t)(data->reextractTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_OFFSET + 1]         = (uint8_t)(data->reextractTime & (uint16_t)0x00FFU);
  
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_OFFSET]         = (uint8_t)(data->reextractWashTime >> 8U);
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_OFFSET + 1]     = (uint8_t)(data->reextractWashTime & (uint16_t)0x00FFU);

  extMemIf.writeByteArray(PROGRAMMANAGER_EXTRACTSETUP_PART1_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  memset(recvArr, (uint8_t)0U, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  recvArr[PROGRAMMANAGER_EXTRACTSETUP_BALANCESPEED_OFFSET]              = (uint8_t)(data->balanceSpeed       );
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_FWDRUNSPEED_OFFSET]               = (uint8_t)(data->fwdRunSpeed        );
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTSPEED_OFFSET]           = (uint8_t)(data->midExtractSpeed    );
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED1_OFFSET]         = (uint8_t)(data->highExtractSpeed1  );
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED2_OFFSET]         = (uint8_t)(data->highExtractSpeed2  );
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED3_OFFSET]         = (uint8_t)(data->highExtractSpeed3  );
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXMIDEXTRACTSPEED_OFFSET]        = (uint8_t)(data->maxMidExtractSpeed );
  recvArr[PROGRAMMANAGER_EXTRACTSETUP_MAXHIGHEXTRACTSPEED_OFFSET]       = (uint8_t)(data->maxHighExtractSpeed);

  extMemIf.writeByteArray(PROGRAMMANAGER_EXTRACTSETUP_PART2_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceWithWaterTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceWithWaterTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainWaterTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainWaterTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDelayTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDelayTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractDelayTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractDelayTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime1_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime1_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime2_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime2_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime3_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime3_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractDelayTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractDelayTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxExtractTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxExtractTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractWashTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractWashTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_BALANCESPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_BALANCESPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_FWDRUNSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_FWDRUNSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed1_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED1_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed1_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED1_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed2_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED2_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed2_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED2_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed3_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED3_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed3_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED3_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxMidExtractSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_MAXMIDEXTRACTSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxMidExtractSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_MAXMIDEXTRACTSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxHighExtractSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_EXTRACTSETUP_MAXHIGHEXTRACTSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxHighExtractSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_EXTRACTSETUP_MAXHIGHEXTRACTSPEED_BASE_ADDR, (uint8_t) *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_DoorLockSetup_GetData(ProgramManager_DoorLockSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_DOORLOCKSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  data->useLockStopButton     = (bool)recvArr[PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_OFFSET];
  data->doorLockValveStatus   = (ProgramManager_RelayEnableStatusType)recvArr[PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_OFFSET];

  data->unlockDoorTemp        = (uint16_t)(recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET]) << 8U;
  data->unlockDoorTemp       |= (uint16_t)(recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET + 1]);

  data->unlockDoorLevel       = (uint16_t)(recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET]) << 8U;
  data->unlockDoorLevel      |= (uint16_t)(recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET + 1]);

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_SetData(ProgramManager_DoorLockSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_OFFSET]   = (uint8_t)(data->useLockStopButton);
  recvArr[PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_OFFSET] = (uint8_t)(data->doorLockValveStatus);

  recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET]      = (uint8_t)(data->unlockDoorTemp >> 8U);
  recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET + 1]  = (uint8_t)(data->unlockDoorTemp & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET]     = (uint8_t)(data->unlockDoorLevel >> 8U);
  recvArr[PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET + 1] = (uint8_t)(data->unlockDoorLevel & (uint16_t)0x00FFU);

  extMemIf.writeByteArray(PROGRAMMANAGER_DOORLOCKSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UseLockStopButton_GetData(bool *data)
{
  *data = (bool)(extMemIf.readByte(PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_OFFSET));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UseLockStopButton_SetData(bool *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_OFFSET, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_DoorLockValveStatus_GetData(ProgramManager_RelayEnableStatusType *data)
{
  *data = (ProgramManager_RelayEnableStatusType)(extMemIf.readByte(PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_OFFSET));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_DoorLockValveStatus_SetData(ProgramManager_RelayEnableStatusType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_OFFSET, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorTemp_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET);
  }
  else
  {
    *data = extMemIf.readByte(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET + 1);
  }

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorTemp_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t temp;

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET, *data);
    extMemIf.writeByte(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET + 1, temp);
  }
  else
  {
    ProgramManager_Common_FahrenheitToCelsiusConv(data, &temp);

    extMemIf.writeByte(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET, temp);
    extMemIf.writeByte(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET + 1, *data);
  }

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorLevel_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorLevel_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET, *data);
  
  return HAL_OK;
}





HAL_StatusTypeDef ProgramManager_AutoSeqConfig_GetData(ProgramManager_AutoSeqConfigStruct *data)
{
  uint8_t stepIndex;

  ProgramManager_SequenceIndex_GetData(&(data->sequenceIndex));

  data->currentStep = (uint8_t)0U;

  for (stepIndex = (uint8_t)0U; stepIndex < PROGRAMMANAGER_STEP_NUM_MAX; stepIndex++)
  {
    ProgramManager_NormStepConfig_GetData(data->sequenceIndex, stepIndex, &(data->normStep)[stepIndex], ProgramManager_gParamConfig.machineFuncCfg.tempUnit);
  }

  ProgramManager_UnloadStepConfig_GetData(data->sequenceIndex, &(data->unloadStep));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_AutoSeqConfig_SetData(ProgramManager_AutoSeqConfigStruct *data)
{
  uint8_t sequenceIndex;
  uint8_t stepIndex;

  ProgramManager_SequenceIndex_SetData((uint8_t)0U);

  for (sequenceIndex = (uint8_t)0U; sequenceIndex < PROGRAMMANAGER_SEQUENCE_NUM_MAX; sequenceIndex++)
  {
    for (stepIndex = (uint8_t)0U; stepIndex < PROGRAMMANAGER_STEP_NUM_MAX; stepIndex++)
    {
      ProgramManager_NormStepConfig_SetData(sequenceIndex, stepIndex, &(data->normStep)[stepIndex], ProgramManager_gParamConfig.machineFuncCfg.tempUnit);
    }

    ProgramManager_UnloadStepConfig_SetData(sequenceIndex, &(data->unloadStep));
  }

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SequenceIndex_GetData(uint8_t *data)
{
  *data = extMemIf.readByte(PROGRAMMANAGER_SEQUENCE_CURRENTINDEX_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SequenceIndex_SetData(uint8_t *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_SEQUENCE_CURRENTINDEX_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit)
{
  uint16_t addr;
  uint8_t  recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE];

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx);

  extMemIf.readByteArray(addr, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  /* Check if sequence index is from AUTO or MANUAL mode configuration */
  if (seqIdx < PROGRAMMANAGER_SEQUENCE_NUM_MAX)
  {
    data->coldWaterMode       = PROGRAMMANAGER_COMMON_MODE_AUTO;
    data->hotWaterMode        = PROGRAMMANAGER_COMMON_MODE_AUTO;
    data->heatMode            = PROGRAMMANAGER_COMMON_MODE_AUTO;
    data->soap1Mode           = PROGRAMMANAGER_COMMON_MODE_AUTO;
    data->soap2Mode           = PROGRAMMANAGER_COMMON_MODE_AUTO;
    data->soap3Mode           = PROGRAMMANAGER_COMMON_MODE_AUTO;
    data->drainMode           = PROGRAMMANAGER_COMMON_MODE_AUTO;
  }
  else
  {
    data->coldWaterMode       = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->hotWaterMode        = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->heatMode            = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->soap1Mode           = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->soap2Mode           = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->soap3Mode           = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->drainMode           = PROGRAMMANAGER_COMMON_MODE_MANUAL;
  }

  data->timeoutCountMode      = false;
  data->timeoutMinute         = (uint8_t)0U;
  data->timeoutSecond         = (uint8_t)0U;

  data->isActive              = (bool)(recvArr[PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET]);

  data->waterMode             = (uint8_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WATERMODE_OFFSET]);
  data->soapMode              = (uint8_t)(recvArr[PROGRAMMANAGER_NORMSTEP_SOAPMODE_OFFSET]);
  data->washMode              = (ProgramManager_WashModeType)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHMODE_OFFSET]);
  data->tempMode              = (ProgramManager_TempModeType)(recvArr[PROGRAMMANAGER_NORMSTEP_TEMPMODE_OFFSET]);
  data->levelMode             = (ProgramManager_LevelModeType)(recvArr[PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET]);

  data->washNum               = recvArr[PROGRAMMANAGER_NORMSTEP_WASHNUM_OFFSET];

  data->washRunTime           = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHRUNTIME_OFFSET]) << 8U;
  data->washRunTime          |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHRUNTIME_OFFSET + 1]);

  data->washStopTime          = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHSTOPTIME_OFFSET]) << 8U;
  data->washStopTime         |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHSTOPTIME_OFFSET + 1]);

  data->washSpeed             = (ProgramManager_MotorSpeedType)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHSPEED_OFFSET]);

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    data->tempThreshold       = recvArr[PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET];
  }
  else
  {
    data->tempThreshold       = recvArr[PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET + 1];
  }

  data->levelThreshold        = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET]) << 8U;
  data->levelThreshold       |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET + 1]);

  data->balanceTime           = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_BALANCETIME_OFFSET]) << 8U;
  data->balanceTime          |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_BALANCETIME_OFFSET + 1]);

  data->midExtractTime        = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_MIDEXTRACTTIME_OFFSET]) << 8U;
  data->midExtractTime       |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_MIDEXTRACTTIME_OFFSET + 1]);

  data->highExtractTime1      = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME1_OFFSET]) << 8U;
  data->highExtractTime1     |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME1_OFFSET + 1]);

  data->highExtractTime2      = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME2_OFFSET]) << 8U;
  data->highExtractTime2     |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME2_OFFSET + 1]);

  data->highExtractTime3      = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME3_OFFSET]) << 8U;
  data->highExtractTime3     |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME3_OFFSET + 1]);

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit)
{
  uint16_t addr;
  uint8_t  recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET]                    = (uint8_t)(data->isActive    );

  recvArr[PROGRAMMANAGER_NORMSTEP_WATERMODE_OFFSET]                   = (uint8_t)(data->waterMode   );
  recvArr[PROGRAMMANAGER_NORMSTEP_SOAPMODE_OFFSET]                    = (uint8_t)(data->soapMode    );
  recvArr[PROGRAMMANAGER_NORMSTEP_WASHMODE_OFFSET]                    = (uint8_t)(data->washMode    );
  recvArr[PROGRAMMANAGER_NORMSTEP_TEMPMODE_OFFSET]                    = (uint8_t)(data->tempMode    );
  recvArr[PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET]                   = (uint8_t)(data->levelMode   );

  recvArr[PROGRAMMANAGER_NORMSTEP_WASHNUM_OFFSET]                     = (uint8_t)(data->washNum     );

  recvArr[PROGRAMMANAGER_NORMSTEP_WASHRUNTIME_OFFSET]                 = (uint8_t)(data->washRunTime >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_WASHRUNTIME_OFFSET + 1]             = (uint8_t)(data->washRunTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_NORMSTEP_WASHSTOPTIME_OFFSET]                = (uint8_t)(data->washStopTime >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_WASHSTOPTIME_OFFSET + 1]            = (uint8_t)(data->washStopTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_NORMSTEP_WASHSPEED_OFFSET]                   = (uint8_t)(data->washSpeed   );

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    recvArr[PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET]             = data->tempThreshold;
    ProgramManager_Common_CelsiusToFahrenheitConv
    (
      (recvArr + PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET),
      (recvArr + PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET + 1)
    );
  }
  else
  {
    recvArr[PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET + 1]         = data->tempThreshold;
    ProgramManager_Common_FahrenheitToCelsiusConv
    (
      (recvArr + PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET + 1),
      (recvArr + PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET)
    );
  }

  recvArr[PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET]              = (uint8_t)(data->levelThreshold >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET + 1]          = (uint8_t)(data->levelThreshold & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_NORMSTEP_BALANCETIME_OFFSET]                 = (uint8_t)(data->balanceTime >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_BALANCETIME_OFFSET + 1]             = (uint8_t)(data->balanceTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_NORMSTEP_MIDEXTRACTTIME_OFFSET]              = (uint8_t)(data->midExtractTime >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_MIDEXTRACTTIME_OFFSET + 1]          = (uint8_t)(data->midExtractTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME1_OFFSET]            = (uint8_t)(data->highExtractTime1 >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME1_OFFSET + 1]        = (uint8_t)(data->highExtractTime1 & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME2_OFFSET]            = (uint8_t)(data->highExtractTime2 >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME2_OFFSET + 1]        = (uint8_t)(data->highExtractTime2 & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME3_OFFSET]            = (uint8_t)(data->highExtractTime3 >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME3_OFFSET + 1]        = (uint8_t)(data->highExtractTime3 & (uint16_t)0x00FFU);

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx);

  extMemIf.writeByteArray(addr, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_IsActive_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET;

  *data = (bool)(extMemIf.readByte(addr));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_IsActive_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET;

  extMemIf.writeByte(addr, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WaterMode_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WATERMODE_OFFSET;

  *data = extMemIf.readByte(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WaterMode_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WATERMODE_OFFSET;

  extMemIf.writeByte(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_SoapMode_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_SOAPMODE_OFFSET;

  *data = extMemIf.readByte(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_SoapMode_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_SOAPMODE_OFFSET;

  extMemIf.writeByte(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_WashModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHMODE_OFFSET;

  *data = (ProgramManager_WashModeType)(extMemIf.readByte(addr));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_WashModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHMODE_OFFSET;

  extMemIf.writeByte(addr, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_TempMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_TempModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_TEMPMODE_OFFSET;

  *data = (ProgramManager_TempModeType)(extMemIf.readByte(addr));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_TempMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_TempModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_TEMPMODE_OFFSET;

  extMemIf.writeByte(addr, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_LevelModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET;

  *data = (ProgramManager_LevelModeType)(extMemIf.readByte(addr));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_LevelModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET;

  extMemIf.writeByte(addr, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashNum_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHNUM_OFFSET;

  *data = extMemIf.readByte(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashNum_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHNUM_OFFSET;

  extMemIf.writeByte(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashRunTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHRUNTIME_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashRunTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHRUNTIME_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashStopTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHSTOPTIME_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashStopTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHSTOPTIME_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashSpeed_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_MotorSpeedType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHSPEED_OFFSET;

  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(addr));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashSpeed_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_MotorSpeedType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHSPEED_OFFSET;

  extMemIf.writeByte(addr, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_TempThreshold_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET;

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    *data = extMemIf.readByte(addr);
  }
  else
  {
    *data = extMemIf.readByte(addr + 1);
  }
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_TempThreshold_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t temp;
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET;

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    ProgramManager_Common_CelsiusToFahrenheitConv(data, &temp);

    extMemIf.writeByte(addr, *data);
    extMemIf.writeByte(addr + 1, temp);
  }
  else
  {
    ProgramManager_Common_FahrenheitToCelsiusConv(data, &temp);

    extMemIf.writeByte(addr, temp);
    extMemIf.writeByte(addr + 1, *data);
  }

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelThreshold_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelThreshold_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_BalanceTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_BALANCETIME_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_BalanceTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_BALANCETIME_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_MidExtractTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_MIDEXTRACTTIME_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_MidExtractTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_MIDEXTRACTTIME_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_HighExtractTime1_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME1_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_HighExtractTime1_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME1_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_HighExtractTime2_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME2_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_HighExtractTime2_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME2_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_HighExtractTime3_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME3_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_HighExtractTime3_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME3_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_UnloadStepConfig_GetData(uint8_t seqIdx, ProgramManager_UnloadStepConfigStruct *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         +  PROGRAMMANAGER_UNLOADSTEP_OFFSET;

  data->reverseTime = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_UnloadStepConfig_SetData(uint8_t seqIdx, ProgramManager_UnloadStepConfigStruct *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         +  PROGRAMMANAGER_UNLOADSTEP_OFFSET;

  extMemIf.writeInteger(addr, data->reverseTime);
  
  return HAL_OK;
}



HAL_StatusTypeDef ProgramManager_ManualSeqConfig_GetData(ProgramManager_ManualSeqConfigStruct *data)
{
  ProgramManager_NormStepConfig_GetData(PROGRAMMANAGER_SEQUENCE_NUM_MAX, (uint8_t)0U, &(data->normStep), ProgramManager_gParamConfig.machineFuncCfg.tempUnit);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ManualSeqConfig_SetData(ProgramManager_ManualSeqConfigStruct *data)
{
  ProgramManager_NormStepConfig_SetData(PROGRAMMANAGER_SEQUENCE_NUM_MAX, (uint8_t)0U, &(data->normStep), ProgramManager_gParamConfig.machineFuncCfg.tempUnit);
  
  return HAL_OK;
}



#ifdef  __cplusplus
}
#endif
