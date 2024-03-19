/* 
 * File:   _1200000000_program_AutoRunHeat.c
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

#define PROGRAMMANAGER_AUTORUNHEAT_INTERNALDATALENGTH                 (uint8_t)7U

#define PROGRAMMANAGER_AUTORUNHEAT_ONESECONDELAPSED_IDX               0U
#define PROGRAMMANAGER_AUTORUNHEAT_TEMPCOUNTER_IDX                    1U
#define PROGRAMMANAGER_AUTORUNHEAT_PRESCOUNTER_IDX                    2U
#define PROGRAMMANAGER_AUTORUNHEAT_TEMPTIMEOUT_IDX                    3U
#define PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_IDX                     4U
#define PROGRAMMANAGER_AUTORUNHEAT_MOTORCOUNTER_IDX                   5U
#define PROGRAMMANAGER_AUTORUNHEAT_MOTORCOUNTERMAX_IDX                6U

#define ProgramManager_AutoRunHeat_OneSecondElapsed                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNHEAT_ONESECONDELAPSED_IDX)
#define ProgramManager_AutoRunHeat_TempCounter                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNHEAT_TEMPCOUNTER_IDX)
#define ProgramManager_AutoRunHeat_PresCounter                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNHEAT_PRESCOUNTER_IDX)
#define ProgramManager_AutoRunHeat_TempTimeout                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNHEAT_TEMPTIMEOUT_IDX)
#define ProgramManager_AutoRunHeat_MotorState                         ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_IDX)
#define ProgramManager_AutoRunHeat_MotorCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNHEAT_MOTORCOUNTER_IDX)
#define ProgramManager_AutoRunHeat_MotorCounterMax                    ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNHEAT_MOTORCOUNTERMAX_IDX)

#define PROGRAMMANAGER_AUTORUNHEAT_ONESECONDELAPSED_MAX               (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_AUTORUNHEAT_EVENT_RUN_WATER                    PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTORUNHEAT_EVENT_RUN_WASH                     PROGRAMMANAGER_EVENT_SUBMENU_2
#define PROGRAMMANAGER_AUTORUNHEAT_EVENT_POST_RUN                     PROGRAMMANAGER_EVENT_SUBMENU_3

#define PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_FWD                     0U
#define PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_STOP1                   1U
#define PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_REV                     2U
#define PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_STOP2                   3U
#define PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_MAX                     4U

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoRunHeat_Entry                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoRunHeat_Exit                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoRunHeat_StateMachine[5] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoRunHeat_Entry                                           ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoRunHeat_Exit                                            ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNHEAT_EVENT_RUN_WATER,                              PROGRAMMANAGER_STATE_AUTO_RUN_WATER     ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNHEAT_EVENT_RUN_WASH,                               PROGRAMMANAGER_STATE_AUTO_RUN_WASH      ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNHEAT_EVENT_POST_RUN,                               PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoRunHeat_InternalCommandHandler(void);

static void ProgramManager_AutoRunHeat_InternalCheckLevelCondition(void);
static void ProgramManager_AutoRunHeat_InternalCheckTempCondition(void);
static void ProgramManager_AutoRunHeat_InternalCheckMotorCondition(void);

static void ProgramManager_AutoRunHeat_InternalCheckStateTransit(void);
static void ProgramManager_AutoRunHeat_InternalControlOutput(void);

static void ProgramManager_AutoRunHeat_InternalUpdateLcdParams(void);

static void ProgramManager_AutoRunHeat_SubMainFunction(void);
static void ProgramManager_AutoRunHeat_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_AutoRunHeat_InternalCommandHandler(void)
{
  uint8_t command = PROGRAMMANAGER_CONTROL_COMMAND_NONE;
  ProgramManager_Control_PostRunStruct *dataHierachy;
  ProgramManager_Control_RunStruct *runDataHierachy;
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
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_HEAT;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNHEAT_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP:
    {
      stateTransit = ProgramManager_Control_CheckNextStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_HEAT;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNHEAT_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {
      stateTransit = ProgramManager_Control_CheckPrevStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_HEAT;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNHEAT_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_SUBSTEP:
    {
      runDataHierachy = (ProgramManager_Control_RunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_RunStruct));
      runDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_HEAT;

      runDataHierachy->oneSecondElapsed  = ProgramManager_AutoRunHeat_OneSecondElapsed;
      runDataHierachy->tempCounter       = ProgramManager_AutoRunHeat_TempCounter;
      runDataHierachy->presCounter       = ProgramManager_AutoRunHeat_PresCounter;
      runDataHierachy->motorState        = ProgramManager_AutoRunHeat_MotorState;
      runDataHierachy->motorCounter      = ProgramManager_AutoRunHeat_MotorCounter;
      runDataHierachy->motorCounterMax   = ProgramManager_AutoRunHeat_MotorCounterMax;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)runDataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNHEAT_EVENT_RUN_WASH);

      stateTransit = true;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_SUBSTEP:
    {
      if (ProgramManager_gPresThresExceeded == (bool)false)
      {
        runDataHierachy = (ProgramManager_Control_RunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_RunStruct));
        runDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_HEAT;

        runDataHierachy->oneSecondElapsed  = ProgramManager_AutoRunHeat_OneSecondElapsed;
        runDataHierachy->tempCounter       = ProgramManager_AutoRunHeat_TempCounter;
        runDataHierachy->presCounter       = ProgramManager_AutoRunHeat_PresCounter;
        runDataHierachy->motorState        = ProgramManager_AutoRunHeat_MotorState;
        runDataHierachy->motorCounter      = ProgramManager_AutoRunHeat_MotorCounter;
        runDataHierachy->motorCounterMax   = ProgramManager_AutoRunHeat_MotorCounterMax;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)runDataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNHEAT_EVENT_RUN_WATER);

        stateTransit = true;
      }

      break;
    }
    default:
      break;
  }

  return stateTransit;
}

/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_InternalCheckLevelCondition(void)
{
  if (ProgramManager_gPresThresExceeded == (bool)false)
  {
    /* Check pressure */
    if (ProgramManager_gCurrentPressure >= ProgramManager_gCurrentPresThreshold)
    {
      if (ProgramManager_AutoRunHeat_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)true;

        ProgramManager_AutoRunHeat_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunHeat_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunHeat_PresCounter = (uint32_t)0U;
    }
  }
  else
  {
    /* Auto-refill when low level */
    /* Check pressure with delta offset */
    if (ProgramManager_gCurrentPressure <= (ProgramManager_gCurrentPresThreshold - ProgramManager_gParamConfig.fillLevelCfg.levelDiffRefill))
    {
      if (ProgramManager_AutoRunHeat_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)false;

        ProgramManager_AutoRunHeat_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunHeat_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunHeat_PresCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_InternalCheckTempCondition(void)
{
  if (ProgramManager_gTempThresExceeded == (bool)false)
  {
    /* Check temperature */
    if (ProgramManager_gCurrentTemperature >= ProgramManager_gCurrentTempThreshold)
    {
      if (ProgramManager_AutoRunHeat_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)true;

        ProgramManager_AutoRunHeat_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunHeat_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunHeat_TempCounter = (uint32_t)0U;
    }

    if (ProgramManager_AutoRunHeat_TempTimeout != (uint32_t)0U)
    {
      ProgramManager_AutoRunHeat_TempTimeout -= (uint32_t)1U;
    }
  }
  else
  {
    /* Check temperature with delta offset */
    if (ProgramManager_gCurrentTemperature <= (ProgramManager_gCurrentTempThreshold - ProgramManager_gParamConfig.heatTempCfg.tempDiffReheat))
    {
      if (ProgramManager_AutoRunHeat_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)false;

        ProgramManager_AutoRunHeat_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunHeat_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunHeat_TempCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_InternalCheckMotorCondition(void)
{
  if (ProgramManager_AutoRunHeat_MotorCounter >= ProgramManager_AutoRunHeat_MotorCounterMax)
  {
    ProgramManager_AutoRunHeat_MotorCounter = (uint32_t)0U;

    ProgramManager_AutoRunHeat_MotorState += (uint32_t)1U;

    if (ProgramManager_AutoRunHeat_MotorState >= PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_MAX)
    {
      ProgramManager_AutoRunHeat_MotorState = PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_FWD;
    }

    switch (ProgramManager_AutoRunHeat_MotorState)
    {
      case PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_FWD:
      case PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_REV:
      {
        ProgramManager_AutoRunHeat_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

        break;
      }
      case PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_STOP1:
      case PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_STOP2:
      default:
      {
        ProgramManager_AutoRunHeat_MotorCounterMax = ProgramManager_gCurrentWashStopTime;

        break;
      }
    }
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_InternalCheckStateTransit(void)
{
  bool conditionOk = (bool)true;
  ProgramManager_Control_RunStruct *dataHierachy;

  if (ProgramManager_Control_NotPaused())
  {
    /* Temperature threshold not reach, and timeout not elapse if configured */
    if (ProgramManager_gTempThresExceeded == (bool)false)
    {
      if (ProgramManager_AutoRunHeat_TempTimeout != (uint32_t)0U)
      {
        conditionOk = (bool)false;
      }
    }

    /* Disable temporarily check pressure threshold to jump into next state */
#if 0
    /* Pressure threshold not reach */
    if (ProgramManager_gPresThresExceeded == (bool)false)
    {
      conditionOk = (bool)false;
    }
#endif

    if (conditionOk == (bool)true)
    {
      dataHierachy = (ProgramManager_Control_RunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_RunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_HEAT;

      dataHierachy->oneSecondElapsed  = ProgramManager_AutoRunHeat_OneSecondElapsed;
      dataHierachy->tempCounter       = ProgramManager_AutoRunHeat_TempCounter;
      dataHierachy->presCounter       = ProgramManager_AutoRunHeat_PresCounter;
      dataHierachy->motorState        = ProgramManager_AutoRunHeat_MotorState;
      dataHierachy->motorCounter      = ProgramManager_AutoRunHeat_MotorCounter;
      dataHierachy->motorCounterMax   = ProgramManager_AutoRunHeat_MotorCounterMax;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNHEAT_EVENT_RUN_WASH);
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_InternalControlOutput(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    /* Control heat generator through temperature */
    if (ProgramManager_gTempThresExceeded == (bool)false)
    {
      ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);
    }
    else
    {
      ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);
    }

    /* Control water through pressure */
    if (ProgramManager_gPresThresExceeded == (bool)false)
    {
      ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_WATER_MASK, \
                                          (uint16_t)((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].waterMode) << PROGRAMMANAGER_CONTROL_OUTPUT_WATER_OFFSET);
    }
    else
    {
      ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_WATER_MASK);
    }

    /* Control soap - always off */
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_MASK);

    /* Control motor */
    if (ProgramManager_AutoRunHeat_MotorState == PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_FWD)
    {
      ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK);
    }
    else if (ProgramManager_AutoRunHeat_MotorState == PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_REV)
    {
      ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_REV_MASK);
    }
    else
    {
      ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK);
    }

    ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK, \
                                        ProgramManager_gCurrentWashSpeed << PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_OFFSET);
  }
  else
  {
    ProgramManager_Control_ClearAllOutput();
  }

  /* Control drain valve - always CLOSE */
  ProgramManager_Control_DrainCloseHandler();
}

/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_InternalUpdateLcdParams(void)
{
  uint32_t timeTemp;

  timeTemp = ProgramManager_AutoRunHeat_TempTimeout;

  /* Time elapsed in second = time * (5 ticks per period) / 1000 ticks */
  timeTemp = timeTemp / (uint32_t)200U;

  ProgramManager_gTimeCountMin = timeTemp / (uint32_t)60U;
  ProgramManager_gTimeCountSec = timeTemp % (uint32_t)60U;
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunHeat_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Control_RunStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_PRE_RUN)
    {
      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNHEAT_INTERNALDATALENGTH);

      ProgramManager_AutoRunHeat_OneSecondElapsed = (uint32_t)0U;

      ProgramManager_AutoRunHeat_TempCounter = (uint32_t)0U;
      ProgramManager_AutoRunHeat_PresCounter = (uint32_t)0U;

      ProgramManager_AutoRunHeat_MotorState = PROGRAMMANAGER_AUTORUNHEAT_MOTORSTATE_FWD;
      ProgramManager_AutoRunHeat_MotorCounter = (uint32_t)0U;
      ProgramManager_AutoRunHeat_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_WATER)
    {
      enterDataHierachy = (ProgramManager_Control_RunStruct *)(pFsmContext->dataHierachy);

      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNHEAT_INTERNALDATALENGTH);

      ProgramManager_AutoRunHeat_OneSecondElapsed = enterDataHierachy->oneSecondElapsed;

      ProgramManager_AutoRunHeat_TempCounter = enterDataHierachy->tempCounter;
      ProgramManager_AutoRunHeat_PresCounter = enterDataHierachy->presCounter;

      ProgramManager_AutoRunHeat_MotorState = enterDataHierachy->motorState;
      ProgramManager_AutoRunHeat_MotorCounter = enterDataHierachy->motorCounter;
      ProgramManager_AutoRunHeat_MotorCounterMax = enterDataHierachy->motorCounterMax;

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
    /* Timeout in minutes * 60s * 1000 ticks / (5 ticks per period) */
    ProgramManager_AutoRunHeat_TempTimeout = (uint32_t)(ProgramManager_gParamConfig.heatTempCfg.maxTimeHeat) * (uint32_t)12000U;

    ProgramManager_SubMainFunctionPush(ProgramManager_AutoRunHeat_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_AutoRunHeat_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunHeat_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoRunHeat_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoRunHeat_InternalCheckLevelCondition();
    ProgramManager_AutoRunHeat_InternalCheckTempCondition();
    ProgramManager_AutoRunHeat_InternalCheckMotorCondition();

    ProgramManager_AutoRunHeat_InternalControlOutput();

    ProgramManager_AutoRunHeat_InternalUpdateLcdParams();

    ProgramManager_AutoRunHeat_InternalCheckStateTransit();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunHeat_SubTickHandler(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    ProgramManager_AutoRunHeat_OneSecondElapsed += (uint32_t)1U;

    if (ProgramManager_AutoRunHeat_OneSecondElapsed >= PROGRAMMANAGER_AUTORUNHEAT_ONESECONDELAPSED_MAX)
    {
      ProgramManager_AutoRunHeat_OneSecondElapsed = (uint32_t)0U;

      ProgramManager_AutoRunHeat_MotorCounter += (uint32_t)1U;
    }
  }
  else
  {
    ProgramManager_AutoRunHeat_OneSecondElapsed = (uint32_t)0U;
  }
}



#ifdef  __cplusplus
}
#endif
