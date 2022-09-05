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
extern tFsmEventEntry MenuManager_PowerOn_StateMachine[2];
/** State init (1) */
extern tFsmEventEntry MenuManager_Init_StateMachine[3];
/** State home (2) */
extern tFsmEventEntry MenuManager_Home_StateMachine[10];
/** State mainSetting (3) */
extern tFsmEventEntry MenuManager_MainSetting_StateMachine[7];
/** State setToDefault (4) */
extern tFsmEventEntry MenuManager_SetToDefault_StateMachine[7];
/** State setDefaultParam (5) */
extern tFsmEventEntry MenuManager_SetDefaultParam_StateMachine[5];
/** State setDefaultProg (6) */
extern tFsmEventEntry MenuManager_SetDefaultProg_StateMachine[5];
/** State machineSetup (7) */
extern tFsmEventEntry MenuManager_MachineSetup_StateMachine[13];
/** State machineFuncSetup (8) */
extern tFsmEventEntry MenuManager_MachineFuncSetup_StateMachine[11];
/** State drainWhileDoorOpen (9) */
extern tFsmEventEntry MenuManager_DrainWhileDoorOpen_StateMachine[7];
/** State heatUseTimeout (10) */
extern tFsmEventEntry MenuManager_HeatUseTimeout_StateMachine[7];
/** State fillUseTimeout (11) */
extern tFsmEventEntry MenuManager_FillUseTimeout_StateMachine[7];
/** State manOperateWhenAuto (12) */
extern tFsmEventEntry MenuManager_ManOperateWhenAuto_StateMachine[7];
/** State tempUnit (13) */
extern tFsmEventEntry MenuManager_TempUnit_StateMachine[7];
/** State drainValveStatus (14) */
extern tFsmEventEntry MenuManager_DrainValveStatus_StateMachine[7];
/** State inputStatusSetup (15) */
extern tFsmEventEntry MenuManager_InputStatusSetup_StateMachine[9];
/** State doorClosed (16) */
extern tFsmEventEntry MenuManager_DoorClosed_StateMachine[7];
/** State extractShock (17) */
extern tFsmEventEntry MenuManager_ExtractShock_StateMachine[7];
/** State emergencyStop (18) */
extern tFsmEventEntry MenuManager_EmergencyStop_StateMachine[7];
/** State inverterError (19) */
extern tFsmEventEntry MenuManager_InverterError_StateMachine[7];
/** State fillLevelSetup (20) */
extern tFsmEventEntry MenuManager_FillLevelSetup_StateMachine[13];
/** State autoRefillWhenLow (21) */
extern tFsmEventEntry MenuManager_AutoRefillWhenLow_StateMachine[7];
/** State zeroLevel (22) */
extern tFsmEventEntry MenuManager_ZeroLevel_StateMachine[9];
/** State lowLevel (23) */
extern tFsmEventEntry MenuManager_LowLevel_StateMachine[9];
/** State midLevel (24) */
extern tFsmEventEntry MenuManager_MidLevel_StateMachine[9];
/** State highLevel (25) */
extern tFsmEventEntry MenuManager_HighLevel_StateMachine[9];
/** State overbrimLevel (26) */
extern tFsmEventEntry MenuManager_OverbrimLevel_StateMachine[9];
/** State levelDiffRefill (27) */
extern tFsmEventEntry MenuManager_LevelDiffRefill_StateMachine[9];
/** State maxTimeFill (28) */
extern tFsmEventEntry MenuManager_MaxTimeFill_StateMachine[9];
/** State heatTempSetup (29) */
extern tFsmEventEntry MenuManager_HeatTempSetup_StateMachine[11];
/** State autoReheatWhenLow (30) */
extern tFsmEventEntry MenuManager_AutoReheatWhenLow_StateMachine[7];
/** State minWaterTemp (31) */
extern tFsmEventEntry MenuManager_MinWaterTemp_StateMachine[9];
/** State maxWaterTemp (32) */
extern tFsmEventEntry MenuManager_MaxWaterTemp_StateMachine[9];
/** State tempThreshold (33) */
extern tFsmEventEntry MenuManager_TempThreshold_StateMachine[9];
/** State tempDiffReheat (34) */
extern tFsmEventEntry MenuManager_TempDiffReheat_StateMachine[9];
/** State maxTimeHeat (35) */
extern tFsmEventEntry MenuManager_MaxTimeHeat_StateMachine[9];
/** State soapSetup (36) */
extern tFsmEventEntry MenuManager_SoapSetup_StateMachine[9];
/** State stopFillWhenSoap (37) */
extern tFsmEventEntry MenuManager_StopFillWhenSoap_StateMachine[7];
/** State timeSoap1 (38) */
extern tFsmEventEntry MenuManager_TimeSoap1_StateMachine[9];
/** State timeSoap2 (39) */
extern tFsmEventEntry MenuManager_TimeSoap2_StateMachine[9];
/** State timeSoap3 (40) */
extern tFsmEventEntry MenuManager_TimeSoap3_StateMachine[9];
/** State washSetup (41) */
extern tFsmEventEntry MenuManager_WashSetup_StateMachine[16];
/** State minPauseFwdRev (42) */
extern tFsmEventEntry MenuManager_MinPauseFwdRev_StateMachine[9];
/** State stdWashRunTime (43) */
extern tFsmEventEntry MenuManager_StdWashRunTime_StateMachine[9];
/** State stdWashStopTime (44) */
extern tFsmEventEntry MenuManager_StdWashStopTime_StateMachine[9];
/** State delWashRunTime (45) */
extern tFsmEventEntry MenuManager_DelWashRunTime_StateMachine[9];
/** State delWashStopTime (46) */
extern tFsmEventEntry MenuManager_DelWashStopTime_StateMachine[9];
/** State hvyWashRunTime (47) */
extern tFsmEventEntry MenuManager_HvyWashRunTime_StateMachine[9];
/** State hvyWashStopTime (48) */
extern tFsmEventEntry MenuManager_HvyWashStopTime_StateMachine[9];
/** State stdWashSpeed (49) */
extern tFsmEventEntry MenuManager_StdWashSpeed_StateMachine[7];
/** State delWashSpeed (50) */
extern tFsmEventEntry MenuManager_DelWashSpeed_StateMachine[7];
/** State hvyWashSpeed (51) */
extern tFsmEventEntry MenuManager_HvyWashSpeed_StateMachine[7];
/** State maxWashSpeed (52) */
extern tFsmEventEntry MenuManager_MaxWashSpeed_StateMachine[7];
/** State extractSetup (53) */
extern tFsmEventEntry MenuManager_ExtractSetup_StateMachine[28];
/** State balanceLevel (54) */
extern tFsmEventEntry MenuManager_BalanceLevel_StateMachine[9];
/** State balanceDrainLevel (55) */
extern tFsmEventEntry MenuManager_BalanceDrainLevel_StateMachine[9];
/** State balanceWithWaterTime (56) */
extern tFsmEventEntry MenuManager_BalanceWithWaterTime_StateMachine[9];
/** State balanceDrainWaterTime (57) */
extern tFsmEventEntry MenuManager_BalanceDrainWaterTime_StateMachine[9];
/** State balanceDelayTime (58) */
extern tFsmEventEntry MenuManager_BalanceDelayTime_StateMachine[9];
/** State fwdRunTime (59) */
extern tFsmEventEntry MenuManager_FwdRunTime_StateMachine[9];
/** State midExtractTime (60) */
extern tFsmEventEntry MenuManager_MidExtractTime_StateMachine[9];
/** State midExtractDelayTime (61) */
extern tFsmEventEntry MenuManager_MidExtractDelayTime_StateMachine[9];



/** Menu internal data structure */
MenuManager_InternalDataStruct MenuManager_InternalData;

/** Context Structure of the Finite State Machine (FSM) */
tFsmContext MenuManager_FsmContext;

/** List of all states */
tFsmStateDefinition MenuManager_StateMachine[62] =
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
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(MenuManager_MidExtractDelayTime_StateMachine)                }
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
	Fsm_Init(&MenuManager_FsmContext, (tFsmState)MENUMANAGER_STATE_POWER_ON);

  /* Switch to INIT state */
  Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_NEXT);
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
