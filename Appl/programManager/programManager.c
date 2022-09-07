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

/** Context Structure of the Finite State Machine (FSM) */
Fsm_ContextStruct ProgramManager_FsmContext;

void (*ProgramManager_SubMainFunction)(void);
void (*ProgramManager_SubTickHandler)(void);



/** Application state machine */
/** State powerOn (0) */
extern Fsm_EventEntryStruct ProgramManager_ApplState_PowerOn[2];
/** State init (1) */
extern Fsm_EventEntryStruct ProgramManager_ApplState_Init[2];
/** State idle (2) */
extern Fsm_EventEntryStruct ProgramManager_ApplState_Idle[4];
/** State auto (3) */
extern Fsm_EventEntryStruct ProgramManager_ApplState_Auto[3];
/** State manual (4) */
extern Fsm_EventEntryStruct ProgramManager_ApplState_Manual[3];

/** List of all states */
Fsm_StateDefStruct ProgramManager_ApplFsmStates[5] =
{
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(ProgramManager_ApplState_PowerOn) },
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(ProgramManager_ApplState_Init) },
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(ProgramManager_ApplState_Idle) },
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(ProgramManager_ApplState_Auto) },
  { FSM_DEFAULT_STATE, FSM_SIZE_AND_ARRAY(ProgramManager_ApplState_Manual) }
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

void ProgramManager_Init(void)
{
  ProgramManager_SubMainFunction = NULL;
  ProgramManager_SubTickHandler = NULL;
  
  /* Start State Machine */
	ProgramManager_FsmContext.stateDefinitions = ProgramManager_ApplFsmStates;
	ProgramManager_FsmContext.stateCount = FSM_ARRAY_SIZE(ProgramManager_ApplFsmStates);
  
  /* Execute POWER ON state */
	Fsm_Init(&ProgramManager_FsmContext, (Fsm_StateType)PROGRAMMANAGER_APPL_STATE_POWER_ON);

  /* Execute INIT state */
  Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_APPL_EVENT_NEXT);
  Fsm_StateTask(&ProgramManager_FsmContext);

  /* Trigger transition to IDLE state */
  Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_APPL_EVENT_NEXT);
}

void ProgramManager_MainFunction(void)
{
  Fsm_StateTask(&ProgramManager_FsmContext);
  
  if (ProgramManager_SubMainFunction != NULL)
  {
    ProgramManager_SubMainFunction();
  }
}



#ifdef  __cplusplus
}
#endif
