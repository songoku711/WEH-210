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
static tFsmGuard ProgramManager_Appl_Manual_Entry             (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard ProgramManager_Appl_Manual_Exit              (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard ProgramManager_Appl_Manual_Prev              (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Application state machine */
tFsmEventEntry ProgramManager_ApplState_Manual[3] =
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

static tFsmGuard ProgramManager_Appl_Manual_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  ProgramManager_SubMainFunction = ProgramManager_Appl_Manual_SubMainFunction;
  ProgramManager_SubTickHandler = ProgramManager_Appl_Manual_SubTickHandler;

  return kFsmGuard_True;
}

static tFsmGuard ProgramManager_Appl_Manual_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  ProgramManager_SubMainFunction = NULL;
  ProgramManager_SubTickHandler = NULL;

  return kFsmGuard_True;
}

static tFsmGuard ProgramManager_Appl_Manual_Prev(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  return kFsmGuard_True;
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
