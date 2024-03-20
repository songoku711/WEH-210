/* 
 * File:   _2200000000_program_ManualPostRun.c
 * Author: Long
 * 
 * In PRE-RUN state, it's assumed that current step is available
 * to run, we will wait for amount of time to calculate parameter,
 * then decide what state is next
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

/** Menu manager internal data definitions */
#define PROGRAMMANAGER_MANUALPOSTRUN_INTERNALDATALENGTH               (uint8_t)3U

#define PROGRAMMANAGER_MANUALPOSTRUN_POST_EXEC_IDX                    0U
#define PROGRAMMANAGER_MANUALPOSTRUN_PREV_STATE_IDX                   1U
#define PROGRAMMANAGER_MANUALPOSTRUN_DATACMD_IDX                      2U

#define ProgramManager_ManualPostRun_PostExec                         ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALPOSTRUN_POST_EXEC_IDX)
#define ProgramManager_ManualPostRun_PrevState                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALPOSTRUN_PREV_STATE_IDX)
#define ProgramManager_ManualPostRun_DataCmd                          ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALPOSTRUN_DATACMD_IDX)

#define PROGRAMMANAGER_MANUALPOSTRUN_EVENT_IDLE                       PROGRAMMANAGER_EVENT_SUBMENU_1



/** Program manager event handlers */
static Fsm_GuardType ProgramManager_ManualPostRun_Entry                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_ManualPostRun_Exit                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_ManualPostRun_StateMachine[3] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_ManualPostRun_Entry                                                ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_ManualPostRun_Exit                                                 ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_MANUALPOSTRUN_EVENT_IDLE,                                        PROGRAMMANAGER_STATE_IDLE               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_ManualPostRun_InternalCommandHandler(void);

static void ProgramManager_ManualPostRun_InternalControlOutput(void);

static void ProgramManager_ManualPostRun_SubMainFunction(void);
static void ProgramManager_ManualPostRun_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void ProgramManager_ManualPostRun_InternalCommandHandler(void)
{
  Fsm_DataHierachyStruct* idleDataHierachy;

  if (ProgramManager_ManualPostRun_PostExec == (uint32_t)0U)
  {
    switch (ProgramManager_ManualPostRun_PrevState)
    {
      case PROGRAMMANAGER_STATE_MANUAL_PRE_RUN:
      case PROGRAMMANAGER_STATE_MANUAL_RUN:
      {
        if (ProgramManager_ManualPostRun_DataCmd == PROGRAMMANAGER_CONTROL_COMMAND_STOP)
        {
          /* Clear all manual option */
          ProgramManager_Control_ClearAllManualOption();

          idleDataHierachy = (Fsm_DataHierachyStruct *)ProgramManager_malloc(sizeof(Fsm_DataHierachyStruct));
          idleDataHierachy->dataId = PROGRAMMANAGER_STATE_MANUAL_POST_RUN;

          ProgramManager_FsmContext.dataHierachy = idleDataHierachy;

          Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_MANUALPOSTRUN_EVENT_IDLE);
        }
        else
        {
          /* nothing here */
        }

        break;
      }
      default:
        break;
    }

    /* Reset global parameters */
    ProgramManager_gTimeCountMin = (uint32_t)0U;
    ProgramManager_gTimeCountSec = (uint32_t)0U;

    ProgramManager_gDrainLevel = (uint8_t)0U;

    ProgramManager_ManualPostRun_PostExec = (uint32_t)1U;
  }
}



/*=============================================================================================*/
static void ProgramManager_ManualPostRun_InternalControlOutput(void)
{
  /* Control drain valve - no control, just left as it was from previous state */
  ProgramManager_Control_ClearAllOutputExceptDrain();
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_ManualPostRun_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Control_PostRunStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_MANUAL_PRE_RUN) || \
        (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_MANUAL_RUN))
    {
      enterDataHierachy = (ProgramManager_Control_PostRunStruct *)(pFsmContext->dataHierachy);

      ProgramManager_InternalDataPush(PROGRAMMANAGER_MANUALPOSTRUN_INTERNALDATALENGTH);

      ProgramManager_ManualPostRun_PrevState = enterDataHierachy->dataId;
      ProgramManager_ManualPostRun_DataCmd = enterDataHierachy->command;

      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else
    {
      retVal = HAL_ERROR;
    }
  }
  else
  {
    retVal = HAL_ERROR;
  }

  if (retVal == HAL_OK)
  {
    ProgramManager_ManualPostRun_PostExec = (uint32_t)0U;
    
    ProgramManager_SubMainFunctionPush(ProgramManager_ManualPostRun_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_ManualPostRun_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_ManualPostRun_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_ManualPostRun_SubMainFunction(void)
{
  ProgramManager_ManualPostRun_InternalCommandHandler();
  ProgramManager_ManualPostRun_InternalControlOutput();
}

/*=============================================================================================*/
static void ProgramManager_ManualPostRun_SubTickHandler(void)
{
  /* nothing here */
}



#ifdef  __cplusplus
}
#endif
