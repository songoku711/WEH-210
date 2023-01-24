/* 
 * File:   _1500000000_program_AutoPostRun.c
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
#define PROGRAMMANAGER_AUTOPOSTRUN_INTERNALDATALENGTH                 (uint8_t)3U

#define PROGRAMMANAGER_AUTOPOSTRUN_POST_EXEC_IDX                      0U
#define PROGRAMMANAGER_AUTOPOSTRUN_PREV_STATE_IDX                     1U
#define PROGRAMMANAGER_AUTOPOSTRUN_DATACMD_IDX                        2U

#define ProgramManager_AutoPostRun_PostExec                           ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTOPOSTRUN_POST_EXEC_IDX)
#define ProgramManager_AutoPostRun_PrevState                          ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTOPOSTRUN_PREV_STATE_IDX)
#define ProgramManager_AutoPostRun_DataCmd                            ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTOPOSTRUN_DATACMD_IDX)

#define PROGRAMMANAGER_AUTOPOSTRUN_EVENT_PRE_RUN                      PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTOPOSTRUN_EVENT_IDLE                         PROGRAMMANAGER_EVENT_SUBMENU_2



/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoPostRun_Entry                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoPostRun_Exit                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoPostRun_StateMachine[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoPostRun_Entry                                                ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoPostRun_Exit                                                 ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTOPOSTRUN_EVENT_PRE_RUN,                                     PROGRAMMANAGER_STATE_AUTO_PRE_RUN       ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTOPOSTRUN_EVENT_IDLE,                                        PROGRAMMANAGER_STATE_IDLE               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_AutoPostRun_InternalCommandHandler(void);

static void ProgramManager_AutoPostRun_InternalControlOutput(void);

static void ProgramManager_AutoPostRun_SubMainFunction(void);
static void ProgramManager_AutoPostRun_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void ProgramManager_AutoPostRun_InternalCommandHandler(void)
{
  bool retVal;
  ProgramManager_Control_PreRunStruct *preRunDataHierachy;
  Fsm_DataHierachyStruct* idleDataHierachy;
  uint32_t step;

  if (ProgramManager_AutoPostRun_PostExec == (uint32_t)0U)
  {
    switch (ProgramManager_AutoPostRun_PrevState)
    {
      case PROGRAMMANAGER_STATE_AUTO_PRE_RUN:
      case PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT:
      case PROGRAMMANAGER_STATE_AUTO_RUN_WASH:
      {
        if (ProgramManager_AutoPostRun_DataCmd == PROGRAMMANAGER_CONTROL_COMMAND_STOP)
        {
          idleDataHierachy = (Fsm_DataHierachyStruct *)ProgramManager_malloc(sizeof(Fsm_DataHierachyStruct));
          idleDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_POST_RUN;

          ProgramManager_FsmContext.dataHierachy = idleDataHierachy;

          Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPOSTRUN_EVENT_IDLE);

          /* Reset initial step index */
          ProgramManager_gAutoSeqConfig.currentStep = ProgramManager_gInitStepIdx;
        }
        else if (ProgramManager_AutoPostRun_DataCmd == PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP)
        {
          retVal = ProgramManager_Control_GetNextStepAvailable(&step);

          if (retVal == true)
          {
            preRunDataHierachy = (ProgramManager_Control_PreRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PreRunStruct));
            preRunDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_POST_RUN;
            preRunDataHierachy->executeStep = step;

            ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)preRunDataHierachy;

            Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPOSTRUN_EVENT_PRE_RUN);
          }
        }
        else if (ProgramManager_AutoPostRun_DataCmd == PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP)
        {
          retVal = ProgramManager_Control_GetPrevStepAvailable(&step);

          if (retVal == true)
          {
            preRunDataHierachy = (ProgramManager_Control_PreRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PreRunStruct));
            preRunDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_POST_RUN;
            preRunDataHierachy->executeStep = step;

            ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)preRunDataHierachy;

            Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPOSTRUN_EVENT_PRE_RUN);
          }
        }
        else
        {
          /* nothing here */
        }

        break;
      }
      case PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN:
      {
        if (ProgramManager_AutoPostRun_DataCmd == PROGRAMMANAGER_CONTROL_COMMAND_STOP)
        {
          idleDataHierachy = (Fsm_DataHierachyStruct *)ProgramManager_malloc(sizeof(Fsm_DataHierachyStruct));
          idleDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_POST_RUN;

          ProgramManager_FsmContext.dataHierachy = idleDataHierachy;

          Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPOSTRUN_EVENT_IDLE);

          /* Reset initial step index */
          ProgramManager_gAutoSeqConfig.currentStep = ProgramManager_gInitStepIdx;
        }
        else if (ProgramManager_AutoPostRun_DataCmd == PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP)
        {
          retVal = ProgramManager_Control_GetNextStepAvailable(&step);

          if (retVal == true)
          {
            preRunDataHierachy = (ProgramManager_Control_PreRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PreRunStruct));
            preRunDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_POST_RUN;
            preRunDataHierachy->executeStep = step;

            ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)preRunDataHierachy;

            Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPOSTRUN_EVENT_PRE_RUN);
          }
        }
        else
        {
          /* nothing here */
        }

        break;
      }
      case PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT:
      {
        if (ProgramManager_AutoPostRun_DataCmd == PROGRAMMANAGER_CONTROL_COMMAND_STOP)
        {
          idleDataHierachy = (Fsm_DataHierachyStruct *)ProgramManager_malloc(sizeof(Fsm_DataHierachyStruct));
          idleDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_POST_RUN;

          ProgramManager_FsmContext.dataHierachy = idleDataHierachy;

          Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPOSTRUN_EVENT_IDLE);

          /* Reset initial step index */
          ProgramManager_gAutoSeqConfig.currentStep = ProgramManager_gInitStepIdx;
        }
        else
        {
          /* nothing here */
        }

        break;
      }
    }

    /* Reset global parameters */
    ProgramManager_gTimeCountMin = (uint32_t)0U;
    ProgramManager_gTimeCountSec = (uint32_t)0U;

    ProgramManager_AutoPostRun_PostExec = (uint32_t)1U;
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoPostRun_InternalControlOutput(void)
{
  ProgramManager_Control_ClearAllOutput();

  /* Control drain valve - always close */
  if (ProgramManager_gParamConfig.machineFuncCfg.drainValveStatus == PROGRAMMANAGER_RELAY_ENABLE_STAT_NO)
  {
    ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_DRAIN_VALVE_MASK);
  }
  else
  {
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_DRAIN_VALVE_MASK);
  }
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoPostRun_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Control_PostRunStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_PRE_RUN) || \
        (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT) || \
        (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_WASH) || \
        (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN) || \
        (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT))
    {
      enterDataHierachy = (ProgramManager_Control_PostRunStruct *)(pFsmContext->dataHierachy);

      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTOPOSTRUN_INTERNALDATALENGTH);

      ProgramManager_AutoPostRun_PrevState = enterDataHierachy->dataId;
      ProgramManager_AutoPostRun_DataCmd = enterDataHierachy->command;

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
    ProgramManager_AutoPostRun_PostExec = (uint32_t)0U;

    ProgramManager_SubMainFunctionPush(ProgramManager_AutoPostRun_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_AutoPostRun_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoPostRun_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoPostRun_SubMainFunction(void)
{
  ProgramManager_AutoPostRun_InternalCommandHandler();
  ProgramManager_AutoPostRun_InternalControlOutput();
}

/*=============================================================================================*/
static void ProgramManager_AutoPostRun_SubTickHandler(void)
{
  /* nothing here */
}



#ifdef  __cplusplus
}
#endif
