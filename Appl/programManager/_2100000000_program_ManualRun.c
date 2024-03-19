/* 
 * File:   _2100000000_program_ManualRun.c
 * Author: Long
 * 
 * In RUN state, 
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

#define PROGRAMMANAGER_MANUALRUN_INTERNALDATALENGTH                   (uint8_t)7U

#define PROGRAMMANAGER_MANUALRUN_ONESECONDELAPSED_IDX                 0U
#define PROGRAMMANAGER_MANUALRUN_TEMPCOUNTER_IDX                      1U
#define PROGRAMMANAGER_MANUALRUN_PRESCOUNTER_IDX                      2U
#define PROGRAMMANAGER_MANUALRUN_PRESCOUNTERSOAP_IDX                  3U
#define PROGRAMMANAGER_MANUALRUN_MOTORSTATE_IDX                       4U
#define PROGRAMMANAGER_MANUALRUN_MOTORCOUNTER_IDX                     5U
#define PROGRAMMANAGER_MANUALRUN_MOTORCOUNTERMAX_IDX                  6U

#define ProgramManager_ManualRun_OneSecondElapsed                     ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALRUN_ONESECONDELAPSED_IDX)
#define ProgramManager_ManualRun_TempCounter                          ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALRUN_TEMPCOUNTER_IDX)
#define ProgramManager_ManualRun_PresCounter                          ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALRUN_PRESCOUNTER_IDX)
#define ProgramManager_ManualRun_PresCounterSoap                      ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALRUN_PRESCOUNTERSOAP_IDX)
#define ProgramManager_ManualRun_MotorState                           ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALRUN_MOTORSTATE_IDX)
#define ProgramManager_ManualRun_MotorCounter                         ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALRUN_MOTORCOUNTER_IDX)
#define ProgramManager_ManualRun_MotorCounterMax                      ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_MANUALRUN_MOTORCOUNTERMAX_IDX)

#define PROGRAMMANAGER_MANUALRUN_ONESECONDELAPSED_MAX                 (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_MANUALRUN_EVENT_POST_RUN                       PROGRAMMANAGER_EVENT_SUBMENU_1

#define PROGRAMMANAGER_MANUALRUN_MOTORSTATE_FWD                       0U
#define PROGRAMMANAGER_MANUALRUN_MOTORSTATE_STOP1                     1U
#define PROGRAMMANAGER_MANUALRUN_MOTORSTATE_REV                       2U
#define PROGRAMMANAGER_MANUALRUN_MOTORSTATE_STOP2                     3U
#define PROGRAMMANAGER_MANUALRUN_MOTORSTATE_MAX                       4U

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_ManualRun_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_ManualRun_Exit                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_ManualRun_StateMachine[3] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_ManualRun_Entry                                                  ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_ManualRun_Exit                                                   ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_MANUALRUN_EVENT_POST_RUN,                                      PROGRAMMANAGER_STATE_MANUAL_POST_RUN    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_ManualRun_InternalCommandHandler(void);

static void ProgramManager_ManualRun_InternalCheckLevelCondition(void);
static void ProgramManager_ManualRun_InternalCheckTempCondition(void);
static void ProgramManager_ManualRun_InternalCheckMotorCondition(void);

static void ProgramManager_ManualRun_InternalControlOutput(void);

static void ProgramManager_ManualRun_InternalUpdateLcdParams(void);

static void ProgramManager_ManualRun_SubMainFunction(void);
static void ProgramManager_ManualRun_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_ManualRun_InternalCommandHandler(void)
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
      dataHierachy->dataId = PROGRAMMANAGER_STATE_MANUAL_RUN;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_MANUALRUN_EVENT_POST_RUN);

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
static void ProgramManager_ManualRun_InternalCheckLevelCondition(void)
{
  if (ProgramManager_gPresThresExceeded == (bool)false)
  {
    /* Check pressure */
    if (ProgramManager_gCurrentPressure >= ProgramManager_gCurrentPresThreshold)
    {
      if (ProgramManager_ManualRun_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)true;

        ProgramManager_ManualRun_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_ManualRun_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_ManualRun_PresCounter = (uint32_t)0U;
    }

    /* Check pressure to pour soap */
    if (ProgramManager_gCurrentPressure >= ProgramManager_gParamConfig.fillLevelCfg.soapStartLevel)
    {
      if (ProgramManager_ManualRun_PresCounterSoap < PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_ManualRun_PresCounterSoap += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_ManualRun_PresCounterSoap = (uint32_t)0U;
    }
  }
  else
  {
    /* Auto-refill when low level */
    /* Check pressure with delta offset */
    if (ProgramManager_gCurrentPressure <= (ProgramManager_gCurrentPresThreshold - ProgramManager_gParamConfig.fillLevelCfg.levelDiffRefill))
    {
      if (ProgramManager_ManualRun_PresCounter == PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        ProgramManager_gPresThresExceeded = (bool)false;

        ProgramManager_ManualRun_PresCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_ManualRun_PresCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_ManualRun_PresCounter = (uint32_t)0U;
    }

    ProgramManager_ManualRun_PresCounterSoap = PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY;
  }
}

/*=============================================================================================*/
static void ProgramManager_ManualRun_InternalCheckTempCondition(void)
{
  if (ProgramManager_gTempThresExceeded == (bool)false)
  {
    /* Check temperature */
    if (ProgramManager_gCurrentTemperature >= ProgramManager_gCurrentTempThreshold)
    {
      if (ProgramManager_ManualRun_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)true;

        ProgramManager_ManualRun_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_ManualRun_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_ManualRun_TempCounter = (uint32_t)0U;
    }
  }
  else
  {
    /* Check temperature with delta offset */
    if (ProgramManager_gCurrentTemperature <= (ProgramManager_gCurrentTempThreshold - ProgramManager_gParamConfig.heatTempCfg.tempDiffReheat))
    {
      if (ProgramManager_ManualRun_TempCounter == PROGRAMMANAGER_CONTROL_TEMP_THRES_DELAY)
      {
        ProgramManager_gTempThresExceeded = (bool)false;

        ProgramManager_ManualRun_TempCounter = (uint32_t)0U;
      }
      else
      {
        ProgramManager_ManualRun_TempCounter += (uint32_t)1U;
      }
    }
    else
    {
      ProgramManager_ManualRun_TempCounter = (uint32_t)0U;
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_ManualRun_InternalCheckMotorCondition(void)
{
  if (ProgramManager_ManualRun_MotorCounter >= ProgramManager_ManualRun_MotorCounterMax)
  {
    ProgramManager_ManualRun_MotorCounter = (uint32_t)0U;

    ProgramManager_ManualRun_MotorState += (uint32_t)1U;

    if (ProgramManager_ManualRun_MotorState >= PROGRAMMANAGER_MANUALRUN_MOTORSTATE_MAX)
    {
      ProgramManager_ManualRun_MotorState = PROGRAMMANAGER_MANUALRUN_MOTORSTATE_FWD;
    }

    switch (ProgramManager_ManualRun_MotorState)
    {
      case PROGRAMMANAGER_MANUALRUN_MOTORSTATE_FWD:
      case PROGRAMMANAGER_MANUALRUN_MOTORSTATE_REV:
      {
        ProgramManager_ManualRun_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

        break;
      }
      case PROGRAMMANAGER_MANUALRUN_MOTORSTATE_STOP1:
      case PROGRAMMANAGER_MANUALRUN_MOTORSTATE_STOP2:
      default:
      {
        ProgramManager_ManualRun_MotorCounterMax = ProgramManager_gCurrentWashStopTime;

        break;
      }
    }
  }
}



/*=============================================================================================*/
static void ProgramManager_ManualRun_InternalControlOutput(void)
{
  if (ProgramManager_Control_NotPaused())
  {
#if 0
    /* Control heat generator through temperature */
    if (ProgramManager_gTempThresExceeded == (bool)false)
    {
      ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);
    }
    else
    {
      ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);
    }
#endif

    /* Control water through pressure */
    if (ProgramManager_gPresThresExceeded == (bool)false)
    {
      if (
          ( ProgramManager_Control_IsManualOptionColdWater() ) || \
          ( ( ProgramManager_Control_IsManualOptionSupply1() || ProgramManager_Control_IsManualOptionSupply2() || ProgramManager_Control_IsManualOptionSupply3()) && (ProgramManager_ManualRun_PresCounterSoap < PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY) )
         )
      {
        ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_COLD_WATER_MASK);
      }
      else
      {
        ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_COLD_WATER_MASK);
      }

      if (ProgramManager_Control_IsManualOptionHotWater())
      {
        ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HOT_WATER_MASK);
      }
      else
      {
        ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HOT_WATER_MASK);
      }
    }
    else
    {
      ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_WATER_MASK);
    }

    /* Control soap */
    if (ProgramManager_gPresThresExceeded == (bool)false)
    {
      if (ProgramManager_ManualRun_PresCounterSoap >= PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY)
      {
        if (ProgramManager_Control_IsManualOptionSupply1())
        {
          ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK);
        }
        else
        {
          ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK);
        }

        if (ProgramManager_Control_IsManualOptionSupply2())
        {
          ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK);
        }
        else
        {
          ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK);
        }

        if (ProgramManager_Control_IsManualOptionSupply3())
        {
          ProgramManager_Control_SetOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);
        }
        else
        {
          ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);
        }
      }
      else
      {
        ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_MASK);
      }
    }
    else
    {
      ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_MASK);
    }
    
    /* Control motor */
    if (ProgramManager_Control_IsManualOptionWash())
    {
      if (ProgramManager_ManualRun_MotorState == PROGRAMMANAGER_MANUALRUN_MOTORSTATE_FWD)
      {
        ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK);
      }
      else if (ProgramManager_ManualRun_MotorState == PROGRAMMANAGER_MANUALRUN_MOTORSTATE_REV)
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
      /* Reset counter when no washing */
      ProgramManager_ManualRun_MotorCounter = (uint32_t)0U;

      ProgramManager_Control_ClearOutput((PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK));
    }

    /* Control drain valve */
    if ( \
        ProgramManager_Control_IsManualOptionColdWater() || \
        ProgramManager_Control_IsManualOptionHotWater()  || \
        ProgramManager_Control_IsManualOptionSupply1()   || \
        ProgramManager_Control_IsManualOptionSupply2()   || \
        ProgramManager_Control_IsManualOptionSupply3()      \
       )
    {
      ProgramManager_Control_DrainCloseHandler();
    }
  }
  else
  {
    ProgramManager_Control_ClearAllOutputExceptDrain();
  }
}

/*=============================================================================================*/
static void ProgramManager_ManualRun_InternalUpdateLcdParams(void)
{
#if 0
  uint32_t timeTemp;

  timeTemp = (uint32_t)0U;

  ProgramManager_gTimeCountMin = timeTemp / (uint32_t)60U;
  ProgramManager_gTimeCountSec = timeTemp % (uint32_t)60U;
#endif

  ProgramManager_gMotorState = (uint8_t)ProgramManager_ManualRun_MotorState;
}



/*=============================================================================================*/
static Fsm_GuardType ProgramManager_ManualRun_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_MANUAL_PRE_RUN)
    {
      ProgramManager_InternalDataPush(PROGRAMMANAGER_MANUALRUN_INTERNALDATALENGTH);

      ProgramManager_ManualRun_OneSecondElapsed = (uint32_t)0U;

      ProgramManager_ManualRun_TempCounter = (uint32_t)0U;
      ProgramManager_ManualRun_PresCounter = (uint32_t)0U;

      if (ProgramManager_gCurrentPressure >= ProgramManager_gParamConfig.fillLevelCfg.soapStartLevel)
      {
        ProgramManager_ManualRun_PresCounterSoap = PROGRAMMANAGER_CONTROL_PRES_THRES_DELAY;
      }
      else
      {
        ProgramManager_ManualRun_PresCounterSoap = (uint32_t)0U;
      }

      ProgramManager_ManualRun_MotorState = PROGRAMMANAGER_MANUALRUN_MOTORSTATE_FWD;
      ProgramManager_ManualRun_MotorCounter = (uint32_t)0U;
      ProgramManager_ManualRun_MotorCounterMax = ProgramManager_gCurrentWashRunTime;

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
    ProgramManager_SubMainFunctionPush(ProgramManager_ManualRun_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_ManualRun_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_ManualRun_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_ManualRun_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_ManualRun_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_ManualRun_InternalCheckLevelCondition();
    ProgramManager_ManualRun_InternalCheckTempCondition();
    ProgramManager_ManualRun_InternalCheckMotorCondition();

    ProgramManager_ManualRun_InternalControlOutput();

    ProgramManager_ManualRun_InternalUpdateLcdParams();
  }
}

/*=============================================================================================*/
static void ProgramManager_ManualRun_SubTickHandler(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    ProgramManager_ManualRun_OneSecondElapsed += (uint32_t)1U;

    if (ProgramManager_ManualRun_OneSecondElapsed >= PROGRAMMANAGER_MANUALRUN_ONESECONDELAPSED_MAX)
    {
      ProgramManager_ManualRun_OneSecondElapsed = (uint32_t)0U;

      if (ProgramManager_Control_IsManualOptionWash())
      {
        ProgramManager_ManualRun_MotorCounter += (uint32_t)1U;
      }
    }
  }
  else
  {
    ProgramManager_ManualRun_OneSecondElapsed = (uint32_t)0U;
  }
}



#ifdef  __cplusplus
}
#endif
