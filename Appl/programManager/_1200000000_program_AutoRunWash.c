/* 
 * File:   _1200000000_program_AutoRunWash.c
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

#define PROGRAMMANAGER_AUTORUNWASH_INTERNALDATALENGTH                 (uint8_t)7U

#define PROGRAMMANAGER_AUTORUNWASH_ONESECONDELAPSED_IDX               0U
#define PROGRAMMANAGER_AUTORUNWASH_TEMPCOUNTER_IDX                    1U
#define PROGRAMMANAGER_AUTORUNWASH_PRESCOUNTER_IDX                    2U
#define PROGRAMMANAGER_AUTORUNWASH_WASHCOUNT_IDX                      3U
#define PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_IDX                     4U
#define PROGRAMMANAGER_AUTORUNWASH_MOTORCOUNTER_IDX                   5U
#define PROGRAMMANAGER_AUTORUNWASH_MOTORCOUNTERMAX_IDX                6U

#define ProgramManager_AutoRunWash_OneSecondElapsed                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWASH_ONESECONDELAPSED_IDX)
#define ProgramManager_AutoRunWash_TempCounter                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWASH_TEMPCOUNTER_IDX)
#define ProgramManager_AutoRunWash_PresCounter                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWASH_PRESCOUNTER_IDX)
#define ProgramManager_AutoRunWash_WashCount                          ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWASH_WASHCOUNT_IDX)
#define ProgramManager_AutoRunWash_MotorState                         ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_IDX)
#define ProgramManager_AutoRunWash_MotorCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWASH_MOTORCOUNTER_IDX)
#define ProgramManager_AutoRunWash_MotorCounterMax                    ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWASH_MOTORCOUNTERMAX_IDX)

#define PROGRAMMANAGER_AUTORUNWASH_ONESECONDELAPSED_MAX               (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_AUTORUNWASH_EVENT_RUN_DRAIN                    PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTORUNWASH_EVENT_POST_RUN                     PROGRAMMANAGER_EVENT_SUBMENU_2

#define PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_FWD                     0U
#define PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_STOP1                   1U
#define PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_REV                     2U
#define PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_STOP2                   3U
#define PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_MAX                     4U

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoRunWash_Entry                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoRunWash_Exit                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoRunWash_StateMachine[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoRunWash_Entry                                                ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoRunWash_Exit                                                 ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNWASH_EVENT_RUN_DRAIN,                                   PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN     ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNWASH_EVENT_POST_RUN,                                    PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoRunWash_InternalCommandHandler(void);

static void ProgramManager_AutoRunWash_InternalCheckLevelCondition(void);
static void ProgramManager_AutoRunWash_InternalCheckTempCondition(void);
static void ProgramManager_AutoRunWash_InternalCheckMotorCondition(void);

static void ProgramManager_AutoRunWash_InternalCheckStateTransit(void);
static void ProgramManager_AutoRunWash_InternalControlOutput(void);

static void ProgramManager_AutoRunWash_InternalUpdateLcdParams(void);

static void ProgramManager_AutoRunWash_SubMainFunction(void);
static void ProgramManager_AutoRunWash_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_AutoRunWash_InternalCommandHandler(void)
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
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WASH;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWASH_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP:
    {
      stateTransit = ProgramManager_Control_CheckNextStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WASH;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWASH_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {
      stateTransit = ProgramManager_Control_CheckPrevStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WASH;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWASH_EVENT_POST_RUN);
      }

      break;
    }
    default:
      break;
  }

  return stateTransit;
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWash_InternalCheckLevelCondition(void)
{
  if (ProgramManager_gPresThresExceeded == (bool)false)
  {
    /* Check pressure */
    if (ProgramManager_gCurrentPressure >= ProgramManager_gCurrentPresThreshold)
    {
      if (ProgramManager_AutoRunWash_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)true;

        ProgramManager_AutoRunWash_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWash_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWash_PresCounter = (uint32_t)0U;
    }
  }
  else
  {
    /* Auto-refill when low level */
    /* Check pressure with delta offset */
    if (ProgramManager_gCurrentPressure <= (ProgramManager_gCurrentPresThreshold - ProgramManager_gParamConfig.fillLevelCfg.levelDiffRefill))
    {
      if (ProgramManager_AutoRunWash_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)false;

        ProgramManager_AutoRunWash_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWash_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWash_PresCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWash_InternalCheckTempCondition(void)
{
  if (ProgramManager_gTempThresExceeded == (bool)false)
  {
    /* Check temperature */
    if (ProgramManager_gCurrentTemperature >= ProgramManager_gCurrentTempThreshold)
    {
      if (ProgramManager_AutoRunWash_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)true;

        ProgramManager_AutoRunWash_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWash_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWash_TempCounter = (uint32_t)0U;
    }
  }
  else
  {
    /* Check temperature with delta offset */
    if (ProgramManager_gCurrentTemperature <= (ProgramManager_gCurrentTempThreshold - ProgramManager_gParamConfig.heatTempCfg.tempDiffReheat))
    {
      if (ProgramManager_AutoRunWash_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)false;

        ProgramManager_AutoRunWash_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWash_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWash_TempCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWash_InternalCheckMotorCondition(void)
{
  if (ProgramManager_AutoRunWash_MotorCounter >= ProgramManager_AutoRunWash_MotorCounterMax)
  {
    ProgramManager_AutoRunWash_MotorCounter = (uint32_t)0U;

    ProgramManager_AutoRunWash_MotorState += (uint32_t)1U;

    if (ProgramManager_AutoRunWash_MotorState >= PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_MAX)
    {
      ProgramManager_AutoRunWash_MotorState = PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_FWD;
    }

    switch (ProgramManager_AutoRunWash_MotorState)
    {
      case PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_FWD:
      case PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_REV:
      {
        ProgramManager_AutoRunWash_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

        break;
      }
      case PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_STOP1:
      case PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_STOP2:
      default:
      {
        ProgramManager_AutoRunWash_MotorCounterMax = ProgramManager_gCurrentWashStopTime;

        break;
      }
    }
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoRunWash_InternalCheckStateTransit(void)
{
  bool conditionOk = (bool)true;
  Fsm_DataHierachyStruct *dataHierachy;

  if (ProgramManager_Control_NotPauseAndError())
  {
    /* Wash times expired, and motor state is in stop state */
    if ((ProgramManager_AutoRunWash_WashCount != (uint32_t)0U) || \
        (ProgramManager_AutoRunWash_MotorState == PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_FWD) || \
        (ProgramManager_AutoRunWash_MotorState == PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_REV))
    {
      conditionOk = (bool)false;
    }

    if (conditionOk == (bool)true)
    {
      dataHierachy = (Fsm_DataHierachyStruct *)ProgramManager_malloc(sizeof(Fsm_DataHierachyStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WASH;

      ProgramManager_FsmContext.dataHierachy = dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWASH_EVENT_RUN_DRAIN);
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWash_InternalControlOutput(void)
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

  /* Turn off all soaps */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);

  /* Control motor */
  if (ProgramManager_AutoRunWash_MotorState == PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_FWD)
  {
    ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK);
  }
  else if (ProgramManager_AutoRunWash_MotorState == PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_REV)
  {
    ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_REV_MASK);
  }
  else
  {
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK);
  }

  ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK, \
                                      ProgramManager_gCurrentWashSpeed << PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_OFFSET);

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
static void ProgramManager_AutoRunWash_InternalUpdateLcdParams(void)
{
  uint32_t timeTemp;

  timeTemp = ProgramManager_AutoRunWash_WashCount;

  ProgramManager_gTimeCountMin = timeTemp / (uint32_t)60U;
  ProgramManager_gTimeCountSec = timeTemp % (uint32_t)60U;

  ProgramManager_gMotorState = (uint8_t)ProgramManager_AutoRunWash_MotorState;
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunWash_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Control_RunWashStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_PRE_RUN)
    {
      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNWASH_INTERNALDATALENGTH);

      ProgramManager_AutoRunWash_OneSecondElapsed = (uint32_t)0U;

      ProgramManager_AutoRunWash_TempCounter = (uint32_t)0U;
      ProgramManager_AutoRunWash_PresCounter = (uint32_t)0U;

      ProgramManager_AutoRunWash_WashCount = ProgramManager_gAutoSeqConfig.normStep[ProgramManager_gAutoSeqConfig.currentStep].washTime;

      ProgramManager_AutoRunWash_MotorState = PROGRAMMANAGER_AUTORUNWASH_MOTORSTATE_FWD;
      ProgramManager_AutoRunWash_MotorCounter = (uint32_t)0U;
      ProgramManager_AutoRunWash_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT)
    {
      enterDataHierachy = (ProgramManager_Control_RunWashStruct *)(pFsmContext->dataHierachy);

      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNWASH_INTERNALDATALENGTH);

      ProgramManager_AutoRunWash_OneSecondElapsed = enterDataHierachy->oneSecondElapsed;

      ProgramManager_AutoRunWash_TempCounter = enterDataHierachy->tempCounter;
      ProgramManager_AutoRunWash_PresCounter = enterDataHierachy->presCounter;

      ProgramManager_AutoRunWash_WashCount = ProgramManager_gAutoSeqConfig.normStep[ProgramManager_gAutoSeqConfig.currentStep].washTime;

      ProgramManager_AutoRunWash_MotorState = enterDataHierachy->motorState;
      ProgramManager_AutoRunWash_MotorCounter = enterDataHierachy->motorCounter;
      ProgramManager_AutoRunWash_MotorCounterMax = enterDataHierachy->motorCounterMax;

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
    ProgramManager_SubMainFunctionPush(ProgramManager_AutoRunWash_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_AutoRunWash_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunWash_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunWash_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoRunWash_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoRunWash_InternalCheckLevelCondition();
    ProgramManager_AutoRunWash_InternalCheckTempCondition();
    ProgramManager_AutoRunWash_InternalCheckMotorCondition();

    ProgramManager_AutoRunWash_InternalControlOutput();

    ProgramManager_AutoRunWash_InternalUpdateLcdParams();

    ProgramManager_AutoRunWash_InternalCheckStateTransit();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWash_SubTickHandler(void)
{
  ProgramManager_AutoRunWash_OneSecondElapsed += (uint32_t)1U;

  if (ProgramManager_AutoRunWash_OneSecondElapsed >= PROGRAMMANAGER_AUTORUNWASH_ONESECONDELAPSED_MAX)
  {
    ProgramManager_AutoRunWash_OneSecondElapsed = (uint32_t)0U;

    ProgramManager_AutoRunWash_MotorCounter += (uint32_t)1U;

    if (ProgramManager_AutoRunWash_WashCount != (uint32_t)0U)
    {
      ProgramManager_AutoRunWash_WashCount -= (uint32_t)1U;
    }
  }
}



#ifdef  __cplusplus
}
#endif
