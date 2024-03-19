/* 
 * File:   ProgramManager_Types.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef PROGRAMMANAGER_TYPES_H
#define PROGRAMMANAGER_TYPES_H

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



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

/* Maximum number of steps in a sequence */
#define PROGRAMMANAGER_STEP_NUM_MAX                                   (uint8_t)9U

/* Maximum number of sequences that controller stores */
#define PROGRAMMANAGER_SEQUENCE_NUM_MAX                               (uint8_t)6U

/* Maximum number of drain steps in a step */
#define PROGRAMMANAGER_STEP_DRAINSTEP_NUM_MAX                         (uint8_t)6U

#define PROGRAMMANAGER_STEP_DRAINSTEP_FORWARD_DRAIN_IDX               (0U)
#define PROGRAMMANAGER_STEP_DRAINSTEP_BALANCE_DRAIN_IDX               (1U)
#define PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL1_DRAIN_IDX             (2U)
#define PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL2_DRAIN_IDX             (3U)
#define PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL3_DRAIN_IDX             (4U)
#define PROGRAMMANAGER_STEP_DRAINSTEP_EXTR_LVL4_DRAIN_IDX             (5U)








/* Temperature unit type */
typedef enum _ProgramManager_TempUnitType
{
  PROGRAMMANAGER_TEMP_UNIT_CELSIUS,                                   /* Temperature unit is Celsius degree */
  PROGRAMMANAGER_TEMP_UNIT_FAHRENHEIT,                                /* Temperature unit is Fahrenheit degree */
  PROGRAMMANAGER_TEMP_UNIT_NUM                                        /* Number of temperature units */
} ProgramManager_TempUnitType;

/* Relay enable status type */
typedef enum _ProgramManager_RelayEnableStatusType
{
  PROGRAMMANAGER_RELAY_ENABLE_STAT_NO,                                /* Relay enable status is normal open */
  PROGRAMMANAGER_RELAY_ENABLE_STAT_NC,                                /* Relay enable status is normal close */
  PROGRAMMANAGER_RELAY_ENABLE_STAT_NUM                                /* Number of relay enable statuses */
} ProgramManager_RelayEnableStatusType;

/* Input active status type */
typedef enum _ProgramManager_InputActiveStatusType
{
  PROGRAMMANAGER_INPUT_ACTIVE_STAT_LOW,                               /* Input active status is low level */
  PROGRAMMANAGER_INPUT_ACTIVE_STAT_HIGH,                              /* Input active status is high level */
  PROGRAMMANAGER_INPUT_ACTIVE_STAT_NUM                                /* Number of input active statuses */
} ProgramManager_InputActiveStatusType;

/* Motor speed level type */
typedef enum _ProgramManager_MotorSpeedType
{
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0,                                 /* Motor speed level 0 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1,                                 /* Motor speed level 1 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_2,                                 /* Motor speed level 2 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3,                                 /* Motor speed level 3 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_4,                                 /* Motor speed level 4 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_5,                                 /* Motor speed level 5 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_6,                                 /* Motor speed level 6 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_7,                                 /* Motor speed level 7 */
  PROGRAMMANAGER_MOTOR_SPEED_LEVEL_NUM                                /* Number of motor speed levels */
} ProgramManager_MotorSpeedType;

/* Program error type */
typedef enum _ProgramManager_ErrorType
{
  PROGRAMMANAGER_ERROR_NONE                       = (uint8_t)0x00,    /* No error */
  PROGRAMMANAGER_ERROR_DOOR_STATUS                = (uint8_t)0x01,    /* Please check the door switch or door pin */
  PROGRAMMANAGER_ERROR_OVERSHOCK                  = (uint8_t)0x03,    /* Please check the capacity of machine, or the clothes knot, or check the input of over shock */
  PROGRAMMANAGER_ERROR_EMERGENCY_STOP             = (uint8_t)0x05,    /* Pressed the emergency button, please check and release it. Or check the input of emergency stop */
  PROGRAMMANAGER_ERROR_INVERTER                   = (uint8_t)0x07,    /* Please check the inverter, or check the input of inverter */
  PROGRAMMANAGER_ERROR_LEVEL_SENSOR               = (uint8_t)0x09,    /* Have not adjusted the level sensor, please contact with factory */
  PROGRAMMANAGER_ERROR_TEMP_SENSOR                = (uint8_t)0x0A,    /* Please check temperature sensor of water, or try another one */
  PROGRAMMANAGER_ERROR_FILL_OVERTIME              = (uint8_t)0x0D,    /* Can't reach the set level in time. Please check the fill valve, or check the level sensor */
  PROGRAMMANAGER_ERROR_HEAT_OVERTIME              = (uint8_t)0x0E,    /* Can't reach the set temperature in time. Please check the device of heat, or check the temperature sensor */
  PROGRAMMANAGER_ERROR_DRAIN_OVERTIME             = (uint8_t)0x0F,    /* There are also some water in basket when drain of extract. Please check the drain valve */
} ProgramManager_ErrorType;

/* Program parameter common mode type */
typedef enum _ProgramManager_CommonModeType
{
  PROGRAMMANAGER_COMMON_MODE_AUTO,                                    /* Common mode is automatic */
  PROGRAMMANAGER_COMMON_MODE_MANUAL,                                  /* Common mode is manual */
  PROGRAMMANAGER_COMMON_MODE_NUM                                      /* Number of common modes */
} ProgramManager_CommonModeType;

/* Program wash mode type */
typedef enum _ProgramManager_WashModeType
{
  PROGRAMMANAGER_WASH_MODE_STANDARD,                                  /* Wash run time and stop time is standard type */
  PROGRAMMANAGER_WASH_MODE_CUSTOM,                                    /* Wash run time and stop time is custom type */
  PROGRAMMANAGER_WASH_MODE_NUM                                        /* Number of wash time modes */
} ProgramManager_WashModeType;

/* Program water level mode type */
typedef enum _ProgramManager_LevelModeType
{
  PROGRAMMANAGER_LEVEL_MODE_LOW,                                      /* Water level is default low type */
  PROGRAMMANAGER_LEVEL_MODE_MID,                                      /* Water level is default middle type */
  PROGRAMMANAGER_LEVEL_MODE_HIGH,                                     /* Water level is default high type */
  PROGRAMMANAGER_LEVEL_MODE_CUSTOM,                                   /* Water level is default custom type */
  PROGRAMMANAGER_LEVEL_MODE_NUM                                       /* Number of water level modes */
} ProgramManager_LevelModeType;

/* Program water temp mode type */
typedef enum _ProgramManager_TempModeType
{
  PROGRAMMANAGER_TEMP_MODE_DEFAULT,                                   /* Water temp is default type */
  PROGRAMMANAGER_TEMP_MODE_CUSTOM,                                    /* Water temp is custom type */
  PROGRAMMANAGER_TEMP_MODE_NUM                                        /* Number of water temp modes */
} ProgramManager_TempModeType;

/* Program drain mode type */
typedef enum _ProgramManager_DrainModeType
{
  PROGRAMMANAGER_DRAIN_MODE_DEFAULT,                                  /* Drain mode is default type */
  PROGRAMMANAGER_DRAIN_MODE_CUSTOM,                                   /* Drain mode is custom type */
  PROGRAMMANAGER_DRAIN_MODE_NUM                                       /* Number of drain modes */
} ProgramManager_DrainModeType;

/* Washing machine type */
typedef enum _ProgramManager_WashingMachineType
{
  PROGRAMMANAGER_WASHING_MACHINE_MOTOR,                               /* Motor type */
  PROGRAMMANAGER_WASHING_MACHINE_INVERTER,                            /* Inverter type */
  PROGRAMMANAGER_WASHING_MACHINE_NUM                                  /* Number of washing machine type */
} ProgramManager_WashingMachineType;








/* Program machine function setup structure */
typedef struct _ProgramManager_MachineFuncSetupStruct
{
  bool                                            manOperateWhenAuto;                     /* Allow manually change the level, temperature and action when program in AUTO mode (1) */
  ProgramManager_TempUnitType                     tempUnit;                               /* Temperature Unit */
  ProgramManager_RelayEnableStatusType            drainValveStatus;                       /* Drain valve relay enable status while draining */
  ProgramManager_WashingMachineType               washMachine;                            /* Washing machine type */
} ProgramManager_MachineFuncSetupStruct;

/* Program sensor input status setup structure */
typedef struct _ProgramManager_InputStatusSetupStruct
{
  ProgramManager_InputActiveStatusType            doorClosed;                             /* Door closed input signal status */
  ProgramManager_InputActiveStatusType            extractShock;                           /* Extract shock input signal status */
  ProgramManager_InputActiveStatusType            emergencyStop;                          /* Emergency stop input signal status */
  ProgramManager_InputActiveStatusType            inverterError;                          /* Inverter error input signal status */
} ProgramManager_InputStatusSetupStruct;

/* Program manager fill and level setup structure */
typedef struct _ProgramManager_FillLevelSetupStruct
{
  uint16_t                                        zeroLevel;                              /* Set the relative zero level */
  uint16_t                                        lowLevel;                               /* Set the low level for water level LOW mode */
  uint16_t                                        midLevel;                               /* Set the middle level for water level MID mode */
  uint16_t                                        highLevel;                              /* Set the high level for water level HIGH mode */
  uint16_t                                        levelDiffRefill;                        /* Automatically refill water when difference between current and set is larger than this value */
  uint16_t                                        soapStartLevel;                         /* Reached water level to pour soap */
} ProgramManager_FillLevelSetupStruct;

#define PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MIN                 (0U)
#define PROGRAMMANAGER_FILLLEVELSETUP_WATER_LEVEL_MAX                 (91U)

#define PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_MIN             (0U)
#define PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_MAX             (91U)

/* Program manager heat and temp setup structure */
typedef struct _ProgramManager_HeatTempSetupStruct
{
  uint8_t                                         tempThreshold;                          /* The water temperature threshold */
  uint8_t                                         tempDiffReheat;                         /* Auto re-heat when difference between current and set is larger than this value */
  uint16_t                                        maxTimeHeat;                            /* If temperature is lower than set value after this amount of time, controller will alarm (minutes) */
} ProgramManager_HeatTempSetupStruct;

#define PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MIN                 (0U)
#define PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MAX                 (100U)

#define PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MIN                 (32U)
#define PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MAX                 (210U)

#define PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_MIN                  (0U)
#define PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_MAX                  (256U)

/* Program manager wash setup structure */
typedef struct _ProgramManager_WashSetupStruct
{
  uint16_t                                        stdWashRunTime;                         /* Standard wash run time (seconds) */
  uint16_t                                        stdWashStopTime;                        /* Standard wash stop time (seconds) */
  ProgramManager_MotorSpeedType                   stdWashSpeed;                           /* Default speed of standard wash */
} ProgramManager_WashSetupStruct;

#define PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MIN                      (0U)
#define PROGRAMMANAGER_WASHSETUP_COMMON_TIME_MAX                      (256U)

/* Program manager drain setup structure */
typedef struct _ProgramManager_DrainSetupStruct
{
  uint16_t                                        drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_NUM_MAX];  /* Drain step configuration */
  uint16_t                                        drainOffTime;                           /* Drain off time */
  uint16_t                                        maxDrainExtrTime;                       /* Max time of extract drain */
  uint8_t                                         reDrainExtrTime;                        /* Re-extract times when shock. If the times is larger than the set value, controller will alarm */
  ProgramManager_MotorSpeedType                   maxDrainExtrSpeed;                      /* Max speed of extract drain */
} ProgramManager_DrainSetupStruct;

#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_FORWARD_DRAIN_MIN         (3U)
#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_FORWARD_DRAIN_MAX         (60U)

#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_BALANCE_DRAIN_MIN         (3U)
#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_BALANCE_DRAIN_MAX         (60U)

#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL1_DRAIN_MIN       (10U)
#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL1_DRAIN_MAX       (600U)

#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL2_DRAIN_MIN       (0U)
#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL2_DRAIN_MAX       (600U)

#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL3_DRAIN_MIN       (0U)
#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL3_DRAIN_MAX       (600U)

#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL4_DRAIN_MIN       (0U)
#define PROGRAMMANAGER_DRAINSETUP_DRAINTIME_EXTR_LVL4_DRAIN_MAX       (600U)

#define PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_MIN                    (10U)
#define PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_MAX                    (200U)

#define PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_MIN                 (0U)
#define PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_MAX                 (9U)



/* Program manager parameter configuration setup structure */
typedef struct _ProgramManager_ParamConfigSetupStruct
{
  ProgramManager_MachineFuncSetupStruct           machineFuncCfg;                         /* Machine function setup configuration */
  ProgramManager_InputStatusSetupStruct           inputStatusCfg;                         /* Input status setup configuration */
  ProgramManager_FillLevelSetupStruct             fillLevelCfg;                           /* Fill level setup configuration */
  ProgramManager_HeatTempSetupStruct              heatTempCfg;                            /* Heat temperature setup configuration */
  ProgramManager_WashSetupStruct                  washCfg;                                /* Wash setup configuration */
  ProgramManager_DrainSetupStruct                 drainCfg;                               /* Drain setup configuration */
} ProgramManager_ParamConfigSetupStruct;








/* Program manager normal step structure */
typedef struct _ProgramManager_NormStepConfigStruct
{
  bool                                            isActive;                               /* This step is active during AUTO mode */
  uint8_t                                         waterMode;                              /* Water mode: bit 0 - uses cold water, bit 1 - uses hot water */
  uint8_t                                         soapMode;                               /* Soap mode: bit 0 - uses soap 1, bit 1 - uses soap 2, bit 2 - uses soap 3 */
  ProgramManager_WashModeType                     washMode;                               /* Wash time mode */
  ProgramManager_DrainModeType                    drainMode;                              /* Drain time mode */
  ProgramManager_TempModeType                     tempMode;                               /* Water temp mode */
  ProgramManager_LevelModeType                    levelMode;                              /* Water level mode */
  uint16_t                                        washTime;                               /* Wash time */
  uint16_t                                        washRunTime;                            /* Wash run time */
  uint16_t                                        washStopTime;                           /* Wash stop time */
  ProgramManager_MotorSpeedType                   washSpeed;                              /* Wash speed */
  uint8_t                                         tempThreshold;                          /* Water temperature threshold */
  uint16_t                                        levelThreshold;                         /* Water level threshold */
  uint16_t                                        drainTime[PROGRAMMANAGER_STEP_DRAINSTEP_NUM_MAX]; /* Drain step configuration */
  uint16_t                                        drainOffTime;                           /* Drain off time */
} ProgramManager_NormStepConfigStruct;

#define PROGRAMMANAGER_NORMSTEPCONFIG_WASHTIME_MIN                    (0U)
#define PROGRAMMANAGER_NORMSTEPCONFIG_WASHTIME_MAX                    (9999U)

/* Program manager AUTO sequence structure */
typedef struct _ProgramManager_AutoSeqConfigStruct
{
  uint8_t                                         sequenceIndex;
  uint8_t                                         currentStep;
  ProgramManager_NormStepConfigStruct             normStep[PROGRAMMANAGER_STEP_NUM_MAX];
} ProgramManager_AutoSeqConfigStruct;

/* Program manager MANUAL sequence structure */
typedef struct _ProgramManager_ManualSeqConfigStruct
{
  ProgramManager_NormStepConfigStruct             normStep;
} ProgramManager_ManualSeqConfigStruct;



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_TYPES_H */
