/* 
 * File:   _2000000000_program_ManualPreRun.c
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
#define PROGRAMMANAGER_MANUALPRERUN_INTERNALDATALENGTH                (uint8_t)3U

#define PROGRAMMANAGER_MANUALPRERUN_GLOBALCOUNTER_IDX                 0U
#define PROGRAMMANAGER_MANUALPRERUN_TEMPCOUNTER_IDX                   1U
#define PROGRAMMANAGER_MANUALPRERUN_PRESCOUNTER_IDX                   2U

#define ProgramManager_ManualPreRun_GlobalCounter                     ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALPRERUN_GLOBALCOUNTER_IDX)
#define ProgramManager_ManualPreRun_TempCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALPRERUN_TEMPCOUNTER_IDX)
#define ProgramManager_ManualPreRun_PresCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALPRERUN_PRESCOUNTER_IDX)

#define PROGRAMMANAGER_MANUALPRERUN_GLOBALCOUNTER_MAX                 (uint32_t)100U    /* 100 x 20ms = 2s */
#define PROGRAMMANAGER_MANUALPRERUN_TEMPCOUNTER_MAX                   (uint32_t)200U    /* 200 x 5ms = 1s */
#define PROGRAMMANAGER_MANUALPRERUN_PRESCOUNTER_MAX                   (uint32_t)200U    /* 200 x 5ms = 1s */

#define PROGRAMMANAGER_MANUALPRERUN_EVENT_RUN                         PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_MANUALPRERUN_EVENT_POST_RUN                    PROGRAMMANAGER_EVENT_SUBMENU_2



/** Program manager event handlers */
static Fsm_GuardType ProgramManager_ManualPreRun_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_ManualPreRun_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_ManualPreRun_StateMachine[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_ManualPreRun_Entry                                               ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_ManualPreRun_Exit                                                ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_MANUALPRERUN_EVENT_RUN,                                        PROGRAMMANAGER_STATE_MANUAL_RUN         ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_MANUALPRERUN_EVENT_POST_RUN,                                   PROGRAMMANAGER_STATE_MANUAL_POST_RUN    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_ManualPreRun_InternalCommandHandler(void);

static void ProgramManager_ManualPreRun_InternalCheckLevelCondition(void);
static void ProgramManager_ManualPreRun_InternalCheckTempCondition(void);

static void ProgramManager_ManualPreRun_InternalCheckStateTransit(void);
static void ProgramManager_ManualPreRun_InternalControlOutput(void);

static void ProgramManager_ManualPreRun_SubMainFunction(void);
static void ProgramManager_ManualPreRun_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_ManualPreRun_InternalCommandHandler(void)
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

      ProgramManager_ManualPreRun_GlobalCounter = (uint32_t)0U;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_STOP:
    {
      dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_MANUAL_PRE_RUN;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_MANUALPRERUN_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_WASH:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_COLDWATER:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_HOTWATER:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_HEAT:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_LEVEL:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_SUPPLY1:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_SUPPLY2:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_SUPPLY3:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_DRAIN:
    case PROGRAMMANAGER_CONTROL_COMMAND_MANUAL_EXTRACT:
    {
      /* Store which button is pressed when change to manual run state */
      ProgramManager_Control_ToggleManualOption(command);

      break;
    }
    default:
      break;
  }

  return stateTransit;
}

/*=============================================================================================*/
static void ProgramManager_ManualPreRun_InternalCheckLevelCondition(void)
{
  /* Check pressure */
  if (ProgramManager_gCurrentPressure >= ProgramManager_gCurrentPresThreshold)
  {
    if (ProgramManager_ManualPreRun_PresCounter == PROGRAMMANAGER_MANUALPRERUN_PRESCOUNTER_MAX)
    {
      ProgramManager_gPresThresExceeded = (bool)true;
    }
    else
    {
      ProgramManager_ManualPreRun_PresCounter += (uint32_t)1U;
    }
  }
  else
  {
    ProgramManager_ManualPreRun_PresCounter = (uint32_t)0U;
    ProgramManager_gPresThresExceeded = (bool)false;
  }
}

/*=============================================================================================*/
static void ProgramManager_ManualPreRun_InternalCheckTempCondition(void)
{
  /* Check temperature */
  if (ProgramManager_gCurrentTemperature >= ProgramManager_gCurrentTempThreshold)
  {
    if (ProgramManager_ManualPreRun_TempCounter == PROGRAMMANAGER_MANUALPRERUN_TEMPCOUNTER_MAX)
    {
      ProgramManager_gTempThresExceeded = (bool)true;
    }
    else
    {
      ProgramManager_ManualPreRun_TempCounter += (uint32_t)1U;
    }
  }
  else
  {
    ProgramManager_ManualPreRun_TempCounter = (uint32_t)0U;
    ProgramManager_gTempThresExceeded = (bool)false;
  }
}



/*=============================================================================================*/
static void ProgramManager_ManualPreRun_InternalCheckStateTransit(void)
{
  Fsm_DataHierachyStruct *dataHierachy;

  if (ProgramManager_Control_NotPaused())
  {
    if (ProgramManager_ManualPreRun_GlobalCounter >= PROGRAMMANAGER_MANUALPRERUN_GLOBALCOUNTER_MAX)
    {
      dataHierachy = (Fsm_DataHierachyStruct *)ProgramManager_malloc(sizeof(Fsm_DataHierachyStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_MANUAL_PRE_RUN;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;
      
      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_MANUALPRERUN_EVENT_RUN);

      ProgramManager_ManualPreRun_TempCounter = (uint32_t)0U;
      ProgramManager_ManualPreRun_PresCounter = (uint32_t)0U;
      ProgramManager_ManualPreRun_GlobalCounter = (uint32_t)0U;
    }
    else
    {
      /* Just wait */
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_ManualPreRun_InternalControlOutput(void)
{
  ProgramManager_Control_ClearAllOutputExceptDrain();
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_ManualPreRun_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_IDLE)
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
    ProgramManager_InternalDataPush(PROGRAMMANAGER_MANUALPRERUN_INTERNALDATALENGTH);

    ProgramManager_ManualPreRun_GlobalCounter = (uint32_t)0U;
    ProgramManager_ManualPreRun_TempCounter = (uint32_t)0U;
    ProgramManager_ManualPreRun_PresCounter = (uint32_t)0U;

    /* Get default manual sequence parameters */
    ProgramManager_ManualSeqConfig_GetData(&ProgramManager_gManualSeqConfig);

    ProgramManager_SubMainFunctionPush(ProgramManager_ManualPreRun_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_ManualPreRun_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_ManualPreRun_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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
static void ProgramManager_ManualPreRun_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_ManualPreRun_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_ManualPreRun_InternalCheckLevelCondition();
    ProgramManager_ManualPreRun_InternalCheckTempCondition();
    
    ProgramManager_ManualPreRun_InternalCheckStateTransit();
    ProgramManager_ManualPreRun_InternalControlOutput();
  }
}

/*=============================================================================================*/
static void ProgramManager_ManualPreRun_SubTickHandler(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    ProgramManager_ManualPreRun_GlobalCounter += (uint32_t)1U;
  }
}



#ifdef  __cplusplus
}
#endif
