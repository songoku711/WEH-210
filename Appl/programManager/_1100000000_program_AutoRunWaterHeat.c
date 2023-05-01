/* 
 * File:   _1100000000_program_AutoRunWaterHeat.c
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

#define PROGRAMMANAGER_AUTORUNWATERHEAT_INTERNALDATALENGTH            (uint8_t)7U

#define PROGRAMMANAGER_AUTORUNWATERHEAT_ONESECONDELAPSED_IDX          0U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_TEMPCOUNTER_IDX               1U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_PRESCOUNTER_IDX               2U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_TEMPTIMEOUT_IDX               3U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_IDX                4U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORCOUNTER_IDX              5U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORCOUNTERMAX_IDX           6U

#define ProgramManager_AutoRunWaterHeat_OneSecondElapsed              ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATERHEAT_ONESECONDELAPSED_IDX)
#define ProgramManager_AutoRunWaterHeat_TempCounter                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATERHEAT_TEMPCOUNTER_IDX)
#define ProgramManager_AutoRunWaterHeat_PresCounter                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATERHEAT_PRESCOUNTER_IDX)
#define ProgramManager_AutoRunWaterHeat_TempTimeout                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATERHEAT_TEMPTIMEOUT_IDX)
#define ProgramManager_AutoRunWaterHeat_MotorState                    ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_IDX)
#define ProgramManager_AutoRunWaterHeat_MotorCounter                  ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORCOUNTER_IDX)
#define ProgramManager_AutoRunWaterHeat_MotorCounterMax               ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORCOUNTERMAX_IDX)

#define PROGRAMMANAGER_AUTORUNWATERHEAT_ONESECONDELAPSED_MAX          (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_RUN_WASH                PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_POST_RUN                PROGRAMMANAGER_EVENT_SUBMENU_2

#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_FWD                0U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_STOP1              1U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_REV                2U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_STOP2              3U
#define PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_MAX                4U

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoRunWaterHeat_Entry            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoRunWaterHeat_Exit             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoRunWaterHeat_StateMachine[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoRunWaterHeat_Entry                                           ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoRunWaterHeat_Exit                                            ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_RUN_WASH,                               PROGRAMMANAGER_STATE_AUTO_RUN_WASH      ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_POST_RUN,                               PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoRunWaterHeat_InternalCommandHandler(void);

static void ProgramManager_AutoRunWaterHeat_InternalCheckLevelCondition(void);
static void ProgramManager_AutoRunWaterHeat_InternalCheckTempCondition(void);
static void ProgramManager_AutoRunWaterHeat_InternalCheckMotorCondition(void);

static void ProgramManager_AutoRunWaterHeat_InternalCheckStateTransit(void);
static void ProgramManager_AutoRunWaterHeat_InternalControlOutput(void);

static void ProgramManager_AutoRunWaterHeat_InternalUpdateLcdParams(void);

static void ProgramManager_AutoRunWaterHeat_SubMainFunction(void);
static void ProgramManager_AutoRunWaterHeat_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_AutoRunWaterHeat_InternalCommandHandler(void)
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
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP:
    {
      stateTransit = ProgramManager_Control_CheckNextStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {
      stateTransit = ProgramManager_Control_CheckPrevStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_POST_RUN);
      }

      break;
    }
    default:
      break;
  }

  return stateTransit;
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWaterHeat_InternalCheckLevelCondition(void)
{
  if (ProgramManager_gPresThresExceeded == (bool)false)
  {
    /* Check pressure */
    if (ProgramManager_gCurrentPressure >= ProgramManager_gCurrentPresThreshold)
    {
      if (ProgramManager_AutoRunWaterHeat_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)true;

        ProgramManager_AutoRunWaterHeat_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWaterHeat_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWaterHeat_PresCounter = (uint32_t)0U;
    }
  }
  else
  {
    /* Auto-refill when low level */
    /* Check pressure with delta offset */
    if (ProgramManager_gCurrentPressure <= (ProgramManager_gCurrentPresThreshold - ProgramManager_gParamConfig.fillLevelCfg.levelDiffRefill))
    {
      if (ProgramManager_AutoRunWaterHeat_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)false;

        ProgramManager_AutoRunWaterHeat_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWaterHeat_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWaterHeat_PresCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWaterHeat_InternalCheckTempCondition(void)
{
  if (ProgramManager_gTempThresExceeded == (bool)false)
  {
    /* Check temperature */
    if (ProgramManager_gCurrentTemperature >= ProgramManager_gCurrentTempThreshold)
    {
      if (ProgramManager_AutoRunWaterHeat_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)true;

        ProgramManager_AutoRunWaterHeat_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWaterHeat_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWaterHeat_TempCounter = (uint32_t)0U;
    }

    if (ProgramManager_AutoRunWaterHeat_TempTimeout != (uint32_t)0U)
    {
      ProgramManager_AutoRunWaterHeat_TempTimeout -= (uint32_t)1U;
    }
  }
  else
  {
    /* Check temperature with delta offset */
    if (ProgramManager_gCurrentTemperature <= (ProgramManager_gCurrentTempThreshold - ProgramManager_gParamConfig.heatTempCfg.tempDiffReheat))
    {
      if (ProgramManager_AutoRunWaterHeat_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)false;

        ProgramManager_AutoRunWaterHeat_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_AutoRunWaterHeat_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_AutoRunWaterHeat_TempCounter = (uint32_t)0U;
    }

    ProgramManager_AutoRunWaterHeat_TempTimeout = (uint32_t)(ProgramManager_gParamConfig.heatTempCfg.maxTimeHeat) * (uint32_t)12000U;
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWaterHeat_InternalCheckMotorCondition(void)
{
  if (ProgramManager_AutoRunWaterHeat_MotorCounter >= ProgramManager_AutoRunWaterHeat_MotorCounterMax)
  {
    ProgramManager_AutoRunWaterHeat_MotorCounter = (uint32_t)0U;

    ProgramManager_AutoRunWaterHeat_MotorState += (uint32_t)1U;

    if (ProgramManager_AutoRunWaterHeat_MotorState >= PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_MAX)
    {
      ProgramManager_AutoRunWaterHeat_MotorState = PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_FWD;
    }

    switch (ProgramManager_AutoRunWaterHeat_MotorState)
    {
      case PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_FWD:
      case PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_REV:
      {
        ProgramManager_AutoRunWaterHeat_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

        break;
      }
      case PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_STOP1:
      case PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_STOP2:
      default:
      {
        ProgramManager_AutoRunWaterHeat_MotorCounterMax = ProgramManager_gCurrentWashStopTime;

        break;
      }
    }
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoRunWaterHeat_InternalCheckStateTransit(void)
{
  bool conditionOk = (bool)true;
  ProgramManager_Control_RunWashStruct *dataHierachy;

  if (ProgramManager_Control_NotPauseAndError())
  {
    /* Temperature threshold not reach, and timeout not elapse if configured */
    if (ProgramManager_gTempThresExceeded == (bool)false)
    {
      if (ProgramManager_AutoRunWaterHeat_TempTimeout != (uint32_t)0U)
      {
        conditionOk = (bool)false;
      }
    }

    /* Pressure threshold not reach */
    if (ProgramManager_gPresThresExceeded == (bool)false)
    {
      conditionOk = (bool)false;
    }

    if (conditionOk == (bool)true)
    {
      dataHierachy = (ProgramManager_Control_RunWashStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_RunWashStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_WATER_HEAT;

      dataHierachy->oneSecondElapsed  = ProgramManager_AutoRunWaterHeat_OneSecondElapsed;
      dataHierachy->tempCounter       = ProgramManager_AutoRunWaterHeat_TempCounter;
      dataHierachy->presCounter       = ProgramManager_AutoRunWaterHeat_PresCounter;
      dataHierachy->motorState        = ProgramManager_AutoRunWaterHeat_MotorState;
      dataHierachy->motorCounter      = ProgramManager_AutoRunWaterHeat_MotorCounter;
      dataHierachy->motorCounterMax   = ProgramManager_AutoRunWaterHeat_MotorCounterMax;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNWATERHEAT_EVENT_RUN_WASH);
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWaterHeat_InternalControlOutput(void)
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

  /* @TODO: Control when water level reaches proper level to pour soap into */
  ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK);
  ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK);
  ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);

  /* Control motor */
  if (ProgramManager_AutoRunWaterHeat_MotorState == PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_FWD)
  {
    ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK);
  }
  else if (ProgramManager_AutoRunWaterHeat_MotorState == PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_REV)
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
static void ProgramManager_AutoRunWaterHeat_InternalUpdateLcdParams(void)
{
  uint32_t timeTemp;

  timeTemp = ProgramManager_AutoRunWaterHeat_TempTimeout;

  /* Time elapsed in second = time * (5 ticks per period) / 1000 ticks */
  timeTemp = timeTemp / (uint32_t)200U;

  ProgramManager_gTimeCountMin = timeTemp / (uint32_t)60U;
  ProgramManager_gTimeCountSec = timeTemp % (uint32_t)60U;
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunWaterHeat_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_PRE_RUN)
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
    ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNWATERHEAT_INTERNALDATALENGTH);

    ProgramManager_AutoRunWaterHeat_OneSecondElapsed = (uint32_t)0U;

    ProgramManager_AutoRunWaterHeat_TempCounter = (uint32_t)0U;
    ProgramManager_AutoRunWaterHeat_PresCounter = (uint32_t)0U;

    /* Timeout in minutes * 60s * 1000 ticks / (5 ticks per period) */
    ProgramManager_AutoRunWaterHeat_TempTimeout = (uint32_t)(ProgramManager_gParamConfig.heatTempCfg.maxTimeHeat) * (uint32_t)12000U;

    ProgramManager_AutoRunWaterHeat_MotorState = PROGRAMMANAGER_AUTORUNWATERHEAT_MOTORSTATE_FWD;
    ProgramManager_AutoRunWaterHeat_MotorCounter = (uint32_t)0U;
    ProgramManager_AutoRunWaterHeat_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

    ProgramManager_SubMainFunctionPush(ProgramManager_AutoRunWaterHeat_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_AutoRunWaterHeat_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunWaterHeat_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunWaterHeat_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoRunWaterHeat_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoRunWaterHeat_InternalCheckLevelCondition();
    ProgramManager_AutoRunWaterHeat_InternalCheckTempCondition();
    ProgramManager_AutoRunWaterHeat_InternalCheckMotorCondition();

    ProgramManager_AutoRunWaterHeat_InternalControlOutput();

    ProgramManager_AutoRunWaterHeat_InternalUpdateLcdParams();

    ProgramManager_AutoRunWaterHeat_InternalCheckStateTransit();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunWaterHeat_SubTickHandler(void)
{
  ProgramManager_AutoRunWaterHeat_OneSecondElapsed += (uint32_t)1U;

  if (ProgramManager_AutoRunWaterHeat_OneSecondElapsed >= PROGRAMMANAGER_AUTORUNWATERHEAT_ONESECONDELAPSED_MAX)
  {
    ProgramManager_AutoRunWaterHeat_OneSecondElapsed = (uint32_t)0U;

    ProgramManager_AutoRunWaterHeat_MotorCounter += (uint32_t)1U;
  }
}



#ifdef  __cplusplus
}
#endif
