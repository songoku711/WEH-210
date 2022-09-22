/* 
 * File:   menuManager.c
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

#include "menuManager.h"



/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

/** Application state machine */
/** State powerOn (0) */
extern Fsm_EventEntryStruct MenuManager_PowerOn_StateMachine[2];
/** State init (1) */
extern Fsm_EventEntryStruct MenuManager_Init_StateMachine[3];
/** State home (2) */
extern Fsm_EventEntryStruct MenuManager_Home_StateMachine[10];
/** State mainSetting (3) */
extern Fsm_EventEntryStruct MenuManager_MainSetting_StateMachine[8];
/** State setToDefault (4) */
extern Fsm_EventEntryStruct MenuManager_SetToDefault_StateMachine[7];
/** State setDefaultParam (5) */
extern Fsm_EventEntryStruct MenuManager_SetDefaultParam_StateMachine[5];
/** State setDefaultProg (6) */
extern Fsm_EventEntryStruct MenuManager_SetDefaultProg_StateMachine[5];
/** State machineSetup (7) */
extern Fsm_EventEntryStruct MenuManager_MachineSetup_StateMachine[13];
/** State machineFuncSetup (8) */
extern Fsm_EventEntryStruct MenuManager_MachineFuncSetup_StateMachine[11];
/** State drainWhileDoorOpen (9) */
extern Fsm_EventEntryStruct MenuManager_DrainWhileDoorOpen_StateMachine[7];
/** State heatUseTimeout (10) */
extern Fsm_EventEntryStruct MenuManager_HeatUseTimeout_StateMachine[7];
/** State fillUseTimeout (11) */
extern Fsm_EventEntryStruct MenuManager_FillUseTimeout_StateMachine[7];
/** State manOperateWhenAuto (12) */
extern Fsm_EventEntryStruct MenuManager_ManOperateWhenAuto_StateMachine[7];
/** State tempUnit (13) */
extern Fsm_EventEntryStruct MenuManager_TempUnit_StateMachine[7];
/** State drainValveStatus (14) */
extern Fsm_EventEntryStruct MenuManager_DrainValveStatus_StateMachine[7];
/** State inputStatusSetup (15) */
extern Fsm_EventEntryStruct MenuManager_InputStatusSetup_StateMachine[9];
/** State doorClosed (16) */
extern Fsm_EventEntryStruct MenuManager_DoorClosed_StateMachine[7];
/** State extractShock (17) */
extern Fsm_EventEntryStruct MenuManager_ExtractShock_StateMachine[7];
/** State emergencyStop (18) */
extern Fsm_EventEntryStruct MenuManager_EmergencyStop_StateMachine[7];
/** State inverterError (19) */
extern Fsm_EventEntryStruct MenuManager_InverterError_StateMachine[7];
/** State fillLevelSetup (20) */
extern Fsm_EventEntryStruct MenuManager_FillLevelSetup_StateMachine[13];
/** State autoRefillWhenLow (21) */
extern Fsm_EventEntryStruct MenuManager_AutoRefillWhenLow_StateMachine[7];
/** State zeroLevel (22) */
extern Fsm_EventEntryStruct MenuManager_ZeroLevel_StateMachine[9];
/** State lowLevel (23) */
extern Fsm_EventEntryStruct MenuManager_LowLevel_StateMachine[9];
/** State midLevel (24) */
extern Fsm_EventEntryStruct MenuManager_MidLevel_StateMachine[9];
/** State highLevel (25) */
extern Fsm_EventEntryStruct MenuManager_HighLevel_StateMachine[9];
/** State overbrimLevel (26) */
extern Fsm_EventEntryStruct MenuManager_OverbrimLevel_StateMachine[9];
/** State levelDiffRefill (27) */
extern Fsm_EventEntryStruct MenuManager_LevelDiffRefill_StateMachine[9];
/** State maxTimeFill (28) */
extern Fsm_EventEntryStruct MenuManager_MaxTimeFill_StateMachine[9];
/** State heatTempSetup (29) */
extern Fsm_EventEntryStruct MenuManager_HeatTempSetup_StateMachine[11];
/** State autoReheatWhenLow (30) */
extern Fsm_EventEntryStruct MenuManager_AutoReheatWhenLow_StateMachine[7];
/** State minWaterTemp (31) */
extern Fsm_EventEntryStruct MenuManager_MinWaterTemp_StateMachine[9];
/** State maxWaterTemp (32) */
extern Fsm_EventEntryStruct MenuManager_MaxWaterTemp_StateMachine[9];
/** State tempThreshold (33) */
extern Fsm_EventEntryStruct MenuManager_TempThreshold_StateMachine[9];
/** State tempDiffReheat (34) */
extern Fsm_EventEntryStruct MenuManager_TempDiffReheat_StateMachine[9];
/** State maxTimeHeat (35) */
extern Fsm_EventEntryStruct MenuManager_MaxTimeHeat_StateMachine[9];
/** State soapSetup (36) */
extern Fsm_EventEntryStruct MenuManager_SoapSetup_StateMachine[9];
/** State stopFillWhenSoap (37) */
extern Fsm_EventEntryStruct MenuManager_StopFillWhenSoap_StateMachine[7];
/** State timeSoap1 (38) */
extern Fsm_EventEntryStruct MenuManager_TimeSoap1_StateMachine[9];
/** State timeSoap2 (39) */
extern Fsm_EventEntryStruct MenuManager_TimeSoap2_StateMachine[9];
/** State timeSoap3 (40) */
extern Fsm_EventEntryStruct MenuManager_TimeSoap3_StateMachine[9];
/** State washSetup (41) */
extern Fsm_EventEntryStruct MenuManager_WashSetup_StateMachine[16];
/** State minPauseFwdRev (42) */
extern Fsm_EventEntryStruct MenuManager_MinPauseFwdRev_StateMachine[9];
/** State stdWashRunTime (43) */
extern Fsm_EventEntryStruct MenuManager_StdWashRunTime_StateMachine[9];
/** State stdWashStopTime (44) */
extern Fsm_EventEntryStruct MenuManager_StdWashStopTime_StateMachine[9];
/** State delWashRunTime (45) */
extern Fsm_EventEntryStruct MenuManager_DelWashRunTime_StateMachine[9];
/** State delWashStopTime (46) */
extern Fsm_EventEntryStruct MenuManager_DelWashStopTime_StateMachine[9];
/** State hvyWashRunTime (47) */
extern Fsm_EventEntryStruct MenuManager_HvyWashRunTime_StateMachine[9];
/** State hvyWashStopTime (48) */
extern Fsm_EventEntryStruct MenuManager_HvyWashStopTime_StateMachine[9];
/** State stdWashSpeed (49) */
extern Fsm_EventEntryStruct MenuManager_StdWashSpeed_StateMachine[7];
/** State delWashSpeed (50) */
extern Fsm_EventEntryStruct MenuManager_DelWashSpeed_StateMachine[7];
/** State hvyWashSpeed (51) */
extern Fsm_EventEntryStruct MenuManager_HvyWashSpeed_StateMachine[7];
/** State maxWashSpeed (52) */
extern Fsm_EventEntryStruct MenuManager_MaxWashSpeed_StateMachine[7];
/** State extractSetup (53) */
extern Fsm_EventEntryStruct MenuManager_ExtractSetup_StateMachine[28];
/** State balanceLevel (54) */
extern Fsm_EventEntryStruct MenuManager_BalanceLevel_StateMachine[9];
/** State balanceDrainLevel (55) */
extern Fsm_EventEntryStruct MenuManager_BalanceDrainLevel_StateMachine[9];
/** State balanceWithWaterTime (56) */
extern Fsm_EventEntryStruct MenuManager_BalanceWithWaterTime_StateMachine[9];
/** State balanceDrainWaterTime (57) */
extern Fsm_EventEntryStruct MenuManager_BalanceDrainWaterTime_StateMachine[9];
/** State balanceDelayTime (58) */
extern Fsm_EventEntryStruct MenuManager_BalanceDelayTime_StateMachine[9];
/** State fwdRunTime (59) */
extern Fsm_EventEntryStruct MenuManager_FwdRunTime_StateMachine[9];
/** State midExtractTime (60) */
extern Fsm_EventEntryStruct MenuManager_MidExtractTime_StateMachine[9];
/** State midExtractDelayTime (61) */
extern Fsm_EventEntryStruct MenuManager_MidExtractDelayTime_StateMachine[9];
/** State highExtractTime1 (62) */
extern Fsm_EventEntryStruct MenuManager_HighExtractTime1_StateMachine[9];
/** State highExtractTime2 (63) */
extern Fsm_EventEntryStruct MenuManager_HighExtractTime2_StateMachine[9];
/** State highExtractTime3 (64) */
extern Fsm_EventEntryStruct MenuManager_HighExtractTime3_StateMachine[9];
/** State highExtractDelayTime (65) */
extern Fsm_EventEntryStruct MenuManager_HighExtractDelayTime_StateMachine[9];
/** State maxExtractTime (66) */
extern Fsm_EventEntryStruct MenuManager_MaxExtractTime_StateMachine[9];
/** State reextractTime (67) */
extern Fsm_EventEntryStruct MenuManager_ReextractTime_StateMachine[7];
/** State reextractWashTime (68) */
extern Fsm_EventEntryStruct MenuManager_ReextractWashTime_StateMachine[9];
/** State balanceSpeed (69) */
extern Fsm_EventEntryStruct MenuManager_BalanceSpeed_StateMachine[7];
/** State fwdRunSpeed (70) */
extern Fsm_EventEntryStruct MenuManager_FwdRunSpeed_StateMachine[7];
/** State midExtractSpeed (71) */
extern Fsm_EventEntryStruct MenuManager_MidExtractSpeed_StateMachine[7];
/** State highExtractSpeed1 (72) */
extern Fsm_EventEntryStruct MenuManager_HighExtractSpeed1_StateMachine[7];
/** State highExtractSpeed2 (73) */
extern Fsm_EventEntryStruct MenuManager_HighExtractSpeed2_StateMachine[7];
/** State highExtractSpeed3 (74) */
extern Fsm_EventEntryStruct MenuManager_HighExtractSpeed3_StateMachine[7];
/** State maxMidExtractSpeed (75) */
extern Fsm_EventEntryStruct MenuManager_MaxMidExtractSpeed_StateMachine[7];
/** State maxHighExtractSpeed (76) */
extern Fsm_EventEntryStruct MenuManager_MaxHighExtractSpeed_StateMachine[7];
/** State doorLockSetup (77) */
extern Fsm_EventEntryStruct MenuManager_DoorLockSetup_StateMachine[9];
/** State useLockStopButton (78) */
extern Fsm_EventEntryStruct MenuManager_UseLockStopButton_StateMachine[7];
/** State doorLockValveStatus (79) */
extern Fsm_EventEntryStruct MenuManager_DoorLockValveStatus_StateMachine[7];
/** State unlockDoorTemp (80) */
extern Fsm_EventEntryStruct MenuManager_UnlockDoorTemp_StateMachine[9];
/** State unlockDoorLevel (81) */
extern Fsm_EventEntryStruct MenuManager_UnlockDoorLevel_StateMachine[9];
/** State programSetup (82) */
extern Fsm_EventEntryStruct MenuManager_ProgramSetup_StateMachine[6];
/** State programStepSetup (83) */
extern Fsm_EventEntryStruct MenuManager_ProgramStepSetup_StateMachine[6];
/** State programMainSetup (84) */
extern Fsm_EventEntryStruct MenuManager_ProgramMainSetup_StateMachine[13];
/** State stepIsActive (85) */
extern Fsm_EventEntryStruct MenuManager_StepIsActive_StateMachine[7];
/** State stepWaterMode (86) */
extern Fsm_EventEntryStruct MenuManager_StepWaterMode_StateMachine[7];
/** State stepSoapMode (87) */
extern Fsm_EventEntryStruct MenuManager_StepSoapMode_StateMachine[9];
/** State stepWashMode (88) */
extern Fsm_EventEntryStruct MenuManager_StepWashMode_StateMachine[8];
/** State stepWashModeCustom (89) */
extern Fsm_EventEntryStruct MenuManager_StepWashModeCustom_StateMachine[8];
/** State stepWashRunTime (90) */
extern Fsm_EventEntryStruct MenuManager_StepWashRunTime_StateMachine[9];
/** State stepWashStopTime (91) */
extern Fsm_EventEntryStruct MenuManager_StepWashStopTime_StateMachine[9];
/** State stepWashSpeed (92) */
extern Fsm_EventEntryStruct MenuManager_StepWashSpeed_StateMachine[7];
/** State stepWashNum (93) */
extern Fsm_EventEntryStruct MenuManager_StepWashNum_StateMachine[9];
/** State stepTempMode (94) */
extern Fsm_EventEntryStruct MenuManager_StepTempMode_StateMachine[8];
/** State stepTempModeCustom (95) */
extern Fsm_EventEntryStruct MenuManager_StepTempModeCustom_StateMachine[9];
/** State stepLevelMode (96) */
extern Fsm_EventEntryStruct MenuManager_StepLevelMode_StateMachine[8];
/** State stepLevelModeCustom (97) */
extern Fsm_EventEntryStruct MenuManager_StepLevelModeCustom_StateMachine[9];
/** State stepExtractParam (98) */
extern Fsm_EventEntryStruct MenuManager_StepExtractParam_StateMachine[10];
/** State stepBalanceTime (99) */
extern Fsm_EventEntryStruct MenuManager_StepBalanceTime_StateMachine[9];
/** State stepMidExtractTime (100) */
extern Fsm_EventEntryStruct MenuManager_StepMidExtractTime_StateMachine[9];
/** State stepHighExtractTime1 (101) */
extern Fsm_EventEntryStruct MenuManager_StepHighExtractTime1_StateMachine[9];
/** State stepHighExtractTime2 (102) */
extern Fsm_EventEntryStruct MenuManager_StepHighExtractTime2_StateMachine[9];
/** State stepHighExtractTime3 (101) */
extern Fsm_EventEntryStruct MenuManager_StepHighExtractTime3_StateMachine[9];



/** Menu internal data structure */
MenuManager_InternalDataStruct MenuManager_InternalData;

/** Context Structure of the Finite State Machine (FSM) */
Fsm_ContextStruct MenuManager_FsmContext;

/** List of all states */
Fsm_StateDefStruct MenuManager_StateMachine[104] =
{
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_PowerOn_StateMachine)                            },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_Init_StateMachine)                               },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_Home_StateMachine)                               },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MainSetting_StateMachine)                        },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_SetToDefault_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_SetDefaultParam_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_SetDefaultProg_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MachineSetup_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MachineFuncSetup_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DrainWhileDoorOpen_StateMachine)                 },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HeatUseTimeout_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_FillUseTimeout_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ManOperateWhenAuto_StateMachine)                 },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_TempUnit_StateMachine)                           },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DrainValveStatus_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_InputStatusSetup_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DoorClosed_StateMachine)                         },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ExtractShock_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_EmergencyStop_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_InverterError_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_FillLevelSetup_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_AutoRefillWhenLow_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ZeroLevel_StateMachine)                          },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_LowLevel_StateMachine)                           },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MidLevel_StateMachine)                           },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighLevel_StateMachine)                          },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_OverbrimLevel_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_LevelDiffRefill_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MaxTimeFill_StateMachine)                        },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HeatTempSetup_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_AutoReheatWhenLow_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MinWaterTemp_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MaxWaterTemp_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_TempThreshold_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_TempDiffReheat_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MaxTimeHeat_StateMachine)                        },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_SoapSetup_StateMachine)                          },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StopFillWhenSoap_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_TimeSoap1_StateMachine)                          },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_TimeSoap2_StateMachine)                          },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_TimeSoap3_StateMachine)                          },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_WashSetup_StateMachine)                          },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MinPauseFwdRev_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StdWashRunTime_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StdWashStopTime_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DelWashRunTime_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DelWashStopTime_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HvyWashRunTime_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HvyWashStopTime_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StdWashSpeed_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DelWashSpeed_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HvyWashSpeed_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MaxWashSpeed_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ExtractSetup_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_BalanceLevel_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_BalanceDrainLevel_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_BalanceWithWaterTime_StateMachine)               },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_BalanceDrainWaterTime_StateMachine)              },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_BalanceDelayTime_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_FwdRunTime_StateMachine)                         },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MidExtractTime_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MidExtractDelayTime_StateMachine)                },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighExtractTime1_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighExtractTime2_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighExtractTime3_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighExtractDelayTime_StateMachine)               },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MaxExtractTime_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ReextractTime_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ReextractWashTime_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_BalanceSpeed_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_FwdRunSpeed_StateMachine)                        },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MidExtractSpeed_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighExtractSpeed1_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighExtractSpeed2_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_HighExtractSpeed3_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MaxMidExtractSpeed_StateMachine)                 },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MaxHighExtractSpeed_StateMachine)                },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DoorLockSetup_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_UseLockStopButton_StateMachine)                  },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_DoorLockValveStatus_StateMachine)                },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_UnlockDoorTemp_StateMachine)                     },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_UnlockDoorLevel_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ProgramSetup_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ProgramStepSetup_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_ProgramMainSetup_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepIsActive_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepWaterMode_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepSoapMode_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepWashMode_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepWashModeCustom_StateMachine)                 },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepWashRunTime_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepWashStopTime_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepWashSpeed_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepWashNum_StateMachine)                        },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepTempMode_StateMachine)                       },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepTempModeCustom_StateMachine)                 },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepLevelMode_StateMachine)                      },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepLevelModeCustom_StateMachine)                },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepExtractParam_StateMachine)                   },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepBalanceTime_StateMachine)                    },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepMidExtractTime_StateMachine)                 },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepHighExtractTime1_StateMachine)               },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepHighExtractTime2_StateMachine)               },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_StepHighExtractTime3_StateMachine)               }
};

void (*MenuManager_SubMainFunction)(void);
void (*MenuManager_SubTickHandler)(void);



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void MenuManager_Init(void)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  MenuManager_InternalData.internalDataCurIdx = (uint8_t)0U;
  
  /* Start State Machine */
	MenuManager_FsmContext.stateDefinitions = MenuManager_StateMachine;
	MenuManager_FsmContext.stateCount = FSM_ARRAY_SIZE(MenuManager_StateMachine);
  
  /* Execute POWER ON state */
	Fsm_Init(&MenuManager_FsmContext, (Fsm_StateType)MENUMANAGER_STATE_POWER_ON);

  /* Switch to INIT state */
  Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_NEXT);
  Fsm_StateTask(&MenuManager_FsmContext);
}

/*=============================================================================================*/
void MenuManager_MainFunction(void)
{
  Fsm_StateTask(&MenuManager_FsmContext);
  
  if (MenuManager_SubMainFunction != NULL)
  {
    MenuManager_SubMainFunction();
  }
}



/*=============================================================================================*/
MenuManager_StateType MenuManager_GetCurrentState(void)
{
  return (MenuManager_StateType)(MenuManager_FsmContext.state);
}



/*=============================================================================================*/
void MenuManager_InternalDataPush(uint8_t length)
{
  if (MenuManager_InternalData.internalDataCurIdx <= MENUMANAGER_SUBMENU_TREE_DEPTH)
  {
    MenuManager_InternalData.internalDataCurIdx++;

    MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].length = length;
    MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].ptr = (uint32_t *)MenuManager_malloc((size_t)length * sizeof(uint32_t));
  }
}

/*=============================================================================================*/
void MenuManager_InternalDataPop(void)
{
  if (MenuManager_InternalData.internalDataCurIdx > (uint8_t)0U)
  {
    MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].length = (uint8_t)0U;
    MenuManager_free(MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].ptr);

    MenuManager_InternalData.internalDataCurIdx--;
  }
}



#ifdef  __cplusplus
}
#endif
