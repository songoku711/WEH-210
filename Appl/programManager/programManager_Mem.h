/* 
 * File:   ProgramManager_Mem.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef PROGRAMMANAGER_MEM_H
#define PROGRAMMANAGER_MEM_H

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

/* Program configuration initialized magic number base address */
#define PROGRAMMANAGER_MAGIC_NUMBER_BASE_ADDR                         (uint16_t)0x08

/* Program configuration initialized magic number */
#define PROGRAMMANAGER_CONF_INIT_MAGIC_NUMBER                         (uint32_t)0x00910913

/* Base address of current sequence index */
#define PROGRAMMANAGER_SEQUENCE_CURRENTINDEX_BASE_ADDR                (uint16_t)0x10





/* Base address of program parameter configuration */
#define PROGRAMMANAGER_PARAMCFG_BASE_ADDR                             (uint16_t)0x20



/* Base address of fill level setup configuration */
#define PROGRAMMANAGER_FILLLEVELSETUP_BASE_ADDR                       (uint16_t)0x20

/* Base address of fill level setup elements */
#define PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_BASE_ADDR             (uint16_t)0x20
#define PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_BASE_ADDR              (uint16_t)0x22
#define PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_BASE_ADDR              (uint16_t)0x24
#define PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_BASE_ADDR             (uint16_t)0x26
#define PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_BASE_ADDR       (uint16_t)0x28

/* Offset address from base of fill level setup configuration */
#define PROGRAMMANAGER_FILLLEVELSETUP_OFFSET                          (uint16_t)0x0

/* Offset address from base of fill level setup elements */
#define PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET                (uint16_t)0x0
#define PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET                 (uint16_t)0x2
#define PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET                 (uint16_t)0x4
#define PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET                (uint16_t)0x6
#define PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET          (uint16_t)0x8



/* Base address of heat setup configuration */
#define PROGRAMMANAGER_HEATTEMPSETUP_BASE_ADDR                        (uint16_t)0x30

/* Base address of heat setup elements */
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR          (uint16_t)0x30
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR         (uint16_t)0x32
#define PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_BASE_ADDR            (uint16_t)0x34

/* Offset address from base of heat setup configuration */
#define PROGRAMMANAGER_HEATTEMPSETUP_OFFSET                           (uint16_t)0x10

/* Offset address from base of heat setup elements */
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET             (uint16_t)0x0
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET            (uint16_t)0x2
#define PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_OFFSET               (uint16_t)0x4



/* Base address of door lock setup configuration */
#define PROGRAMMANAGER_DOORLOCKSETUP_BASE_ADDR                        (uint16_t)0x40

/* Base address of door lock setup elements */
#define PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_BASE_ADDR      (uint16_t)0x40
#define PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_BASE_ADDR    (uint16_t)0x41
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_BASE_ADDR         (uint16_t)0x42
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_BASE_ADDR        (uint16_t)0x44

/* Offset address from base of door lock setup configuration */
#define PROGRAMMANAGER_DOORLOCKSETUP_OFFSET                           (uint16_t)0x20

/* Offset address from base of door lock setup elements */
#define PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_OFFSET         (uint16_t)0x0
#define PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_OFFSET       (uint16_t)0x1
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET            (uint16_t)0x2
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET           (uint16_t)0x4



/* Base address of wash setup configuration */
#define PROGRAMMANAGER_WASHSETUP_BASE_ADDR                            (uint16_t)0x50

/* Base address of wash setup elements */
#define PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_BASE_ADDR             (uint16_t)0x50
#define PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_BASE_ADDR            (uint16_t)0x52
#define PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_BASE_ADDR             (uint16_t)0x54
#define PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_BASE_ADDR            (uint16_t)0x56
#define PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_BASE_ADDR             (uint16_t)0x58
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_BASE_ADDR            (uint16_t)0x5A
#define PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_BASE_ADDR               (uint16_t)0x5C
#define PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_BASE_ADDR               (uint16_t)0x5D
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_BASE_ADDR               (uint16_t)0x5E

/* Offset address from base of wash setup configuration */
#define PROGRAMMANAGER_WASHSETUP_OFFSET                               (uint16_t)0x30

/* Offset address from base of wash setup elements */
#define PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_OFFSET                (uint16_t)0x0
#define PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_OFFSET               (uint16_t)0x2
#define PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_OFFSET                (uint16_t)0x4
#define PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_OFFSET               (uint16_t)0x6
#define PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_OFFSET                (uint16_t)0x8
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_OFFSET               (uint16_t)0xA
#define PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_OFFSET                  (uint16_t)0xC
#define PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_OFFSET                  (uint16_t)0xD
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_OFFSET                  (uint16_t)0xE



/* Base address of drain setup configuration */
#define PROGRAMMANAGER_DRAINSETUP_BASE_ADDR                           (uint16_t)0x60

/* Base address of drain setup elements */
#define PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_BASE_ADDR            (uint16_t)0x60
#define PROGRAMMANAGER_DRAINSETUP_FORWARDDRAINTIME_BASE_ADDR          (uint16_t)0x62
#define PROGRAMMANAGER_DRAINSETUP_BALANCEDRAINTIME_BASE_ADDR          (uint16_t)0x64
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL1DRAINTIME_BASE_ADDR         (uint16_t)0x66
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL2DRAINTIME_BASE_ADDR         (uint16_t)0x68
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL3DRAINTIME_BASE_ADDR         (uint16_t)0x6A
#define PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_BASE_ADDR           (uint16_t)0x6C
#define PROGRAMMANAGER_DRAINSETUP_FORWARDDRAINSPEED_BASE_ADDR         (uint16_t)0x6D
#define PROGRAMMANAGER_DRAINSETUP_BALANCEDRAINSPEED_BASE_ADDR         (uint16_t)0x6E
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL1DRAINSPEED_BASE_ADDR        (uint16_t)0x6F
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL2DRAINSPEED_BASE_ADDR        (uint16_t)0x70
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL3DRAINSPEED_BASE_ADDR        (uint16_t)0x71
#define PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_BASE_ADDR              (uint16_t)0x72
#define PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_BASE_ADDR          (uint16_t)0x74
#define PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_BASE_ADDR           (uint16_t)0x76
#define PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRSPEED_BASE_ADDR         (uint16_t)0x77

/* Offset address from base of extract setup configuration */
#define PROGRAMMANAGER_DRAINSETUP_OFFSET                              (uint16_t)0x40

/* Offset address from base of extract setup elements */
#define PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINTIME_OFFSET               (uint16_t)0x0
#define PROGRAMMANAGER_DRAINSETUP_FORWARDDRAINTIME_OFFSET             (uint16_t)0x2
#define PROGRAMMANAGER_DRAINSETUP_BALANCEDRAINTIME_OFFSET             (uint16_t)0x4
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL1DRAINTIME_OFFSET            (uint16_t)0x6
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL2DRAINTIME_OFFSET            (uint16_t)0x8
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL3DRAINTIME_OFFSET            (uint16_t)0xA
#define PROGRAMMANAGER_DRAINSETUP_FIRSTDRAINSPEED_OFFSET              (uint16_t)0xC
#define PROGRAMMANAGER_DRAINSETUP_FORWARDDRAINSPEED_OFFSET            (uint16_t)0xD
#define PROGRAMMANAGER_DRAINSETUP_BALANCEDRAINSPEED_OFFSET            (uint16_t)0xE
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL1DRAINSPEED_OFFSET           (uint16_t)0xF
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL2DRAINSPEED_OFFSET           (uint16_t)0x10
#define PROGRAMMANAGER_DRAINSETUP_EXTRLVL3DRAINSPEED_OFFSET           (uint16_t)0x11
#define PROGRAMMANAGER_DRAINSETUP_DRAINOFFTIME_OFFSET                 (uint16_t)0x12
#define PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRTIME_OFFSET             (uint16_t)0x14
#define PROGRAMMANAGER_DRAINSETUP_REDRAINEXTRTIME_OFFSET              (uint16_t)0x16
#define PROGRAMMANAGER_DRAINSETUP_MAXDRAINEXTRSPEED_OFFSET            (uint16_t)0x17



/* Base address of machine function setup configuration */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_BASE_ADDR                     (uint16_t)0x70

/* Base address of machine function setup elements */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_BASE_ADDR  (uint16_t)0x70
#define PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_BASE_ADDR            (uint16_t)0x71
#define PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_BASE_ADDR    (uint16_t)0x72
#define PROGRAMMANAGER_MACHINEFUNCSETUP_WASHMACHINE_BASE_ADDR         (uint16_t)0x73

/* Offset address from base of machine function setup configuration */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_OFFSET                        (uint16_t)0x50

/* Offset address from base of machine function setup elements */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_OFFSET     (uint16_t)0x0
#define PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_OFFSET               (uint16_t)0x1
#define PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_OFFSET       (uint16_t)0x2
#define PROGRAMMANAGER_MACHINEFUNCSETUP_WASHMACHINE_OFFSET            (uint16_t)0x3



/* Base address of input status setup configuration */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_BASE_ADDR                     (uint16_t)0x80

/* Base address of input status setup elements */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_BASE_ADDR          (uint16_t)0x80
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_BASE_ADDR        (uint16_t)0x81
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_BASE_ADDR       (uint16_t)0x82
#define PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_BASE_ADDR       (uint16_t)0x83

/* Offset address from base of input status setup configuration */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_OFFSET                        (uint16_t)0x60

/* Offset address from base of input status setup elements */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_OFFSET             (uint16_t)0x0
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_OFFSET           (uint16_t)0x1
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_OFFSET          (uint16_t)0x2
#define PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_OFFSET          (uint16_t)0x3





/* Base address of program AUTO sequence configuration */
#define PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR                         (uint16_t)0x100
#define PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE                        (uint16_t)0x280

/* Base address of program MANUAL sequence configuration */
#define PROGRAMMANAGER_MANUALSEQUENCE_BASE_ADDR                       (uint16_t)0xD80

/* Offset address of normal step configuration in program sequence configuration */
#define PROGRAMMANAGER_NORMSTEP_OFFSET                                (uint16_t)0x0
#define PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE                            (uint16_t)0x40

/* Offset address of normal step elements in program sequence configuration */
#define PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET                       (uint16_t)0x00
#define PROGRAMMANAGER_NORMSTEP_WATERMODE_OFFSET                      (uint16_t)0x01
#define PROGRAMMANAGER_NORMSTEP_SOAPMODE_OFFSET                       (uint16_t)0x02
#define PROGRAMMANAGER_NORMSTEP_WASHMODE_OFFSET                       (uint16_t)0x03
#define PROGRAMMANAGER_NORMSTEP_DRAINMODE_OFFSET                      (uint16_t)0x04
#define PROGRAMMANAGER_NORMSTEP_TEMPMODE_OFFSET                       (uint16_t)0x05
#define PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET                      (uint16_t)0x06
#define PROGRAMMANAGER_NORMSTEP_WASHTIME_OFFSET                       (uint16_t)0x07
#define PROGRAMMANAGER_NORMSTEP_WASHRUNTIME_OFFSET                    (uint16_t)0x09
#define PROGRAMMANAGER_NORMSTEP_WASHSTOPTIME_OFFSET                   (uint16_t)0x0B
#define PROGRAMMANAGER_NORMSTEP_WASHSPEED_OFFSET                      (uint16_t)0x0E
#define PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET                  (uint16_t)0x0F
#define PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET                 (uint16_t)0x10

#define PROGRAMMANAGER_NORMSTEP_DRAINSTEP_OFFSET                      (uint16_t)0x20
#define PROGRAMMANAGER_NORMSTEP_DRAINSTEP_BLOCK_SIZE                  (uint16_t)0x03

#define PROGRAMMANAGER_NORMSTEP_DRAINSTEP_DRAINTIME_OFFSET            (uint16_t)0x00
#define PROGRAMMANAGER_NORMSTEP_DRAINSTEP_DRAINSPEED_OFFSET           (uint16_t)0x02



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

HAL_StatusTypeDef ProgramManager_ParamConfigSetup_GetData(ProgramManager_ParamConfigSetupStruct *data);
HAL_StatusTypeDef ProgramManager_ParamConfigSetup_SetData(ProgramManager_ParamConfigSetupStruct *data);

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_GetData(ProgramManager_MachineFuncSetupStruct *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_SetData(ProgramManager_MachineFuncSetupStruct *data);

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_ManOperateWhenAuto_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_ManOperateWhenAuto_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_TempUnit_GetData(ProgramManager_TempUnitType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_TempUnit_SetData(ProgramManager_TempUnitType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainValveStatus_GetData(ProgramManager_RelayEnableStatusType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainValveStatus_SetData(ProgramManager_RelayEnableStatusType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_WashMachine_GetData(ProgramManager_WashingMachineType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_WashMachine_SetData(ProgramManager_WashingMachineType *data);



HAL_StatusTypeDef ProgramManager_InputStatusSetup_GetData(ProgramManager_InputStatusSetupStruct *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_SetData(ProgramManager_InputStatusSetupStruct *data);

HAL_StatusTypeDef ProgramManager_InputStatusSetup_DoorClosed_GetData(ProgramManager_InputActiveStatusType *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_DoorClosed_SetData(ProgramManager_InputActiveStatusType *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_ExtractShock_GetData(ProgramManager_InputActiveStatusType *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_ExtractShock_SetData(ProgramManager_InputActiveStatusType *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_EmergencyStop_GetData(ProgramManager_InputActiveStatusType *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_EmergencyStop_SetData(ProgramManager_InputActiveStatusType *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_InverterError_GetData(ProgramManager_InputActiveStatusType *data);
HAL_StatusTypeDef ProgramManager_InputStatusSetup_InverterError_SetData(ProgramManager_InputActiveStatusType *data);



HAL_StatusTypeDef ProgramManager_FillLevelSetup_GetData(ProgramManager_FillLevelSetupStruct *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_SetData(ProgramManager_FillLevelSetupStruct *data);

HAL_StatusTypeDef ProgramManager_FillLevelSetup_ZeroLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_ZeroLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LowLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LowLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_MidLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_MidLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_HighLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_HighLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LevelDiffRefill_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LevelDiffRefill_SetData(uint16_t *data);



HAL_StatusTypeDef ProgramManager_HeatTempSetup_GetData(ProgramManager_HeatTempSetupStruct *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_SetData(ProgramManager_HeatTempSetupStruct *data, ProgramManager_TempUnitType tempUnit);

HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempThreshold_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempThreshold_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempDiffReheat_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempDiffReheat_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxTimeHeat_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxTimeHeat_SetData(uint16_t *data);



HAL_StatusTypeDef ProgramManager_WashSetup_GetData(ProgramManager_WashSetupStruct *data);
HAL_StatusTypeDef ProgramManager_WashSetup_SetData(ProgramManager_WashSetupStruct *data);

HAL_StatusTypeDef ProgramManager_WashSetup_StdWashRunTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_StdWashRunTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_StdWashStopTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_StdWashStopTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_DelWashRunTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_DelWashRunTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_DelWashStopTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_DelWashStopTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashRunTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashRunTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashStopTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashStopTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_StdWashSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_WashSetup_StdWashSpeed_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_WashSetup_DelWashSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_WashSetup_DelWashSpeed_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_WashSetup_HvyWashSpeed_SetData(ProgramManager_MotorSpeedType *data);



HAL_StatusTypeDef ProgramManager_DrainSetup_GetData(ProgramManager_DrainSetupStruct *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_SetData(ProgramManager_DrainSetupStruct *data);

HAL_StatusTypeDef ProgramManager_DrainSetup_DrainTime_GetData(uint8_t drainStep, uint16_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_DrainTime_SetData(uint8_t drainStep, uint16_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_DrainSpeed_GetData(uint8_t drainStep, ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_DrainSpeed_SetData(uint8_t drainStep, ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_DrainOffTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_DrainOffTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_ReDrainExtrTime_GetData(uint8_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_ReDrainExtrTime_SetData(uint8_t *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_DrainSetup_MaxDrainExtrSpeed_SetData(ProgramManager_MotorSpeedType *data);



HAL_StatusTypeDef ProgramManager_DoorLockSetup_GetData(ProgramManager_DoorLockSetupStruct *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_SetData(ProgramManager_DoorLockSetupStruct *data);

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UseLockStopButton_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UseLockStopButton_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_DoorLockValveStatus_GetData(ProgramManager_RelayEnableStatusType *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_DoorLockValveStatus_SetData(ProgramManager_RelayEnableStatusType *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorTemp_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorTemp_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorLevel_SetData(uint16_t *data);





HAL_StatusTypeDef ProgramManager_AutoSeqConfig_GetData(ProgramManager_AutoSeqConfigStruct *data);
HAL_StatusTypeDef ProgramManager_AutoSeqConfig_SetData(ProgramManager_AutoSeqConfigStruct *data);

HAL_StatusTypeDef ProgramManager_SequenceIndex_GetData(uint8_t *data);
HAL_StatusTypeDef ProgramManager_SequenceIndex_SetData(uint8_t data);

HAL_StatusTypeDef ProgramManager_NormStepConfig_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_NormStepConfig_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit);

HAL_StatusTypeDef ProgramManager_NormStepConfig_IsActive_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_IsActive_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WaterMode_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WaterMode_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_SoapMode_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_SoapMode_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_WashModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_WashModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_DrainModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_DrainModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_TempMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_TempModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_TempMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_TempModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_LevelModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_LevelModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashRunTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashRunTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashStopTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashStopTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashSpeed_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_WashSpeed_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_TempThreshold_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_NormStepConfig_TempThreshold_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelThreshold_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_LevelThreshold_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainSpeed_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_DrainStepConfig_DrainSpeed_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t drainStepIdx, ProgramManager_MotorSpeedType *data);



HAL_StatusTypeDef ProgramManager_ManualSeqConfig_GetData(ProgramManager_ManualSeqConfigStruct *data);
HAL_StatusTypeDef ProgramManager_ManualSeqConfig_SetData(ProgramManager_ManualSeqConfigStruct *data);



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_MEM_H */
