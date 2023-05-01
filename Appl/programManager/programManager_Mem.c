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
    .manOperateWhenAuto         = true,                                     /* Allow manually change the level, temperature and action when program in AUTO mode */
    .tempUnit                   = PROGRAMMANAGER_TEMP_UNIT_CELSIUS,         /* Temperature Unit is Celsius */
    .drainValveStatus           = PROGRAMMANAGER_RELAY_ENABLE_STAT_NC,      /* Drain valve relay enable status while draining is normal close */
    .washMachine                = PROGRAMMANAGER_WASHING_MACHINE_MOTOR      /* Washing machine type - motor */
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
    .zeroLevel                  = (uint16_t)5U,                             /* Set the relative zero level */
    .lowLevel                   = (uint16_t)20U,                            /* Set the low level for water level LOW mode */
    .midLevel                   = (uint16_t)50U,                            /* Set the middle level for water level MID mode */
    .highLevel                  = (uint16_t)80U,                            /* Set the high level for water level HIGH mode */
    .levelDiffRefill            = (uint16_t)3U,                             /* Automatically refill water when difference between current and set is larger than this value */
  },
  .heatTempCfg =                                                            /* Heat temperature setup configuration */
  {
    .tempThreshold              = (uint16_t)30U,                            /* The water temperature threshold */
    .tempDiffReheat             = (uint16_t)2U,                             /* Auto re-heat when difference between current and set is larger than this value */
    .maxTimeHeat                = (uint16_t)5U,                             /* If temperature is lower than set value after this amount of time, controller will alarm (minutes) */
  },
  .washCfg =                                                                /* Wash setup configuration */
  {
    .stdWashRunTime             = (uint16_t)20U,                            /* Standard wash run time (seconds) */
    .stdWashStopTime            = (uint16_t)5U,                             /* Standard wash stop time (seconds) */
    .delWashRunTime             = (uint16_t)20U,                            /* Delicate wash run time (seconds) */
    .delWashStopTime            = (uint16_t)5U,                             /* Delicate wash stop time (seconds) */
    .hvyWashRunTime             = (uint16_t)20U,                            /* Heavy wash run time (seconds) */
    .hvyWashStopTime            = (uint16_t)5U,                             /* Heavy wash stop time (seconds). When is set to 0, it becomes single wash */
    .stdWashSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Default speed of standard wash */
    .delWashSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1,       /* Default speed of delicate wash */
    .hvyWashSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3,       /* Default speed of heavy wash */
  },
  .drainCfg =                                                               /* Drain setup configuration */
  {
    .drainStepCfg[PROGRAMMANAGER_STEP_DRAINSTEP_FIRST_DRAIN_IDX] =          /* Drain step configuration */
    {
      .drainTime                = (uint16_t)20U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0        /* Drain running speed */
    },
    .drainStepCfg[PROGRAMMANAGER_STEP_DRAINSTEP_FORWARD_DRAIN_IDX] =        /* Drain step configuration */
    {
      .drainTime                = (uint16_t)10U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0        /* Drain running speed */
    },
    .drainStepCfg[PROGRAMMANAGER_STEP_DRAINSTEP_BALANCE_DRAIN_IDX] =        /* Drain step configuration */
    {
      .drainTime                = (uint16_t)10U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1        /* Drain running speed */
    },
    .drainStepCfg[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL1_DRAIN_IDX] =      /* Drain step configuration */
    {
      .drainTime                = (uint16_t)20U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1        /* Drain running speed */
    },
    .drainStepCfg[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX] =      /* Drain step configuration */
    {
      .drainTime                = (uint16_t)40U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_2        /* Drain running speed */
    },
    .drainStepCfg[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX] =      /* Drain step configuration */
    {
      .drainTime                = (uint16_t)40U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3        /* Drain running speed */
    },
    .drainOffTime               = (uint16_t)5U,                             /* Drain off time */
    .maxDrainExtrTime           = (uint16_t)300U,                           /* Max time of extract drain */
    .reDrainExtrTime            = (uint16_t)3U,                             /* Re-extract times when shock */
    .maxDrainExtrSpeed          = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3        /* Max speed of extract drain */
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
    .drainMode                  = PROGRAMMANAGER_DRAIN_MODE_DEFAULT,        /* Drain mode - default */
    .tempMode                   = PROGRAMMANAGER_TEMP_MODE_DEFAULT,         /* Temp mode - default */
    .levelMode                  = PROGRAMMANAGER_LEVEL_MODE_LOW,            /* Water level mode - low */
    .washTime                   = (uint16_t)300U,                           /* Wash time */
    .washRunTime                = (uint16_t)20U,                            /* Wash run time */
    .washStopTime               = (uint16_t)5U,                             /* Wash stop time */
    .washSpeed                  = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Wash speed */
    .tempThreshold              = (uint16_t)30U,                            /* Water temperature threshold */
    .levelThreshold             = (uint16_t)50U,                            /* Water level threshold */
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_FIRST_DRAIN_IDX] =             /* Drain step configuration */
    {
      .drainTime                = (uint16_t)20U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_FORWARD_DRAIN_IDX] =           /* Drain step configuration */
    {
      .drainTime                = (uint16_t)10U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_BALANCE_DRAIN_IDX] =           /* Drain step configuration */
    {
      .drainTime                = (uint16_t)10U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL1_DRAIN_IDX] =         /* Drain step configuration */
    {
      .drainTime                = (uint16_t)20U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX] =         /* Drain step configuration */
    {
      .drainTime                = (uint16_t)40U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_2        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX] =         /* Drain step configuration */
    {
      .drainTime                = (uint16_t)40U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3        /* Drain running speed */
    }
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
    .drainMode                  = PROGRAMMANAGER_DRAIN_MODE_DEFAULT,        /* Drain mode - default */
    .tempMode                   = PROGRAMMANAGER_TEMP_MODE_DEFAULT,         /* Temp mode - default */
    .levelMode                  = PROGRAMMANAGER_LEVEL_MODE_LOW,            /* Water level mode - low */
    .washTime                   = (uint16_t)300U,                           /* Wash time */
    .washRunTime                = (uint16_t)20U,                            /* Wash run time */
    .washStopTime               = (uint16_t)5U,                             /* Wash stop time */
    .washSpeed                  = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,       /* Wash speed */
    .tempThreshold              = (uint16_t)30U,                            /* Water temperature threshold */
    .levelThreshold             = (uint16_t)50U,                            /* Water level threshold */
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_FIRST_DRAIN_IDX] =             /* Drain step configuration */
    {
      .drainTime                = (uint16_t)20U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_FORWARD_DRAIN_IDX] =           /* Drain step configuration */
    {
      .drainTime                = (uint16_t)10U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_BALANCE_DRAIN_IDX] =           /* Drain step configuration */
    {
      .drainTime                = (uint16_t)10U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL1_DRAIN_IDX] =         /* Drain step configuration */
    {
      .drainTime                = (uint16_t)20U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX] =         /* Drain step configuration */
    {
      .drainTime                = (uint16_t)40U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_2        /* Drain running speed */
    },
    .drainStep[PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX] =         /* Drain step configuration */
    {
      .drainTime                = (uint16_t)40U,                            /* Drain running time */
      .drainSpeed               = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3        /* Drain running speed */
    }
  }
};



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
  (void)ProgramManager_WashSetup_GetData        (&(data->washCfg));
  (void)ProgramManager_DrainSetup_GetData       (&(data->drainCfg));
  (void)ProgramManager_DoorLockSetup_GetData    (&(data->doorLockCfg));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_ParamConfigSetup_SetData(ProgramManager_ParamConfigSetupStruct *data)
{
  (void)ProgramManager_MachineFuncSetup_SetData (&(data->machineFuncCfg));
  (void)ProgramManager_InputStatusSetup_SetData (&(data->inputStatusCfg));
  (void)ProgramManager_FillLevelSetup_SetData   (&(data->fillLevelCfg));
  (void)ProgramManager_HeatTempSetup_SetData    (&(data->heatTempCfg), (data->machineFuncCfg).tempUnit);
  (void)ProgramManager_WashSetup_SetData        (&(data->washCfg));
  (void)ProgramManager_DrainSetup_SetData       (&(data->drainCfg));
  (void)ProgramManager_DoorLockSetup_SetData    (&(data->doorLockCfg));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_GetData(ProgramManager_MachineFuncSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_MACHINEFUNCSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  data->manOperateWhenAuto    = (bool)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_OFFSET];
  data->tempUnit              = (ProgramManager_TempUnitType)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_OFFSET];
  data->drainValveStatus      = (ProgramManager_RelayEnableStatusType)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_OFFSET];
  data->washMachine           = (ProgramManager_WashingMachineType)recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_WASHMACHINE_OFFSET];
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_SetData(ProgramManager_MachineFuncSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_OFFSET]  = (uint8_t)(data->manOperateWhenAuto);
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_OFFSET]            = (uint8_t)(data->tempUnit          );
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_OFFSET]    = (uint8_t)(data->drainValveStatus  );
  recvArr[PROGRAMMANAGER_MACHINEFUNCSETUP_WASHMACHINE_OFFSET]         = (uint8_t)(data->washMachine       );

  extMemIf.writeByteArray(PROGRAMMANAGER_MACHINEFUNCSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);
  
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

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_WashMachine_GetData(ProgramManager_WashingMachineType *data)
{
  *data = (ProgramManager_WashingMachineType)(extMemIf.readByte(PROGRAMMANAGER_MACHINEFUNCSETUP_WASHMACHINE_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_WashMachine_SetData(ProgramManager_WashingMachineType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_MACHINEFUNCSETUP_WASHMACHINE_BASE_ADDR, (uint8_t) *data);
  
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

  data->zeroLevel           = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET]) << 8U;
  data->zeroLevel          |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET + 1]);

  data->lowLevel            = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET]) << 8U;
  data->lowLevel           |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET + 1]);

  data->midLevel            = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET]) << 8U;
  data->midLevel           |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET + 1]);

  data->highLevel           = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET]) << 8U;
  data->highLevel          |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET + 1]);

  data->levelDiffRefill     = (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET]) << 8U;
  data->levelDiffRefill    |= (uint16_t)(recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET + 1]);

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_FillLevelSetup_SetData(ProgramManager_FillLevelSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET]           = (uint8_t)(data->zeroLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET + 1]       = (uint8_t)(data->zeroLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET]            = (uint8_t)(data->lowLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET + 1]        = (uint8_t)(data->lowLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET]            = (uint8_t)(data->midLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET + 1]        = (uint8_t)(data->midLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET]           = (uint8_t)(data->highLevel >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET + 1]       = (uint8_t)(data->highLevel & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET]     = (uint8_t)(data->levelDiffRefill >> 8U);
  recvArr[PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET + 1] = (uint8_t)(data->levelDiffRefill & (uint16_t)0x00FFU);

  extMemIf.writeByteArray(PROGRAMMANAGER_FILLLEVELSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);
  
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



HAL_StatusTypeDef ProgramManager_HeatTempSetup_GetData(ProgramManager_HeatTempSetupStruct *data, ProgramManager_TempUnitType tempUnit)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_HEATTEMPSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_HALF_BLOCK_SIZE);

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    data->tempThreshold       = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET];
    data->tempDiffReheat      = recvArr[PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET];
  }
  else
  {
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

  if (tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
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



HAL_StatusTypeDef ProgramManager_WashSetup_GetData(ProgramManager_WashSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_WASHSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

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

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_WashSetup_SetData(ProgramManager_WashSetupStruct *data)
{
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE] = { 0U };

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

  extMemIf.writeByteArray(PROGRAMMANAGER_WASHSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);
  
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



HAL_StatusTypeDef ProgramManager_DrainSetup_GetData(ProgramManager_DrainSetupStruct *data)
{
  uint8_t drainStepIdx;
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE];

  extMemIf.readByteArray(PROGRAMMANAGER_DRAINSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  for (drainStepIdx = (uint8_t)0U; drainStepIdx < PROGRAMMANAGER_STEP_DRAINSTEP_NUM_MAX; drainStepIdx++)
  {
    (data->drainStepCfg)[drainStepIdx].drainTime  = (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U)]) << 8U;
    (data->drainStepCfg)[drainStepIdx].drainTime |= (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U) + 1]);
  
    (data->drainStepCfg)[drainStepIdx].drainSpeed = (ProgramManager_MotorSpeedType)(recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_OFFSET + drainStepIdx]);
  }

  data->drainOffTime            = (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_OFFSET]) << 8U;
  data->drainOffTime           |= (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_OFFSET + 1]);

  data->maxDrainExtrTime        = (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_OFFSET]) << 8U;
  data->maxDrainExtrTime       |= (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_OFFSET + 1]);

  data->reDrainExtrTime         = (recvArr[PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_OFFSET]);
  data->maxDrainExtrSpeed       = (ProgramManager_MotorSpeedType)(recvArr[PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRSPEED_OFFSET]);

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_SetData(ProgramManager_DrainSetupStruct *data)
{
  uint8_t drainStepIdx;
  uint8_t recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE] = { 0U };

  for (drainStepIdx = (uint8_t)0U; drainStepIdx < PROGRAMMANAGER_STEP_DRAINSTEP_NUM_MAX; drainStepIdx++)
  {
    recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U)]       = (uint8_t)((data->drainStepCfg)[drainStepIdx].drainTime >> 8U);
    recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U) + 1]   = (uint8_t)((data->drainStepCfg)[drainStepIdx].drainTime & (uint16_t)0x00FFU);

    recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_OFFSET + drainStepIdx]              = (uint8_t)((data->drainStepCfg)[drainStepIdx].drainSpeed);
  }

  recvArr[PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_OFFSET]              = (uint8_t)(data->drainOffTime >> 8U);
  recvArr[PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_OFFSET + 1]          = (uint8_t)(data->drainOffTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_OFFSET]          = (uint8_t)(data->maxDrainExtrTime >> 8U);
  recvArr[PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_OFFSET + 1]      = (uint8_t)(data->maxDrainExtrTime & (uint16_t)0x00FFU);

  recvArr[PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_OFFSET]           = data->reDrainExtrTime;
  
  recvArr[PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRSPEED_OFFSET]         = (uint8_t)(data->maxDrainExtrSpeed);
  
  extMemIf.writeByteArray(PROGRAMMANAGER_DRAINSETUP_BASE_ADDR, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_DrainTime_GetData(uint8_t drainStep, uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_BASE_ADDR + (uint16_t)(drainStep << 1U));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_DrainTime_SetData(uint8_t drainStep, uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_BASE_ADDR + (uint16_t)(drainStep << 1U), *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_DrainSpeed_GetData(uint8_t drainStep, ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readInteger(PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_BASE_ADDR + (uint16_t)drainStep));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_DrainSpeed_SetData(uint8_t drainStep, ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_BASE_ADDR + (uint16_t)drainStep, (uint8_t) *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_DrainOffTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_DrainOffTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrTime_GetData(uint16_t *data)
{
  *data = extMemIf.readInteger(PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrTime_SetData(uint16_t *data)
{
  extMemIf.writeInteger(PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_ReDrainExtrTime_GetData(uint8_t *data)
{
  *data = extMemIf.readByte(PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_ReDrainExtrTime_SetData(uint8_t *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_BASE_ADDR, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrSpeed_GetData(ProgramManager_MotorSpeedType *data)
{
  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRSPEED_BASE_ADDR));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrSpeed_SetData(ProgramManager_MotorSpeedType *data)
{
  extMemIf.writeByte(PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRSPEED_BASE_ADDR, (uint8_t) *data);
  
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
      ProgramManager_NormStepConfig_SetData(sequenceIndex, stepIndex, &(data->normStep)[0], ProgramManager_gParamConfig.machineFuncCfg.tempUnit);
    }
  }

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SequenceIndex_GetData(uint8_t *data)
{
  *data = extMemIf.readByte(PROGRAMMANAGER_SEQUENCE_CURRENTINDEX_BASE_ADDR);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_SequenceIndex_SetData(uint8_t data)
{
  extMemIf.writeByte(PROGRAMMANAGER_SEQUENCE_CURRENTINDEX_BASE_ADDR, data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit)
{
  uint16_t addr;
  uint8_t  drainStepIdx;
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
    data->drainComMode        = PROGRAMMANAGER_COMMON_MODE_AUTO;
  }
  else
  {
    data->coldWaterMode       = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->hotWaterMode        = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->heatMode            = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->soap1Mode           = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->soap2Mode           = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->soap3Mode           = PROGRAMMANAGER_COMMON_MODE_MANUAL;
    data->drainComMode        = PROGRAMMANAGER_COMMON_MODE_MANUAL;
  }

  data->isActive              = (bool)(recvArr[PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET]);

  data->waterMode             = (uint8_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WATERMODE_OFFSET]);
  data->soapMode              = (uint8_t)(recvArr[PROGRAMMANAGER_NORMSTEP_SOAPMODE_OFFSET]);
  data->washMode              = (ProgramManager_WashModeType)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHMODE_OFFSET]);
  data->drainMode             = (ProgramManager_DrainModeType)(recvArr[PROGRAMMANAGER_NORMSTEP_DRAINMODE_OFFSET]);
  data->tempMode              = (ProgramManager_TempModeType)(recvArr[PROGRAMMANAGER_NORMSTEP_TEMPMODE_OFFSET]);
  data->levelMode             = (ProgramManager_LevelModeType)(recvArr[PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET]);

  data->washTime              = (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHTIME_OFFSET]) << 8U;
  data->washTime             |= (uint16_t)(recvArr[PROGRAMMANAGER_NORMSTEP_WASHTIME_OFFSET + 1]);

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

  memset(recvArr, (uint8_t)0U, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_OFFSET;

  extMemIf.readByteArray(addr, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  for (drainStepIdx = (uint8_t)0U; drainStepIdx < PROGRAMMANAGER_STEP_DRAINSTEP_NUM_MAX; drainStepIdx++)
  {
    (data->drainStep)[drainStepIdx].drainTime  = (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U)]) << 8U;
    (data->drainStep)[drainStepIdx].drainTime |= (uint16_t)(recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U) + 1]);
  
    (data->drainStep)[drainStepIdx].drainSpeed = (ProgramManager_MotorSpeedType)(recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_OFFSET + drainStepIdx]);
  }

  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit)
{
  uint16_t addr;
  uint8_t  drainStepIdx;
  uint8_t  recvArr[PROGRAMMANAGER_CONFIG_BLOCK_SIZE] = { 0U };

  recvArr[PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET]                    = (uint8_t)(data->isActive);

  recvArr[PROGRAMMANAGER_NORMSTEP_WATERMODE_OFFSET]                   = (uint8_t)(data->waterMode);
  recvArr[PROGRAMMANAGER_NORMSTEP_SOAPMODE_OFFSET]                    = (uint8_t)(data->soapMode);
  recvArr[PROGRAMMANAGER_NORMSTEP_WASHMODE_OFFSET]                    = (uint8_t)(data->washMode);
  recvArr[PROGRAMMANAGER_NORMSTEP_DRAINMODE_OFFSET]                   = (uint8_t)(data->drainMode);
  recvArr[PROGRAMMANAGER_NORMSTEP_TEMPMODE_OFFSET]                    = (uint8_t)(data->tempMode);
  recvArr[PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET]                   = (uint8_t)(data->levelMode);

  recvArr[PROGRAMMANAGER_NORMSTEP_WASHTIME_OFFSET]                    = (uint8_t)(data->washTime >> 8U);
  recvArr[PROGRAMMANAGER_NORMSTEP_WASHTIME_OFFSET + 1]                = (uint8_t)(data->washTime & (uint16_t)0x00FFU);

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

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx);

  extMemIf.writeByteArray(addr, recvArr, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  memset(recvArr, (uint8_t)0U, PROGRAMMANAGER_CONFIG_BLOCK_SIZE);

  for (drainStepIdx = (uint8_t)0U; drainStepIdx < PROGRAMMANAGER_STEP_DRAINSTEP_NUM_MAX; drainStepIdx++)
  {
    recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U)]       = (uint8_t)((data->drainStep)[drainStepIdx].drainTime >> 8U);
    recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET + (drainStepIdx << 1U) + 1]   = (uint8_t)((data->drainStep)[drainStepIdx].drainTime & (uint16_t)0x00FFU);

    recvArr[PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_OFFSET + drainStepIdx]              = (uint8_t)((data->drainStep)[drainStepIdx].drainSpeed);
  }

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_OFFSET;
  
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

HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_DrainModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINMODE_OFFSET;

  *data = (ProgramManager_DrainModeType)(extMemIf.readByte(addr));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_DrainModeType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINMODE_OFFSET;

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

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHTIME_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_WashTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_WASHTIME_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
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

HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_OFFSET \
         + (PROGRAMMANAGER_NORMSTEP_DRAINSTEP_BLOCK_SIZE * drainStepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_DRAINTIME_OFFSET;

  *data = extMemIf.readInteger(addr);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, uint16_t *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_OFFSET \
         + (PROGRAMMANAGER_NORMSTEP_DRAINSTEP_BLOCK_SIZE * drainStepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_DRAINTIME_OFFSET;

  extMemIf.writeInteger(addr, *data);
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainSpeed_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, ProgramManager_MotorSpeedType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_OFFSET \
         + (PROGRAMMANAGER_NORMSTEP_DRAINSTEP_BLOCK_SIZE * drainStepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_DRAINSPEED_OFFSET;

  *data = (ProgramManager_MotorSpeedType)(extMemIf.readByte(addr));
  
  return HAL_OK;
}

HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainSpeed_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, ProgramManager_MotorSpeedType *data)
{
  uint16_t addr;

  addr =    PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR \
         + (PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE * seqIdx) \
         + (PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE * stepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_OFFSET \
         + (PROGRAMMANAGER_NORMSTEP_DRAINSTEP_BLOCK_SIZE * drainStepIdx) \
         +  PROGRAMMANAGER_NORMSTEP_DRAINSTEP_DRAINSPEED_OFFSET;

  extMemIf.writeByte(addr, (uint8_t) *data);
  
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
