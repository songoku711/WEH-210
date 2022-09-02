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
#define PROGRAMMANAGER_FILLLEVELSETUP_AUTOREFILLWHENLOW_BASE_ADDR     (uint16_t)0x20
#define PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_BASE_ADDR             (uint16_t)0x21
#define PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_BASE_ADDR              (uint16_t)0x23
#define PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_BASE_ADDR              (uint16_t)0x25
#define PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_BASE_ADDR             (uint16_t)0x27
#define PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_BASE_ADDR         (uint16_t)0x29
#define PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_BASE_ADDR       (uint16_t)0x2B
#define PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_BASE_ADDR           (uint16_t)0x2D

/* Offset address from base of fill level setup configuration */
#define PROGRAMMANAGER_FILLLEVELSETUP_OFFSET                          (uint16_t)0x0

/* Offset address from base of fill level setup elements */
#define PROGRAMMANAGER_FILLLEVELSETUP_AUTOREFILLWHENLOW_OFFSET        (uint16_t)0x0
#define PROGRAMMANAGER_FILLLEVELSETUP_ZEROLEVEL_OFFSET                (uint16_t)0x1
#define PROGRAMMANAGER_FILLLEVELSETUP_LOWLEVEL_OFFSET                 (uint16_t)0x3
#define PROGRAMMANAGER_FILLLEVELSETUP_MIDLEVEL_OFFSET                 (uint16_t)0x5
#define PROGRAMMANAGER_FILLLEVELSETUP_HIGHLEVEL_OFFSET                (uint16_t)0x7
#define PROGRAMMANAGER_FILLLEVELSETUP_OVERBRIMLEVEL_OFFSET            (uint16_t)0x9
#define PROGRAMMANAGER_FILLLEVELSETUP_LEVELDIFFREFILL_OFFSET          (uint16_t)0xB
#define PROGRAMMANAGER_FILLLEVELSETUP_MAXTIMEFILL_OFFSET              (uint16_t)0xD



/* Base address of heat setup configuration */
#define PROGRAMMANAGER_HEATTEMPSETUP_BASE_ADDR                        (uint16_t)0x30

/* Base address of heat setup elements */
#define PROGRAMMANAGER_HEATTEMPSETUP_AUTOREHEATWHENLOW_BASE_ADDR      (uint16_t)0x30
#define PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_BASE_ADDR           (uint16_t)0x31
#define PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_BASE_ADDR           (uint16_t)0x33
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_BASE_ADDR          (uint16_t)0x35
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_BASE_ADDR         (uint16_t)0x37
#define PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_BASE_ADDR            (uint16_t)0x39

/* Offset address from base of heat setup configuration */
#define PROGRAMMANAGER_HEATTEMPSETUP_OFFSET                           (uint16_t)0x10

/* Offset address from base of heat setup elements */
#define PROGRAMMANAGER_HEATTEMPSETUP_AUTOREHEATWHENLOW_OFFSET         (uint16_t)0x0
#define PROGRAMMANAGER_HEATTEMPSETUP_MINWATERTEMP_OFFSET              (uint16_t)0x1
#define PROGRAMMANAGER_HEATTEMPSETUP_MAXWATERTEMP_OFFSET              (uint16_t)0x3
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPTHRESHOLD_OFFSET             (uint16_t)0x5
#define PROGRAMMANAGER_HEATTEMPSETUP_TEMPDIFFREHEAT_OFFSET            (uint16_t)0x7
#define PROGRAMMANAGER_HEATTEMPSETUP_MAXTIMEHEAT_OFFSET               (uint16_t)0x9



/* Base address of soap setup configuration */
#define PROGRAMMANAGER_SOAPSETUP_BASE_ADDR                            (uint16_t)0x40

/* Base address of soap setup elements */
#define PROGRAMMANAGER_SOAPSETUP_STOPFILLWHENSOAP_BASE_ADDR           (uint16_t)0x40
#define PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_BASE_ADDR                  (uint16_t)0x41
#define PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_BASE_ADDR                  (uint16_t)0x43
#define PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_BASE_ADDR                  (uint16_t)0x45

/* Offset address from base of soap setup configuration */
#define PROGRAMMANAGER_SOAPSETUP_OFFSET                               (uint16_t)0x20

/* Offset address from base of soap setup elements */
#define PROGRAMMANAGER_SOAPSETUP_STOPFILLWHENSOAP_OFFSET              (uint16_t)0x0
#define PROGRAMMANAGER_SOAPSETUP_TIMESOAP1_OFFSET                     (uint16_t)0x1
#define PROGRAMMANAGER_SOAPSETUP_TIMESOAP2_OFFSET                     (uint16_t)0x3
#define PROGRAMMANAGER_SOAPSETUP_TIMESOAP3_OFFSET                     (uint16_t)0x5



/* Base address of door lock setup configuration */
#define PROGRAMMANAGER_DOORLOCKSETUP_BASE_ADDR                        (uint16_t)0x50

/* Base address of door lock setup elements */
#define PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_BASE_ADDR      (uint16_t)0x50
#define PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_BASE_ADDR    (uint16_t)0x51
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_BASE_ADDR         (uint16_t)0x52
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_BASE_ADDR        (uint16_t)0x54

/* Offset address from base of door lock setup configuration */
#define PROGRAMMANAGER_DOORLOCKSETUP_OFFSET                           (uint16_t)0x30

/* Offset address from base of door lock setup elements */
#define PROGRAMMANAGER_DOORLOCKSETUP_USELOCKSTOPBUTTON_OFFSET         (uint16_t)0x0
#define PROGRAMMANAGER_DOORLOCKSETUP_DOORLOCKVALVESTATUS_OFFSET       (uint16_t)0x1
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORTEMP_OFFSET            (uint16_t)0x2
#define PROGRAMMANAGER_DOORLOCKSETUP_UNLOCKDOORLEVEL_OFFSET           (uint16_t)0x4



/* Base address of wash setup configuration */
#define PROGRAMMANAGER_WASHSETUP_BASE_ADDR                            (uint16_t)0x60

/* Base address of wash setup elements */
#define PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_BASE_ADDR             (uint16_t)0x60
#define PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_BASE_ADDR             (uint16_t)0x62
#define PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_BASE_ADDR            (uint16_t)0x64
#define PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_BASE_ADDR             (uint16_t)0x66
#define PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_BASE_ADDR            (uint16_t)0x68
#define PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_BASE_ADDR             (uint16_t)0x6A
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_BASE_ADDR            (uint16_t)0x6C
#define PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_BASE_ADDR               (uint16_t)0x6E
#define PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_BASE_ADDR               (uint16_t)0x6F
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_BASE_ADDR               (uint16_t)0x70
#define PROGRAMMANAGER_WASHSETUP_MAXWASHSPEED_BASE_ADDR               (uint16_t)0x71

/* Offset address from base of wash setup configuration */
#define PROGRAMMANAGER_WASHSETUP_OFFSET                               (uint16_t)0x40

/* Offset address from base of wash setup elements */
#define PROGRAMMANAGER_WASHSETUP_MINPAUSEFWDREV_OFFSET                (uint16_t)0x0
#define PROGRAMMANAGER_WASHSETUP_STDWASHRUNTIME_OFFSET                (uint16_t)0x2
#define PROGRAMMANAGER_WASHSETUP_STDWASHSTOPTIME_OFFSET               (uint16_t)0x4
#define PROGRAMMANAGER_WASHSETUP_DELWASHRUNTIME_OFFSET                (uint16_t)0x6
#define PROGRAMMANAGER_WASHSETUP_DELWASHSTOPTIME_OFFSET               (uint16_t)0x8
#define PROGRAMMANAGER_WASHSETUP_HVYWASHRUNTIME_OFFSET                (uint16_t)0xA
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSTOPTIME_OFFSET               (uint16_t)0xC
#define PROGRAMMANAGER_WASHSETUP_STDWASHSPEED_OFFSET                  (uint16_t)0xE
#define PROGRAMMANAGER_WASHSETUP_DELWASHSPEED_OFFSET                  (uint16_t)0xF
#define PROGRAMMANAGER_WASHSETUP_HVYWASHSPEED_OFFSET                  (uint16_t)0x10
#define PROGRAMMANAGER_WASHSETUP_MAXWASHSPEED_OFFSET                  (uint16_t)0x11



/* Base address of extract setup part 1 configuration */
#define PROGRAMMANAGER_EXTRACTSETUP_PART1_BASE_ADDR                   (uint16_t)0x80

/* Base address of extract setup part 1 elements */
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_BASE_ADDR            (uint16_t)0x80
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_BASE_ADDR       (uint16_t)0x82
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_BASE_ADDR    (uint16_t)0x84
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_BASE_ADDR   (uint16_t)0x86
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_BASE_ADDR        (uint16_t)0x88
#define PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_BASE_ADDR              (uint16_t)0x8A
#define PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_BASE_ADDR          (uint16_t)0x8C
#define PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_BASE_ADDR     (uint16_t)0x8E
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_BASE_ADDR        (uint16_t)0x90
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_BASE_ADDR        (uint16_t)0x92
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_BASE_ADDR        (uint16_t)0x94
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_BASE_ADDR    (uint16_t)0x96
#define PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_BASE_ADDR          (uint16_t)0x98
#define PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_BASE_ADDR           (uint16_t)0x9A
#define PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_BASE_ADDR       (uint16_t)0x9C

/* Offset address from base of extract setup part 1 configuration */
#define PROGRAMMANAGER_EXTRACTSETUP_PART1_OFFSET                      (uint16_t)0x60

/* Offset address from base of extract setup part 1 elements */
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCELEVEL_OFFSET               (uint16_t)0x0
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINLEVEL_OFFSET          (uint16_t)0x2
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEWITHWATERTIME_OFFSET       (uint16_t)0x4
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEDRAINWATERTIME_OFFSET      (uint16_t)0x6
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCEDELAYTIME_OFFSET           (uint16_t)0x8
#define PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_OFFSET                 (uint16_t)0xA
#define PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTTIME_OFFSET             (uint16_t)0xC
#define PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_OFFSET        (uint16_t)0xE
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME1_OFFSET           (uint16_t)0x10
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME2_OFFSET           (uint16_t)0x12
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTTIME3_OFFSET           (uint16_t)0x14
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTDELAYTIME_OFFSET       (uint16_t)0x16
#define PROGRAMMANAGER_EXTRACTSETUP_MAXEXTRACTTIME_OFFSET             (uint16_t)0x18
#define PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTTIME_OFFSET              (uint16_t)0x1A
#define PROGRAMMANAGER_EXTRACTSETUP_REEXTRACTWASHTIME_OFFSET          (uint16_t)0x1C



/* Base address of extract setup part 2 configuration */
#define PROGRAMMANAGER_EXTRACTSETUP_PART2_BASE_ADDR                   (uint16_t)0xA0

/* Base address of extract setup part 2 elements */
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCESPEED_BASE_ADDR            (uint16_t)0xA0
#define PROGRAMMANAGER_EXTRACTSETUP_FWDRUNSPEED_BASE_ADDR             (uint16_t)0xA1
#define PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTSPEED_BASE_ADDR         (uint16_t)0xA2
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED1_BASE_ADDR       (uint16_t)0xA3
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED2_BASE_ADDR       (uint16_t)0xA4
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED3_BASE_ADDR       (uint16_t)0xA5
#define PROGRAMMANAGER_EXTRACTSETUP_MAXMIDEXTRACTSPEED_BASE_ADDR      (uint16_t)0xA6
#define PROGRAMMANAGER_EXTRACTSETUP_MAXHIGHEXTRACTSPEED_BASE_ADDR     (uint16_t)0xA7

/* Offset address from base of extract setup part 2 configuration */
#define PROGRAMMANAGER_EXTRACTSETUP_PART2_OFFSET                      (uint16_t)0x80

/* Offset address from base of extract setup part 2 elements */
#define PROGRAMMANAGER_EXTRACTSETUP_BALANCESPEED_OFFSET               (uint16_t)0x0
#define PROGRAMMANAGER_EXTRACTSETUP_FWDRUNSPEED_OFFSET                (uint16_t)0x1
#define PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTSPEED_OFFSET            (uint16_t)0x2
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED1_OFFSET          (uint16_t)0x3
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED2_OFFSET          (uint16_t)0x4
#define PROGRAMMANAGER_EXTRACTSETUP_HIGHEXTRACTSPEED3_OFFSET          (uint16_t)0x5
#define PROGRAMMANAGER_EXTRACTSETUP_MAXMIDEXTRACTSPEED_OFFSET         (uint16_t)0x6
#define PROGRAMMANAGER_EXTRACTSETUP_MAXHIGHEXTRACTSPEED_OFFSET        (uint16_t)0x7



/* Base address of machine function setup configuration */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_BASE_ADDR                     (uint16_t)0xC0

/* Base address of machine function setup elements */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINWHILEDOOROPEN_BASE_ADDR  (uint16_t)0xC0
#define PROGRAMMANAGER_MACHINEFUNCSETUP_HEATUSETIMEOUT_BASE_ADDR      (uint16_t)0xC1
#define PROGRAMMANAGER_MACHINEFUNCSETUP_FILLUSETIMEOUT_BASE_ADDR      (uint16_t)0xC2
#define PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_BASE_ADDR  (uint16_t)0xC3
#define PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_BASE_ADDR            (uint16_t)0xC4
#define PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_BASE_ADDR    (uint16_t)0xC5

/* Offset address from base of machine function setup configuration */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_OFFSET                        (uint16_t)0xA0

/* Offset address from base of machine function setup elements */
#define PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINWHILEDOOROPEN_OFFSET     (uint16_t)0x0
#define PROGRAMMANAGER_MACHINEFUNCSETUP_HEATUSETIMEOUT_OFFSET         (uint16_t)0x1
#define PROGRAMMANAGER_MACHINEFUNCSETUP_FILLUSETIMEOUT_OFFSET         (uint16_t)0x2
#define PROGRAMMANAGER_MACHINEFUNCSETUP_MANOPERATEWHENAUTO_OFFSET     (uint16_t)0x3
#define PROGRAMMANAGER_MACHINEFUNCSETUP_TEMPUNIT_OFFSET               (uint16_t)0x4
#define PROGRAMMANAGER_MACHINEFUNCSETUP_DRAINVALVESTATUS_OFFSET       (uint16_t)0x5



/* Base address of input status setup configuration */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_BASE_ADDR                     (uint16_t)0xE0

/* Base address of input status setup elements */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_BASE_ADDR          (uint16_t)0xE0
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_BASE_ADDR        (uint16_t)0xE1
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_BASE_ADDR       (uint16_t)0xE2
#define PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_BASE_ADDR       (uint16_t)0xE3

/* Offset address from base of input status setup configuration */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_OFFSET                        (uint16_t)0xC0

/* Offset address from base of input status setup elements */
#define PROGRAMMANAGER_INPUTSTATUSSETUP_DOORCLOSED_OFFSET             (uint16_t)0x0
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EXTRACTSHOCK_OFFSET           (uint16_t)0x1
#define PROGRAMMANAGER_INPUTSTATUSSETUP_EMERGENCYSTOP_OFFSET          (uint16_t)0x2
#define PROGRAMMANAGER_INPUTSTATUSSETUP_INVERTERERROR_OFFSET          (uint16_t)0x3





/* Base address of program AUTO sequence configuration */
#define PROGRAMMANAGER_AUTOSEQUENCE_BASE_ADDR                         (uint16_t)0x100
#define PROGRAMMANAGER_AUTOSEQUENCE_BLOCK_SIZE                        (uint16_t)0x140

/* Base address of program MANUAL sequence configuration */
#define PROGRAMMANAGER_MANUALSEQUENCE_BASE_ADDR                       (uint16_t)0xD80

/* Offset address of normal step configuration in program sequence configuration */
#define PROGRAMMANAGER_NORMSTEP_OFFSET                                (uint16_t)0x0
#define PROGRAMMANAGER_NORMSTEP_BLOCK_SIZE                            (uint16_t)0x20

/* Offset address of normal step elements in program sequence configuration */
#define PROGRAMMANAGER_NORMSTEP_ISACTIVE_OFFSET                       (uint16_t)0x00
#define PROGRAMMANAGER_NORMSTEP_USECOLDWATER_OFFSET                   (uint16_t)0x01
#define PROGRAMMANAGER_NORMSTEP_USEHOTWATER_OFFSET                    (uint16_t)0x02
#define PROGRAMMANAGER_NORMSTEP_USESOAP1_OFFSET                       (uint16_t)0x03
#define PROGRAMMANAGER_NORMSTEP_USESOAP2_OFFSET                       (uint16_t)0x04
#define PROGRAMMANAGER_NORMSTEP_USESOAP3_OFFSET                       (uint16_t)0x05
#define PROGRAMMANAGER_NORMSTEP_WASHTIMEMODE_OFFSET                   (uint16_t)0x06
#define PROGRAMMANAGER_NORMSTEP_LEVELMODE_OFFSET                      (uint16_t)0x07
#define PROGRAMMANAGER_NORMSTEP_WASHNUM_OFFSET                        (uint16_t)0x08
#define PROGRAMMANAGER_NORMSTEP_TEMPTHRESHOLD_OFFSET                  (uint16_t)0x0A
#define PROGRAMMANAGER_NORMSTEP_LEVELTHRESHOLD_OFFSET                 (uint16_t)0x0C
#define PROGRAMMANAGER_NORMSTEP_BALANCETIME_OFFSET                    (uint16_t)0x0E
#define PROGRAMMANAGER_NORMSTEP_MIDEXTRACTTIME_OFFSET                 (uint16_t)0x10
#define PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME1_OFFSET               (uint16_t)0x12
#define PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME2_OFFSET               (uint16_t)0x14
#define PROGRAMMANAGER_NORMSTEP_HIGHEXTRACTTIME3_OFFSET               (uint16_t)0x16

/* Offset address of unload step configuration in program sequence configuration */
#define PROGRAMMANAGER_UNLOADSTEP_OFFSET                              (uint16_t)0x13E
#define PROGRAMMANAGER_UNLOADSTEP_BLOCK_SIZE                          (uint16_t)0x2

/* Offset address of unload step elements in program sequence configuration */
#define PROGRAMMANAGER_UNLOADSTEP_REVERSETIME_OFFSET                  (uint16_t)0x0



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

HAL_StatusTypeDef ProgramManager_ParamConfigSetup_GetData(ProgramManager_ParamConfigSetupStruct *data);
HAL_StatusTypeDef ProgramManager_ParamConfigSetup_SetData(ProgramManager_ParamConfigSetupStruct *data);

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_GetData(ProgramManager_MachineFuncSetupStruct *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_SetData(ProgramManager_MachineFuncSetupStruct *data);

HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainWhileDoorOpen_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainWhileDoorOpen_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_HeatUseTimeout_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_HeatUseTimeout_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_FillUseTimeout_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_FillUseTimeout_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_ManOperateWhenAuto_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_ManOperateWhenAuto_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_TempUnit_GetData(ProgramManager_TempUnitType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_TempUnit_SetData(ProgramManager_TempUnitType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainValveStatus_GetData(ProgramManager_RelayEnableStatusType *data);
HAL_StatusTypeDef ProgramManager_MachineFuncSetup_DrainValveStatus_SetData(ProgramManager_RelayEnableStatusType *data);



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

HAL_StatusTypeDef ProgramManager_FillLevelSetup_AutoRefillWhenLow_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_AutoRefillWhenLow_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_ZeroLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_ZeroLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LowLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LowLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_MidLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_MidLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_HighLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_HighLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_OverbrimLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_OverbrimLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LevelDiffRefill_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_LevelDiffRefill_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_MaxTimeFill_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_FillLevelSetup_MaxTimeFill_SetData(uint16_t *data);



HAL_StatusTypeDef ProgramManager_HeatTempSetup_GetData(ProgramManager_HeatTempSetupStruct *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_SetData(ProgramManager_HeatTempSetupStruct *data, ProgramManager_TempUnitType tempUnit);

HAL_StatusTypeDef ProgramManager_HeatTempSetup_AutoReheatWhenLow_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_AutoReheatWhenLow_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MinWaterTemp_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MinWaterTemp_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxWaterTemp_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxWaterTemp_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempThreshold_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempThreshold_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempDiffReheat_GetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_TempDiffReheat_SetData(uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxTimeHeat_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_HeatTempSetup_MaxTimeHeat_SetData(uint16_t *data);



HAL_StatusTypeDef ProgramManager_SoapSetup_GetData(ProgramManager_SoapSetupStruct *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_SetData(ProgramManager_SoapSetupStruct *data);

HAL_StatusTypeDef ProgramManager_SoapSetup_StopFillWhenSoap_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_StopFillWhenSoap_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap1_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap1_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap2_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap2_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap3_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_SoapSetup_TimeSoap3_SetData(uint16_t *data);



HAL_StatusTypeDef ProgramManager_WashSetup_GetData(ProgramManager_WashSetupStruct *data);
HAL_StatusTypeDef ProgramManager_WashSetup_SetData(ProgramManager_WashSetupStruct *data);

HAL_StatusTypeDef ProgramManager_WashSetup_MinPauseFwdRev_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_WashSetup_MinPauseFwdRev_SetData(uint16_t *data);
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
HAL_StatusTypeDef ProgramManager_WashSetup_MaxWashSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_WashSetup_MaxWashSpeed_SetData(ProgramManager_MotorSpeedType *data);



HAL_StatusTypeDef ProgramManager_ExtractSetup_GetData(ProgramManager_ExtractSetupStruct *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_SetData(ProgramManager_ExtractSetupStruct *data);

HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainLevel_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceWithWaterTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceWithWaterTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainWaterTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDrainWaterTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDelayTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceDelayTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractDelayTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractDelayTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime1_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime1_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime2_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime2_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime3_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractTime3_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractDelayTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractDelayTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxExtractTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxExtractTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractWashTime_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_ReextractWashTime_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_BalanceSpeed_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_FwdRunSpeed_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MidExtractSpeed_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed1_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed1_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed2_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed2_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed3_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_HighExtractSpeed3_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxMidExtractSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxMidExtractSpeed_SetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxHighExtractSpeed_GetData(ProgramManager_MotorSpeedType *data);
HAL_StatusTypeDef ProgramManager_ExtractSetup_MaxHighExtractSpeed_SetData(ProgramManager_MotorSpeedType *data);



HAL_StatusTypeDef ProgramManager_DoorLockSetup_GetData(ProgramManager_DoorLockSetupStruct *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_SetData(ProgramManager_DoorLockSetupStruct *data);

HAL_StatusTypeDef ProgramManager_DoorLockSetup_UseLockStopButton_GetData(bool *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UseLockStopButton_SetData(bool *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_DoorLockValveStatus_GetData(ProgramManager_RelayEnableStatusType *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_DoorLockValveStatus_SetData(ProgramManager_RelayEnableStatusType *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorTemp_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorTemp_SetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorLevel_GetData(uint16_t *data);
HAL_StatusTypeDef ProgramManager_DoorLockSetup_UnlockDoorLevel_SetData(uint16_t *data);





HAL_StatusTypeDef ProgramManager_AutoSeqConfig_GetData(ProgramManager_AutoSeqConfigStruct *data);
HAL_StatusTypeDef ProgramManager_AutoSeqConfig_SetData(ProgramManager_AutoSeqConfigStruct *data);

HAL_StatusTypeDef ProgramManager_SequenceIndex_GetData(uint8_t *data);
HAL_StatusTypeDef ProgramManager_SequenceIndex_SetData(uint8_t *data);

HAL_StatusTypeDef ProgramManager_NormStepConfig_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_NormStepConfig_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_NormStepConfigStruct *data, ProgramManager_TempUnitType tempUnit);

HAL_StatusTypeDef ProgramManager_NormStepConfig_isActive_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_isActive_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useColdWater_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useColdWater_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useHotWater_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useHotWater_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useSoap1_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useSoap1_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useSoap2_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useSoap2_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useSoap3_GetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_useSoap3_SetData(uint8_t seqIdx, uint8_t stepIdx, bool *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_washTimeMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_WashModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_washTimeMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_WashModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_levelMode_GetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_LevelModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_levelMode_SetData(uint8_t seqIdx, uint8_t stepIdx, ProgramManager_LevelModeType *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_washNum_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_washNum_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_tempThreshold_GetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_NormStepConfig_tempThreshold_SetData(uint8_t seqIdx, uint8_t stepIdx, uint8_t *data, ProgramManager_TempUnitType tempUnit);
HAL_StatusTypeDef ProgramManager_NormStepConfig_levelThreshold_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_levelThreshold_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_balanceTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_balanceTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_midExtractTime_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_midExtractTime_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_highExtractTime1_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_highExtractTime1_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_highExtractTime2_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_highExtractTime2_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_highExtractTime3_GetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);
HAL_StatusTypeDef ProgramManager_NormStepConfig_highExtractTime3_SetData(uint8_t seqIdx, uint8_t stepIdx, uint16_t *data);



HAL_StatusTypeDef ProgramManager_UnloadStepConfig_GetData(uint8_t seqIdx, ProgramManager_UnloadStepConfigStruct *data);
HAL_StatusTypeDef ProgramManager_UnloadStepConfig_SetData(uint8_t seqIdx, ProgramManager_UnloadStepConfigStruct *data);



HAL_StatusTypeDef ProgramManager_ManualSeqConfig_GetData(ProgramManager_ManualSeqConfigStruct *data);
HAL_StatusTypeDef ProgramManager_ManualSeqConfig_SetData(ProgramManager_ManualSeqConfigStruct *data);



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_MEM_H */
