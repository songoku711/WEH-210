/* 
 * File:   _1000000000_program_AutoPreRun.c
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
#define PROGRAMMANAGER_AUTOPRERUN_INTERNALDATALENGTH                  (uint8_t)3U

#define PROGRAMMANAGER_AUTOPRERUN_GLOBALCOUNTER_IDX                   0U
#define PROGRAMMANAGER_AUTOPRERUN_TEMPCOUNTER_IDX                     1U
#define PROGRAMMANAGER_AUTOPRERUN_PRESCOUNTER_IDX                     2U

#define ProgramManager_AutoPreRun_GlobalCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTOPRERUN_GLOBALCOUNTER_IDX)
#define ProgramManager_AutoPreRun_TempCounter                         ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTOPRERUN_TEMPCOUNTER_IDX)
#define ProgramManager_AutoPreRun_PresCounter                         ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTOPRERUN_PRESCOUNTER_IDX)

#define PROGRAMMANAGER_AUTOPRERUN_GLOBALCOUNTER_MAX                   (uint32_t)100U    /* 100 x 20ms = 2s */
#define PROGRAMMANAGER_AUTOPRERUN_TEMPCOUNTER_MAX                     (uint32_t)200U    /* 200 x 5ms = 1s */
#define PROGRAMMANAGER_AUTOPRERUN_PRESCOUNTER_MAX                     (uint32_t)200U    /* 200 x 5ms = 1s */

#define PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_WATER_HEAT                PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_WASH                      PROGRAMMANAGER_EVENT_SUBMENU_2
#define PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_EXTRACT                   PROGRAMMANAGER_EVENT_SUBMENU_3
#define PROGRAMMANAGER_AUTOPRERUN_EVENT_POST_RUN                      PROGRAMMANAGER_EVENT_SUBMENU_4



/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoPreRun_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoPreRun_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoPreRun_StateMachine[6] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoPreRun_Entry                                                 ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoPreRun_Exit                                                  ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_WATER_HEAT,                               PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_WASH,                                     PROGRAMMANAGER_STATE_AUTO_RUN_WASH      ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_EXTRACT,                                  PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT   ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTOPRERUN_EVENT_POST_RUN,                                     PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoPreRun_InternalCommandHandler(void);

static void ProgramManager_AutoPreRun_InternalCheckLevelCondition(void);
static void ProgramManager_AutoPreRun_InternalCheckTempCondition(void);

static void ProgramManager_AutoPreRun_InternalCheckStateTransit(void);
static void ProgramManager_AutoPreRun_InternalControlOutput(void);

static void ProgramManager_AutoPreRun_SubMainFunction(void);
static void ProgramManager_AutoPreRun_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_AutoPreRun_InternalCommandHandler(void)
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

      ProgramManager_AutoPreRun_GlobalCounter = (uint32_t)0U;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_STOP:
    {
      dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_PRE_RUN;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPRERUN_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP:
    {
      stateTransit = ProgramManager_Control_CheckNextStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_PRE_RUN;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPRERUN_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {
      stateTransit = ProgramManager_Control_CheckPrevStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_PRE_RUN;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPRERUN_EVENT_POST_RUN);
      }

      break;
    }
    default:
      break;
  }

  return stateTransit;
}

/*=============================================================================================*/
static void ProgramManager_AutoPreRun_InternalCheckLevelCondition(void)
{
  /* Check pressure */
  if (ProgramManager_gCurrentPressure >= ProgramManager_gCurrentPresThreshold)
  {
    if (ProgramManager_AutoPreRun_PresCounter == PROGRAMMANAGER_AUTOPRERUN_PRESCOUNTER_MAX)
    {
      ProgramManager_gPresThresExceeded = (bool)true;
    }
    else
    {
      ProgramManager_AutoPreRun_PresCounter += (uint32_t)1U;
    }
  }
  else
  {
    ProgramManager_AutoPreRun_PresCounter = (uint32_t)0U;
    ProgramManager_gPresThresExceeded = (bool)false;
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoPreRun_InternalCheckTempCondition(void)
{
  /* Check temperature */
  if (ProgramManager_gCurrentTemperature >= ProgramManager_gCurrentTempThreshold)
  {
    if (ProgramManager_AutoPreRun_TempCounter == PROGRAMMANAGER_AUTOPRERUN_TEMPCOUNTER_MAX)
    {
      ProgramManager_gTempThresExceeded = (bool)true;
    }
    else
    {
      ProgramManager_AutoPreRun_TempCounter += (uint32_t)1U;
    }
  }
  else
  {
    ProgramManager_AutoPreRun_TempCounter = (uint32_t)0U;
    ProgramManager_gTempThresExceeded = (bool)false;
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoPreRun_InternalCheckStateTransit(void)
{
  if (ProgramManager_Control_NotPauseAndError())
  {
    if ((ProgramManager_AutoPreRun_TempCounter >= PROGRAMMANAGER_AUTOPRERUN_TEMPCOUNTER_MAX) || \
        (ProgramManager_AutoPreRun_PresCounter >= PROGRAMMANAGER_AUTOPRERUN_PRESCOUNTER_MAX))
    {
      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_WASH);

      ProgramManager_AutoPreRun_TempCounter = (uint32_t)0U;
      ProgramManager_AutoPreRun_PresCounter = (uint32_t)0U;
      ProgramManager_AutoPreRun_GlobalCounter = (uint32_t)0U;
    }
    else if (ProgramManager_AutoPreRun_GlobalCounter >= PROGRAMMANAGER_AUTOPRERUN_GLOBALCOUNTER_MAX)
    {
      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_WATER_HEAT);

      ProgramManager_AutoPreRun_TempCounter = (uint32_t)0U;
      ProgramManager_AutoPreRun_PresCounter = (uint32_t)0U;
      ProgramManager_AutoPreRun_GlobalCounter = (uint32_t)0U;
    }
    else
    {
      /* Just wait */
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoPreRun_InternalControlOutput(void)
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
static Fsm_GuardType ProgramManager_AutoPreRun_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Control_PreRunStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_IDLE) || \
        (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_POST_RUN))
    {
      enterDataHierachy = (ProgramManager_Control_PreRunStruct *)(pFsmContext->dataHierachy);

      ProgramManager_gAutoSeqConfig.currentStep = enterDataHierachy->executeStep;

      /* Execute step is extract step */
      if (ProgramManager_gAutoSeqConfig.currentStep == PROGRAMMANAGER_STEP_NUM_MAX)
      {
        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTOPRERUN_EVENT_RUN_EXTRACT);
      }

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
    /* Extract step will jump to RUN_EXTRACT state immediately */
    if (ProgramManager_gAutoSeqConfig.currentStep != PROGRAMMANAGER_STEP_NUM_MAX)
    {
      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTOPRERUN_INTERNALDATALENGTH);

      ProgramManager_AutoPreRun_GlobalCounter = (uint32_t)0U;
      ProgramManager_AutoPreRun_TempCounter = (uint32_t)0U;
      ProgramManager_AutoPreRun_PresCounter = (uint32_t)0U;

      ProgramManager_SubMainFunctionPush(ProgramManager_AutoPreRun_SubMainFunction);
      ProgramManager_SubTickHandler = ProgramManager_AutoPreRun_SubTickHandler;
    }

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoPreRun_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_gAutoSeqConfig.currentStep != PROGRAMMANAGER_STEP_NUM_MAX)
  {
    ProgramManager_SubMainFunctionPop();
    ProgramManager_SubTickHandler = NULL;

    /* Free internal data */
    ProgramManager_InternalDataPop();
  }

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoPreRun_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoPreRun_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoPreRun_InternalCheckLevelCondition();
    ProgramManager_AutoPreRun_InternalCheckTempCondition();
    
    ProgramManager_AutoPreRun_InternalCheckStateTransit();
    ProgramManager_AutoPreRun_InternalControlOutput();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoPreRun_SubTickHandler(void)
{
  if (ProgramManager_Control_NotPauseAndError())
  {
    ProgramManager_AutoPreRun_GlobalCounter += (uint32_t)1U;
  }
}



#ifdef  __cplusplus
}
#endif
