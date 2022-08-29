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
static tFsmGuard ProgramManager_Appl_Idle_Entry               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard ProgramManager_Appl_Idle_Exit                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard ProgramManager_Appl_Idle_Next                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard ProgramManager_Appl_Idle_Next_2              (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Application state machine */
tFsmEventEntry ProgramManager_ApplState_Idle[4] =
{
  FSM_TRIGGER_ENTRY         (                                       ProgramManager_Appl_Idle_Entry                                            ),
  FSM_TRIGGER_EXIT          (                                       ProgramManager_Appl_Idle_Exit                                             ),
  FSM_TRIGGER_TRANSITION_ACTION (PROGRAMMANAGER_APPL_EVENT_NEXT,    ProgramManager_Appl_Idle_Next,              PROGRAMMANAGER_APPL_STATE_AUTO),
  FSM_TRIGGER_TRANSITION_ACTION (PROGRAMMANAGER_APPL_EVENT_NEXT_2,  ProgramManager_Appl_Idle_Next_2,            PROGRAMMANAGER_APPL_STATE_MANUAL)
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_Appl_Idle_SubMainFunction(void);
static void ProgramManager_Appl_Idle_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static tFsmGuard ProgramManager_Appl_Idle_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  ProgramManager_SubMainFunction = ProgramManager_Appl_Idle_SubMainFunction;
  ProgramManager_SubTickHandler = ProgramManager_Appl_Idle_SubTickHandler;

  return kFsmGuard_True;
}

static tFsmGuard ProgramManager_Appl_Idle_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  ProgramManager_SubMainFunction = NULL;
  ProgramManager_SubTickHandler = NULL;

  return kFsmGuard_True;
}

static tFsmGuard ProgramManager_Appl_Idle_Next(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  return kFsmGuard_True;
}

static tFsmGuard ProgramManager_Appl_Idle_Next_2(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  return kFsmGuard_True;
}

static void ProgramManager_Appl_Idle_SubMainFunction(void)
{

}

static void ProgramManager_Appl_Idle_SubTickHandler(void)
{

}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/




#ifdef  __cplusplus
}
#endif
