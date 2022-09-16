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

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_Idle_Entry                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Idle_Exit                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_Idle_StateMachine[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_Idle_Entry                                                       ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_Idle_Exit                                                        ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_EVENT_SUBMENU_1,                                               PROGRAMMANAGER_STATE_AUTO_INIT          ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_EVENT_SUBMENU_2,                                               PROGRAMMANAGER_STATE_MANUAL_INIT        )
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
  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_INIT) || \
        (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_MANUAL_INIT))
    {
      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_INIT)
    {
      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    ProgramManager_SubMainFunctionPush(ProgramManager_Idle_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_Idle_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_Idle_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_Idle_SubMainFunction(void)
{

}

/*=============================================================================================*/
static void ProgramManager_Idle_SubTickHandler(void)
{

}



#ifdef  __cplusplus
}
#endif
