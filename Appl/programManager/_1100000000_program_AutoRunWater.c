/* 
 * File:   _1100000000_program_AutoRunWater.c
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

#define PROGRAMMANAGER_AUTORUNWATER_INTERNALDATALENGTH                (uint8_t)7U

#define PROGRAMMANAGER_AUTORUNWATER_ONESECONDELAPSED_IDX              0U
#define PROGRAMMANAGER_AUTORUNWATER_TEMPCOUNTER_IDX                   1U
#define PROGRAMMANAGER_AUTORUNWATER_PRESCOUNTER_IDX                   2U
#define PROGRAMMANAGER_AUTORUNWATER_PRESCOUNTERSOAP_IDX               3U
#define PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_IDX                    4U
#define PROGRAMMANAGER_AUTORUNWATER_MOTORCOUNTER_IDX                  5U
#define PROGRAMMANAGER_AUTORUNWATER_MOTORCOUNTERMAX_IDX               6U

#define ProgramManager_AutoRunWater_OneSecondElapsed                  ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATER_ONESECONDELAPSED_IDX)
#define ProgramManager_AutoRunWater_TempCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATER_TEMPCOUNTER_IDX)
#define ProgramManager_AutoRunWater_PresCounter                       ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATER_PRESCOUNTER_IDX)
#define ProgramManager_AutoRunWater_PresCounterSoap                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATER_PRESCOUNTERSOAP_IDX)
#define ProgramManager_AutoRunWater_MotorState                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_IDX)
#define ProgramManager_AutoRunWater_MotorCounter                      ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATER_MOTORCOUNTER_IDX)
#define ProgramManager_AutoRunWater_MotorCounterMax                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATER_MOTORCOUNTERMAX_IDX)

#define PROGRAMMANAGER_AUTORUNWATER_ONESECONDELAPSED_MAX              (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_AUTORUNWATER_EVENT_RUN_HEAT                    PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTORUNWATER_EVENT_RUN_WASH                    PROGRAMMANAGER_EVENT_SUBMENU_2
#define PROGRAMMANAGER_AUTORUNWATER_EVENT_POST_RUN                    PROGRAMMANAGER_EVENT_SUBMENU_3

#define PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_FWD                    0U
#define PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_STOP1                  1U
#define PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_REV                    2U
#define PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_STOP2                  3U
#define PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_MAX                    4U

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoRunWater_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoRunWater_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoRunWater_StateMachine[5] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoRunWater_Entry                                               ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoRunWater_Exit                                                ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNWATER_EVENT_RUN_HEAT,                                   PROGRAMMANAGER_STATE_AUTO_RUN_HEAT      ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNWATER_EVENT_RUN_WASH,                                   PROGRAMMANAGER_STATE_AUTO_RUN_WASH      ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNWATER_EVENT_POST_RUN,                                   PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoRunWater_InternalCommandHandler(void);

static void ProgramManager_AutoRunWater_InternalCheckLevelCondition(void);
static void ProgramManager_AutoRunWater_InternalCheckTempCondition(void);
static void ProgramManager_AutoRunWater_InternalCheckMotorCondition(void);

static void ProgramManager_AutoRunWater_InternalCheckStateTransit(void);
static void ProgramManager_AutoRunWater_InternalControlOutput(void);

static void ProgramManager_AutoRunWater_InternalUpdateLcdParams(void);

static void ProgramManager_AutoRunWater_SubMainFunction(void);
static void ProgramManager_AutoRunWater_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_AutoRunWater_InternalCommandHandler(void)
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
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATER_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP:
    {
      stateTransit = ProgramManager_Control_CheckNextStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATER_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {
      stateTransit = ProgramManager_Control_CheckPrevStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATER_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_SUBSTEP:
    {
      runDataHierachy = (ProgramManager_Control_RunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_RunStruct));
      runDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER;

      runDataHierachy->oneSecondElapsed  = ProgramManager_AutoRunWater_OneSecondElapsed;
      runDataHierachy->tempCounter       = ProgramManager_AutoRunWater_TempCounter;
      runDataHierachy->presCounter       = ProgramManager_AutoRunWater_PresCounter;
      runDataHierachy->motorState        = ProgramManager_AutoRunWater_MotorState;
      runDataHierachy->motorCounter      = ProgramManager_AutoRunWater_MotorCounter;
      runDataHierachy->motorCounterMax   = ProgramManager_AutoRunWater_MotorCounterMax;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)runDataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATER_EVENT_RUN_WASH);

      stateTransit = true;

      break;
    }
    default:
      break;
  }

  return stateTransit;
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWater_InternalCheckLevelCondition(void)
{
  if (ProgramManager_gPresThresExceeded == (bool)false)
  {
    /* Check pressure */
    if (ProgramManager_gCurrentPressure >= ProgramManager_gCurrentPresThreshold)
    {
      if (ProgramManager_AutoRunWater_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)true;

        ProgramManager_AutoRunWater_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWater_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWater_PresCounter = (uint32_t)0U;
    }

    /* Check pressure to pour soap */
    if (ProgramManager_gCurrentPressure >= ProgramManager_gParamConfig.fillLevelCfg.soapStartLevel)
    {
      if (ProgramManager_AutoRunWater_PresCounterSoap < PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_AutoRunWater_PresCounterSoap += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWater_PresCounterSoap = (uint32_t)0U;
    }
  }
  else
  {
    /* Auto-refill when low level */
    /* Check pressure with delta offset */
    if (ProgramManager_gCurrentPressure <= (ProgramManager_gCurrentPresThreshold - ProgramManager_gParamConfig.fillLevelCfg.levelDiffRefill))
    {
      if (ProgramManager_AutoRunWater_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)false;

        ProgramManager_AutoRunWater_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWater_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWater_PresCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWater_InternalCheckTempCondition(void)
{
  if (ProgramManager_gTempThresExceeded == (bool)false)
  {
    /* Check temperature */
    if (ProgramManager_gCurrentTemperature >= ProgramManager_gCurrentTempThreshold)
    {
      if (ProgramManager_AutoRunWater_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)true;

        ProgramManager_AutoRunWater_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWater_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWater_TempCounter = (uint32_t)0U;
    }
  }
  else
  {
    /* Check temperature with delta offset */
    if (ProgramManager_gCurrentTemperature <= (ProgramManager_gCurrentTempThreshold - ProgramManager_gParamConfig.heatTempCfg.tempDiffReheat))
    {
      if (ProgramManager_AutoRunWater_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)false;

        ProgramManager_AutoRunWater_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWater_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWater_TempCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWater_InternalCheckMotorCondition(void)
{
  if (ProgramManager_AutoRunWater_MotorCounter >= ProgramManager_AutoRunWater_MotorCounterMax)
  {
    ProgramManager_AutoRunWater_MotorCounter = (uint32_t)0U;

    ProgramManager_AutoRunWater_MotorState += (uint32_t)1U;

    if (ProgramManager_AutoRunWater_MotorState >= PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_MAX)
    {
      ProgramManager_AutoRunWater_MotorState = PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_FWD;
    }

    switch (ProgramManager_AutoRunWater_MotorState)
    {
      case PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_FWD:
      case PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_REV:
      {
        ProgramManager_AutoRunWater_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

        break;
      }
      case PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_STOP1:
      case PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_STOP2:
      default:
      {
        ProgramManager_AutoRunWater_MotorCounterMax = ProgramManager_gCurrentWashStopTime;

        break;
      }
    }
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoRunWater_InternalCheckStateTransit(void)
{
  bool conditionOk = (bool)true;
  ProgramManager_Control_RunStruct *dataHierachy;

  if (ProgramManager_Control_NotPaused())
  {
    /* Pressure threshold not reach */
    if (ProgramManager_gPresThresExceeded == (bool)false)
    {
      conditionOk = (bool)false;
    }

    if (conditionOk == (bool)true)
    {
      dataHierachy = (ProgramManager_Control_RunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_RunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER;

      dataHierachy->oneSecondElapsed  = ProgramManager_AutoRunWater_OneSecondElapsed;
      dataHierachy->tempCounter       = ProgramManager_AutoRunWater_TempCounter;
      dataHierachy->presCounter       = ProgramManager_AutoRunWater_PresCounter;
      dataHierachy->motorState        = ProgramManager_AutoRunWater_MotorState;
      dataHierachy->motorCounter      = ProgramManager_AutoRunWater_MotorCounter;
      dataHierachy->motorCounterMax   = ProgramManager_AutoRunWater_MotorCounterMax;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      if (ProgramManager_gTempThresExceeded == (bool)false)
      {
        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATER_EVENT_RUN_HEAT);
      }
      else
      {
        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATER_EVENT_RUN_WASH);
      }
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWater_InternalControlOutput(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    /* Control heat generator - always off */
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);

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

    /* Control soap */
    if (ProgramManager_AutoRunWater_PresCounterSoap >= PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
    {
      if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].soapMode & (uint8_t)0x01)
      {
        ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK);
      }
      else
      {
        ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK);
      }

      if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].soapMode & (uint8_t)0x02)
      {
        ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK);
      }
      else
      {
        ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK);
      }

      if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].soapMode & (uint8_t)0x04)
      {
        ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);
      }
      else
      {
        ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);
      }
    }

    /* Control motor */
    if (ProgramManager_AutoRunWater_MotorState == PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_FWD)
    {
      ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK);
    }
    else if (ProgramManager_AutoRunWater_MotorState == PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_REV)
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
static void ProgramManager_AutoRunWater_InternalUpdateLcdParams(void)
{
  ProgramManager_gTimeCountMin = (uint32_t)0U;
  ProgramManager_gTimeCountSec = (uint32_t)0U;
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunWater_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Control_RunStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_PRE_RUN)
    {
      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNWATER_INTERNALDATALENGTH);

      ProgramManager_AutoRunWater_OneSecondElapsed = (uint32_t)0U;

      ProgramManager_AutoRunWater_TempCounter = (uint32_t)0U;
      ProgramManager_AutoRunWater_PresCounter = (uint32_t)0U;

      ProgramManager_AutoRunWater_MotorState = PROGRAMMANAGER_AUTORUNWATER_MOTORSTATE_FWD;
      ProgramManager_AutoRunWater_MotorCounter = (uint32_t)0U;
      ProgramManager_AutoRunWater_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if ((pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_HEAT) || \
             (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_RUN_WASH))
    {
      enterDataHierachy = (ProgramManager_Control_RunStruct *)(pFsmContext->dataHierachy);

      ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNWATER_INTERNALDATALENGTH);

      ProgramManager_AutoRunWater_OneSecondElapsed = enterDataHierachy->oneSecondElapsed;

      ProgramManager_AutoRunWater_TempCounter = enterDataHierachy->tempCounter;
      ProgramManager_AutoRunWater_PresCounter = enterDataHierachy->presCounter;

      ProgramManager_AutoRunWater_MotorState = enterDataHierachy->motorState;
      ProgramManager_AutoRunWater_MotorCounter = enterDataHierachy->motorCounter;
      ProgramManager_AutoRunWater_MotorCounterMax = enterDataHierachy->motorCounterMax;

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
    ProgramManager_AutoRunWater_PresCounterSoap = (uint32_t)0U;

    ProgramManager_SubMainFunctionPush(ProgramManager_AutoRunWater_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_AutoRunWater_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunWater_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunWater_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoRunWater_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoRunWater_InternalCheckLevelCondition();
    ProgramManager_AutoRunWater_InternalCheckTempCondition();
    ProgramManager_AutoRunWater_InternalCheckMotorCondition();

    ProgramManager_AutoRunWater_InternalControlOutput();

    ProgramManager_AutoRunWater_InternalUpdateLcdParams();

    ProgramManager_AutoRunWater_InternalCheckStateTransit();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWater_SubTickHandler(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    ProgramManager_AutoRunWater_OneSecondElapsed += (uint32_t)1U;

    if (ProgramManager_AutoRunWater_OneSecondElapsed >= PROGRAMMANAGER_AUTORUNWATER_ONESECONDELAPSED_MAX)
    {
      ProgramManager_AutoRunWater_OneSecondElapsed = (uint32_t)0U;

      ProgramManager_AutoRunWater_MotorCounter += (uint32_t)1U;
    }
  }
  else
  {
    ProgramManager_AutoRunWater_OneSecondElapsed = (uint32_t)0U;
  }
}



#ifdef  __cplusplus
}
#endif
