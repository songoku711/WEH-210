/* 
 * File:   _0000000000_program_Idle.c
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

/** Application event handlers */
static Fsm_GuardType ProgramManager_Idle_Entry                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Idle_Exit                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Idle_Next                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Idle_Next_2                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Application state machine */
Fsm_EventEntryStruct ProgramManagerState_Idle[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_Idle_Entry                                            ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_Idle_Exit                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( PROGRAMMANAGER_EVENT_NEXT,          ProgramManager_Idle_Next,              PROGRAMMANAGER_STATE_AUTO),
  FSM_TRIGGER_TRANSITION_ACTION ( PROGRAMMANAGER_EVENT_NEXT_2,   ProgramManager_Idle_Next_2,            PROGRAMMANAGER_STATE_MANUAL)
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_Idle_SubMainFunction(void);
static void ProgramManager_Idle_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static Fsm_GuardType ProgramManager_Idle_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunction = ProgramManager_Idle_SubMainFunction;
  ProgramManager_SubTickHandler = ProgramManager_Idle_SubTickHandler;

  return FSM_GUARD_TRUE;
}

static Fsm_GuardType ProgramManager_Idle_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunction = NULL;
  ProgramManager_SubTickHandler = NULL;

  return FSM_GUARD_TRUE;
}

static Fsm_GuardType ProgramManager_Idle_Next(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  return FSM_GUARD_TRUE;
}

static Fsm_GuardType ProgramManager_Idle_Next_2(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  return FSM_GUARD_TRUE;
}

static void ProgramManager_Idle_SubMainFunction(void)
{

}

static void ProgramManager_Idle_SubTickHandler(void)
{

}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/




#ifdef  __cplusplus
}
#endif
