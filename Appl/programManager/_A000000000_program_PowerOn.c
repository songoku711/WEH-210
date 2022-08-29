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

/** State data hierachy structure */
typedef struct
{
  uint32_t              stateId;
  bool                  isMemReady;
} ProgramManager_Appl_PowerOn_DataHierachy;



/** Application event handlers */
static tFsmGuard ProgramManager_Appl_PowerOn_Entry            (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Application state machine */
tFsmEventEntry ProgramManager_ApplState_PowerOn[2] =
{
  FSM_TRIGGER_ENTRY         (                                       ProgramManager_Appl_PowerOn_Entry                                         ),
  FSM_TRIGGER_TRANSITION    (PROGRAMMANAGER_APPL_EVENT_NEXT,                                                    PROGRAMMANAGER_APPL_STATE_INIT)
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static tFsmGuard ProgramManager_Appl_PowerOn_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t magicNum;
  ProgramManager_Appl_PowerOn_DataHierachy* dataHierachy;

  /* Get program magic number in external memory */
  magicNum = extMemIf.readWord(PROGRAMMANAGER_MAGIC_NUMBER_BASE_ADDR);

  dataHierachy = (ProgramManager_Appl_PowerOn_DataHierachy*)ProgramManager_malloc(sizeof(ProgramManager_Appl_PowerOn_DataHierachy));
  dataHierachy->stateId = PROGRAMMANAGER_APPL_STATE_POWER_ON;

  /* If magic number matched with default value, it means program database is ready */
  if (PROGRAMMANAGER_CONF_INIT_MAGIC_NUMBER == magicNum)
  {
    dataHierachy->isMemReady = true;
  }
  else
  {
    dataHierachy->isMemReady = false;
  }

  pFsmContext->dataHierachy = (tFsmDataHierachy *)dataHierachy;

  return kFsmGuard_True;
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/




#ifdef  __cplusplus
}
#endif
