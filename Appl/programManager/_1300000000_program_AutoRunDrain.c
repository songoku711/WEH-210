/* 
 * File:   _1300000000_program_AutoRunDrain.c
 * Author: Long
 * 
 * In RUN_WATER_HEAT state, 
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

#define PROGRAMMANAGER_AUTORUNDRAIN_INTERNALDATALENGTH                (uint8_t)4U

#define PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_IDX              0U
#define PROGRAMMANAGER_AUTORUNDRAIN_LEVELLOWREACHED_IDX               1U
#define PROGRAMMANAGER_AUTORUNDRAIN_PRESCOUNTER_IDX                   2U
#define PROGRAMMANAGER_AUTORUNDRAIN_TIMEOUTCOUNTER_IDX                3U

#define ProgramManager_AutoRunDrain_OneSecondElapsed                  ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_IDX)
#define ProgramManager_AutoRunDrain_LevelLowReached                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_LEVELLOWREACHED_IDX)
#define ProgramManager_AutoRunDrain_PresCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_PRESCOUNTER_IDX)
#define ProgramManager_AutoRunDrain_TimeOutCounter                    ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_TIMEOUTCOUNTER_IDX)

#define PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_MAX              (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_AUTORUNDRAIN_PRESCOUNTER_MAX                   (uint32_t)10U /* 10s */
#define PROGRAMMANAGER_AUTORUNDRAIN_TIMEOUTCOUNTER_MAX                (uint32_t)600U /* 600s = 10m */

#define PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN                    PROGRAMMANAGER_EVENT_SUBMENU_1

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoRunDrain_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoRunDrain_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoRunDrain_StateMachine[3] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoRunDrain_Entry                                               ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoRunDrain_Exit                                                ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN,                                   PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoRunDrain_InternalCommandHandler(void);

static void ProgramManager_AutoRunDrain_InternalCheckLevelCondition(void);

static void ProgramManager_AutoRunDrain_InternalCheckStateTransit(void);
static void ProgramManager_AutoRunDrain_InternalControlOutput(void);

static void ProgramManager_AutoRunDrain_InternalUpdateLcdParams(void);

static void ProgramManager_AutoRunDrain_SubMainFunction(void);
static void ProgramManager_AutoRunDrain_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_AutoRunDrain_InternalCommandHandler(void)
{
  uint8_t command = PROGRAMMANAGER_CONTROL_COMMAND_NONE;
  ProgramManager_Control_PostRunStruct *dataHierachy;
  bool stateTransit = false;

  ProgramManager_Control_RetrieveCommand(&command);

  /* Check command */
  switch (command)
  {
    case PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME:
    {
      ProgramManager_Control_TogglePauseResumeHandler();

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_STOP:
    {
      dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
#if 0
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP:
    {
      stateTransit = ProgramManager_Control_CheckNextStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {
      stateTransit = ProgramManager_Control_CheckPrevStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN);
      }

      break;
    }
#endif
    default:
      break;
  }

  return stateTransit;
}

/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_InternalCheckLevelCondition(void)
{
  /* Check pressure */
  if (ProgramManager_gCurrentPressure > (uint32_t)(ProgramManager_gParamConfig.fillLevelCfg.zeroLevel))
  {
    ProgramManager_AutoRunDrain_LevelLowReached = (uint32_t)0U;
    ProgramManager_AutoRunDrain_PresCounter = (uint32_t)0U;
  }
  else
  {
    ProgramManager_AutoRunDrain_LevelLowReached = (uint32_t)1U;
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_InternalCheckStateTransit(void)
{
  bool conditionOk = (bool)true;
  ProgramManager_Control_PostRunStruct *dataHierachy;

  if (ProgramManager_Control_NotPauseAndError())
  {
    /* Level low reached or timeout expired */
    if ((ProgramManager_AutoRunDrain_PresCounter < PROGRAMMANAGER_AUTORUNDRAIN_PRESCOUNTER_MAX) && \
        (ProgramManager_AutoRunDrain_TimeOutCounter < PROGRAMMANAGER_AUTORUNDRAIN_TIMEOUTCOUNTER_MAX))
    {
      conditionOk = (bool)false;
    }

    if (conditionOk == (bool)true)
    {
      dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN);
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_InternalControlOutput(void)
{
  /* Turn off heat generator */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);

  /* Turn off water */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_WATER_MASK);

  /* Turn off all soaps */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);

  /* Turn off motor */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK);

  /* Control drain valve - always open */
  if (ProgramManager_gParamConfig.machineFuncCfg.drainValveStatus == PROGRAMMANAGER_RELAY_ENABLE_STAT_NO)
  {
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_DRAIN_VALVE_MASK);
  }
  else
  {
    ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_DRAIN_VALVE_MASK);
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_InternalUpdateLcdParams(void)
{
  ProgramManager_gTimeCountMin = (uint32_t)0U;
  ProgramManager_gTimeCountSec = (uint32_t)0U;
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunDrain_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_WASH)
    {
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
    ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNDRAIN_INTERNALDATALENGTH);

    ProgramManager_AutoRunDrain_OneSecondElapsed = (uint32_t)0U;
    ProgramManager_AutoRunDrain_LevelLowReached = (uint32_t)0U;
    ProgramManager_AutoRunDrain_PresCounter = (uint32_t)0U;
    ProgramManager_AutoRunDrain_TimeOutCounter = (uint32_t)0U;

    ProgramManager_SubMainFunctionPush(ProgramManager_AutoRunDrain_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_AutoRunDrain_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunDrain_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoRunDrain_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoRunDrain_InternalCheckLevelCondition();

    ProgramManager_AutoRunDrain_InternalControlOutput();

    ProgramManager_AutoRunDrain_InternalUpdateLcdParams();

    ProgramManager_AutoRunDrain_InternalCheckStateTransit();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_SubTickHandler(void)
{
  ProgramManager_AutoRunDrain_OneSecondElapsed += (uint32_t)1U;

  if (ProgramManager_AutoRunDrain_OneSecondElapsed >= PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_MAX)
  {
    ProgramManager_AutoRunDrain_OneSecondElapsed = (uint32_t)0U;

    if (ProgramManager_AutoRunDrain_LevelLowReached != (uint32_t)0U)
    {
      ProgramManager_AutoRunDrain_PresCounter += (uint32_t)1U;
    }

    ProgramManager_AutoRunDrain_TimeOutCounter += (uint32_t)1U;
  }
}



#ifdef  __cplusplus
}
#endif
