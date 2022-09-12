/* 
 * File:   _A000000000_program_PowerOn.c
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
static Fsm_GuardType ProgramManager_PowerOn_Entry                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_PowerOn_StateMachine[2] =
{
  FSM_TRIGGER_ENTRY         (                                         ProgramManager_PowerOn_Entry                                                    ),
  FSM_TRIGGER_TRANSITION    ( PROGRAMMANAGER_EVENT_NEXT,                                                      PROGRAMMANAGER_STATE_INIT               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static Fsm_GuardType ProgramManager_PowerOn_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t magicNum;
  ProgramManager_Common_MemInit_DataHierachyStruct* dataHierachy;

  /* Get program magic number in external memory */
  magicNum = extMemIf.readWord(PROGRAMMANAGER_MAGIC_NUMBER_BASE_ADDR);

  dataHierachy = (ProgramManager_Common_MemInit_DataHierachyStruct*)ProgramManager_malloc(sizeof(ProgramManager_Common_MemInit_DataHierachyStruct));
  dataHierachy->stateId = PROGRAMMANAGER_STATE_POWER_ON;

  /* If magic number matched with default value, it means program database is ready */
  if (PROGRAMMANAGER_CONF_INIT_MAGIC_NUMBER == magicNum)
  {
    dataHierachy->isMemReady = true;
  }
  else
  {
    dataHierachy->isMemReady = false;
  }

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  return FSM_GUARD_TRUE;
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/




#ifdef  __cplusplus
}
#endif
