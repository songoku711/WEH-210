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

/** Program manager internal data definitions */
#define PROGRAMMANAGER_INIT_INTERNALDATALENGTH                        (uint8_t)2U

#define PROGRAMMANAGER_INIT_COUNTDOWN_IDX                             0U
#define PROGRAMMANAGER_INIT_INITSTATE_IDX                             1U

#define ProgramManager_Init_Countdown                                 ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_INIT_COUNTDOWN_IDX)
#define ProgramManager_Init_InitState                                 ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_INIT_INITSTATE_IDX)

#define PROGRAMMANAGER_INIT_COUNTDOWN_MAX                             (uint32_t)10U

#define PROGRAMMANAGER_INIT_INITSTATE_READY                           (uint32_t)0U
#define PROGRAMMANAGER_INIT_INITSTATE_INIT_DEF_PROG                   (uint32_t)1U
#define PROGRAMMANAGER_INIT_INITSTATE_READ_PROG                       (uint32_t)2U
#define PROGRAMMANAGER_INIT_INITSTATE_WAIT_COM                        (uint32_t)3U
#define PROGRAMMANAGER_INIT_INITSTATE_GET_POWER_HWID                  (uint32_t)4U
#define PROGRAMMANAGER_INIT_INITSTATE_SET_PARAM                       (uint32_t)5U
#define PROGRAMMANAGER_INIT_INITSTATE_GET_POWER_STAT                  (uint32_t)6U
#define PROGRAMMANAGER_INIT_INITSTATE_DONE                            (uint32_t)7U
#define PROGRAMMANAGER_INIT_INITSTATE_FAIL                            (uint32_t)255U



/** Program manager event handlers */
static Fsm_GuardType ProgramManager_Init_Entry                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Init_Exit                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_Init_StateMachine[3] =
{
  FSM_TRIGGER_ENTRY         (                                         ProgramManager_Init_Entry                                                       ),
  FSM_TRIGGER_EXIT          (                                         ProgramManager_Init_Exit                                                        ),
  FSM_TRIGGER_TRANSITION    ( PROGRAMMANAGER_EVENT_NEXT,                                                      PROGRAMMANAGER_STATE_IDLE               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_Init_SubMainFunction(void);
static void ProgramManager_Init_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static Fsm_GuardType ProgramManager_Init_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Common_MemInit_DataHierachyStruct* enterDataHierachy;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy == NULL)
  {
    return FSM_GUARD_FALSE;
  }

  /* Check if previous state data hierachy ID is correct */
  if (pFsmContext->dataHierachy->dataId != PROGRAMMANAGER_STATE_POWER_ON)
  {
    return FSM_GUARD_FALSE;
  }

  /* Allocate and initialize internal data value */
  ProgramManager_InternalDataPush(PROGRAMMANAGER_INIT_INTERNALDATALENGTH);

  ProgramManager_Init_Countdown = (uint32_t)0U;
  ProgramManager_Init_InitState = PROGRAMMANAGER_INIT_INITSTATE_READY;

  /* Store pointer function */
  ProgramManager_SubMainFunctionPush(ProgramManager_Init_SubMainFunction);
  ProgramManager_SubTickHandler = ProgramManager_Init_SubTickHandler;

  enterDataHierachy = (ProgramManager_Common_MemInit_DataHierachyStruct *)(pFsmContext->dataHierachy);
  pFsmContext->dataHierachy = NULL;

  /* Check if program database in external memory is ready */
  if (enterDataHierachy->isMemReady == false)
  {
    ProgramManager_Init_InitState = PROGRAMMANAGER_INIT_INITSTATE_INIT_DEF_PROG;
  }
  else
  {
    ProgramManager_Init_InitState = PROGRAMMANAGER_INIT_INITSTATE_READ_PROG;
  }

  /* Read program parameters and current sequence */
  ProgramManager_ParamConfigSetup_GetData(&ProgramManager_gParamConfig);
  ProgramManager_AutoSeqConfig_GetData(&ProgramManager_gAutoSeqConfig);
  ProgramManager_ManualSeqConfig_GetData(&ProgramManager_gManualSeqConfig);

  /* Release previous state data hierachy */
  ProgramManager_free(enterDataHierachy);

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_Init_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_Init_SubMainFunction(void)
{
  switch (ProgramManager_Init_InitState)
  {
    case PROGRAMMANAGER_INIT_INITSTATE_READY:
    {
      ProgramManager_Init_InitState = PROGRAMMANAGER_INIT_INITSTATE_INIT_DEF_PROG;

      break;
    }
    case PROGRAMMANAGER_INIT_INITSTATE_INIT_DEF_PROG:
    {
      /* Initialize program database by setting default factory value */
      ProgramManager_ParamConfigSetup_SetData((ProgramManager_ParamConfigSetupStruct *)&ProgramManager_gParamDefConfig);
      ProgramManager_AutoSeqConfig_SetData((ProgramManager_AutoSeqConfigStruct *)&ProgramManager_gAutoSeqDefConfig);
      ProgramManager_ManualSeqConfig_SetData((ProgramManager_ManualSeqConfigStruct *)&ProgramManager_gManualSeqDefConfig);

      extMemIf.writeWord(PROGRAMMANAGER_MAGIC_NUMBER_BASE_ADDR, PROGRAMMANAGER_CONF_INIT_MAGIC_NUMBER);

      ProgramManager_Init_InitState = PROGRAMMANAGER_INIT_INITSTATE_READ_PROG;
      break;
    }
    case PROGRAMMANAGER_INIT_INITSTATE_READ_PROG:
    {
      /* Read program parameters and current sequence */
      ProgramManager_ParamConfigSetup_GetData(&ProgramManager_gParamConfig);
      ProgramManager_AutoSeqConfig_GetData(&ProgramManager_gAutoSeqConfig);
      ProgramManager_ManualSeqConfig_GetData(&ProgramManager_gManualSeqConfig);

      // ProgramManager_Init_InitState = PROGRAMMANAGER_INIT_INITSTATE_WAIT_COM;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_EVENT_NEXT);

      ProgramManager_Init_InitState = PROGRAMMANAGER_INIT_INITSTATE_DONE;
      break;
    }
    case PROGRAMMANAGER_INIT_INITSTATE_WAIT_COM:
    case PROGRAMMANAGER_INIT_INITSTATE_GET_POWER_HWID:
    case PROGRAMMANAGER_INIT_INITSTATE_SET_PARAM:
    case PROGRAMMANAGER_INIT_INITSTATE_GET_POWER_STAT:
    case PROGRAMMANAGER_INIT_INITSTATE_DONE:
    case PROGRAMMANAGER_INIT_INITSTATE_FAIL:
    default:
      break;
  }
}

/*=============================================================================================*/
static void ProgramManager_Init_SubTickHandler(void)
{

}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/




#ifdef  __cplusplus
}
#endif
