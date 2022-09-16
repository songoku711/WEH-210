/* 
 * File:   _2000000000_program_ManualInit.c
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
static Fsm_GuardType ProgramManager_ManualInit_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_ManualInit_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_ManualInit_StateMachine[2] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_ManualInit_Entry                                                 ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_ManualInit_Exit                                                  )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_ManualInit_SubMainFunction(void);
static void ProgramManager_ManualInit_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static Fsm_GuardType ProgramManager_ManualInit_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPush(ProgramManager_ManualInit_SubMainFunction);
  ProgramManager_SubTickHandler = ProgramManager_ManualInit_SubTickHandler;

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_ManualInit_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_ManualInit_SubMainFunction(void)
{

}

/*=============================================================================================*/
static void ProgramManager_ManualInit_SubTickHandler(void)
{

}



#ifdef  __cplusplus
}
#endif
