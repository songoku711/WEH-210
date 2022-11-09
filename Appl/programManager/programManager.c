/* 
 * File:   programManager.c
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




/*===============================================================================================
*                                        ENUMERATIONS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

ProgramManager_ParamConfigSetupStruct ProgramManager_gParamConfig;
ProgramManager_AutoSeqConfigStruct ProgramManager_gAutoSeqConfig;
ProgramManager_ManualSeqConfigStruct ProgramManager_gManualSeqConfig;

/** Program internal data structure */
ProgramManager_InternalDataStruct ProgramManager_InternalData;

/** Context Structure of the Finite State Machine (FSM) */
Fsm_ContextStruct ProgramManager_FsmContext;

ProgramManager_SubMainFunctionStruct ProgramManager_gSubMainFunctionConfig;
void (*ProgramManager_SubTickHandler)(void);



/** Application state machine */
/** State powerOn (0) */
extern Fsm_EventEntryStruct ProgramManager_PowerOn_StateMachine[2];
/** State init (1) */
extern Fsm_EventEntryStruct ProgramManager_Init_StateMachine[3];
/** State idle (2) */
extern Fsm_EventEntryStruct ProgramManager_Idle_StateMachine[4];
/** State auto (3) */
extern Fsm_EventEntryStruct ProgramManager_AutoInit_StateMachine[2];
/** State manual (4) */
extern Fsm_EventEntryStruct ProgramManager_ManualInit_StateMachine[2];

/** List of all states */
Fsm_StateDefStruct ProgramManager_ApplFsmStates[5] =
{
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(ProgramManager_PowerOn_StateMachine)                         },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(ProgramManager_Init_StateMachine)                            },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(ProgramManager_Idle_StateMachine)                            },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(ProgramManager_AutoInit_StateMachine)                        },
  { FSM_DEFAULT_STATE,        FSM_SIZE_AND_ARRAY(ProgramManager_ManualInit_StateMachine)                      }
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

void ProgramManager_Init(void)
{
  ProgramManager_gSubMainFunctionConfig.subMainFunctionNum = (uint8_t)0U;
  ProgramManager_SubTickHandler = NULL;

  ProgramManager_InternalData.internalDataCurIdx = (uint8_t)0U;

  ProgramManager_Control_Init();
  
  /* Start State Machine */
	ProgramManager_FsmContext.stateDefinitions = ProgramManager_ApplFsmStates;
	ProgramManager_FsmContext.stateCount = FSM_ARRAY_SIZE(ProgramManager_ApplFsmStates);
  
  /* Execute POWER ON state */
	Fsm_Init(&ProgramManager_FsmContext, (Fsm_StateType)PROGRAMMANAGER_STATE_POWER_ON);

  /* Execute INIT state */
  Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_EVENT_NEXT);
  Fsm_StateTask(&ProgramManager_FsmContext);
}

/*=============================================================================================*/
/* Task periodic time is 5ms */
void ProgramManager_MainFunction(void)
{
  uint8_t subMainFunctionIdx;

  Fsm_StateTask(&ProgramManager_FsmContext);
  
  for (subMainFunctionIdx = (uint8_t)0U; \
       subMainFunctionIdx < ProgramManager_gSubMainFunctionConfig.subMainFunctionNum; \
       subMainFunctionIdx++)
  {
    (ProgramManager_gSubMainFunctionConfig.subMainFunction)[subMainFunctionIdx]();
  }
}



/*=============================================================================================*/
void ProgramManager_SubMainFunctionPush(void (*subMainFunction)(void))
{
  if (ProgramManager_gSubMainFunctionConfig.subMainFunctionNum < PROGRAMMANAGER_SUBMAINFUNCTION_NUM_MAX)
  {
    (ProgramManager_gSubMainFunctionConfig.subMainFunction)[ProgramManager_gSubMainFunctionConfig.subMainFunctionNum] = subMainFunction;
    ProgramManager_gSubMainFunctionConfig.subMainFunctionNum++;
  }
}

/*=============================================================================================*/
void ProgramManager_SubMainFunctionPop(void)
{
  if (ProgramManager_gSubMainFunctionConfig.subMainFunctionNum > (uint8_t)0U)
  {
    (ProgramManager_gSubMainFunctionConfig.subMainFunction)[ProgramManager_gSubMainFunctionConfig.subMainFunctionNum] = NULL;
    ProgramManager_gSubMainFunctionConfig.subMainFunctionNum--;
  }
}



/*=============================================================================================*/
ProgramManager_StateType ProgramManager_GetCurrentState(void)
{
  return (ProgramManager_StateType)(ProgramManager_FsmContext.state);
}



/*=============================================================================================*/
void ProgramManager_InternalDataPush(uint8_t length)
{
  if (ProgramManager_InternalData.internalDataCurIdx <= PROGRAMMANAGER_SUBMENU_TREE_DEPTH)
  {
    ProgramManager_InternalData.internalDataCurIdx++;

    ProgramManager_InternalData.internalDataArr[ProgramManager_InternalData.internalDataCurIdx].length = length;
    ProgramManager_InternalData.internalDataArr[ProgramManager_InternalData.internalDataCurIdx].ptr = (uint32_t *)ProgramManager_malloc((size_t)length * sizeof(uint32_t));
  }
}

/*=============================================================================================*/
void ProgramManager_InternalDataPop(void)
{
  if (ProgramManager_InternalData.internalDataCurIdx > (uint8_t)0U)
  {
    ProgramManager_InternalData.internalDataArr[ProgramManager_InternalData.internalDataCurIdx].length = (uint8_t)0U;
    ProgramManager_free(ProgramManager_InternalData.internalDataArr[ProgramManager_InternalData.internalDataCurIdx].ptr);

    ProgramManager_InternalData.internalDataCurIdx--;
  }
}



#ifdef  __cplusplus
}
#endif
