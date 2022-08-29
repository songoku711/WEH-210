/* 
 * File:   _B000000000_program_Init.c
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
} ProgramManager_Appl_Init_EnterDataHierachy;



/** Application event handlers */
static tFsmGuard ProgramManager_Appl_Init_Entry               (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Application state machine */
tFsmEventEntry ProgramManager_ApplState_Init[2] =
{
  FSM_TRIGGER_ENTRY         (                                       ProgramManager_Appl_Init_Entry                                            ),
  FSM_TRIGGER_TRANSITION    (PROGRAMMANAGER_APPL_EVENT_NEXT,                                                    PROGRAMMANAGER_APPL_STATE_IDLE)
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static tFsmGuard ProgramManager_Appl_Init_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  ProgramManager_Appl_Init_EnterDataHierachy* enterDataHierachy;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy == NULL)
  {
    return kFsmGuard_False;
  }

  /* Check if previous state data hierachy ID is correct */
  if (pFsmContext->dataHierachy->dataId != PROGRAMMANAGER_APPL_STATE_POWER_ON)
  {
    return kFsmGuard_False;
  }

  enterDataHierachy = (ProgramManager_Appl_Init_EnterDataHierachy *)(pFsmContext->dataHierachy);
  pFsmContext->dataHierachy = NULL;

  /* Check if program database in external memory is ready */
  if (enterDataHierachy->isMemReady != true)
  {
    /* Initialize program database by setting default factory value */
    ProgramManager_ParamConfigSetup_SetData((ProgramManager_ParamConfigSetupStruct *)&ProgramManager_gParamDefConfig);
    ProgramManager_AutoSeqConfig_SetData((ProgramManager_AutoSeqConfigStruct *)&ProgramManager_gAutoSeqDefConfig);
    ProgramManager_ManualSeqConfig_SetData((ProgramManager_ManualSeqConfigStruct *)&ProgramManager_gManualSeqDefConfig);

    extMemIf.writeWord(PROGRAMMANAGER_MAGIC_NUMBER_BASE_ADDR, PROGRAMMANAGER_CONF_INIT_MAGIC_NUMBER);
  }

  /* Read program parameters and current sequence */
  ProgramManager_ParamConfigSetup_GetData(&ProgramManager_gParamConfig);
  ProgramManager_AutoSeqConfig_GetData(&ProgramManager_gAutoSeqConfig);
  ProgramManager_ManualSeqConfig_GetData(&ProgramManager_gManualSeqConfig);

  /* Release previous state data hierachy */
  ProgramManager_free(enterDataHierachy);

  return kFsmGuard_True;
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/




#ifdef  __cplusplus
}
#endif
