/* 
 * File:   _2000000000_program_Manual.c
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
static Fsm_GuardType ProgramManager_Appl_Manual_Entry             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Appl_Manual_Exit              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Appl_Manual_Prev              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Application state machine */
Fsm_EventEntryStruct ProgramManager_ApplState_Manual[3] =
{
  FSM_TRIGGER_ENTRY         (                                       ProgramManager_Appl_Manual_Entry                                          ),
  FSM_TRIGGER_EXIT          (                                       ProgramManager_Appl_Manual_Exit                                           ),
  FSM_TRIGGER_TRANSITION_ACTION (PROGRAMMANAGER_APPL_EVENT_PREV,    ProgramManager_Appl_Manual_Prev,            PROGRAMMANAGER_APPL_STATE_IDLE)
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_Appl_Manual_SubMainFunction(void);
static void ProgramManager_Appl_Manual_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static Fsm_GuardType ProgramManager_Appl_Manual_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunction = ProgramManager_Appl_Manual_SubMainFunction;
  ProgramManager_SubTickHandler = ProgramManager_Appl_Manual_SubTickHandler;

  return FSM_GUARD_TRUE;
}

static Fsm_GuardType ProgramManager_Appl_Manual_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunction = NULL;
  ProgramManager_SubTickHandler = NULL;

  return FSM_GUARD_TRUE;
}

static Fsm_GuardType ProgramManager_Appl_Manual_Prev(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  return FSM_GUARD_TRUE;
}

static void ProgramManager_Appl_Manual_SubMainFunction(void)
{

}

static void ProgramManager_Appl_Manual_SubTickHandler(void)
{

}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/




#ifdef  __cplusplus
}
#endif
