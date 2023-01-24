/* 
 * File:   _1400000000_program_AutoRunExtract.c
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

#define PROGRAMMANAGER_AUTORUNEXTRACT_INTERNALDATALENGTH              (uint8_t)7U

#define PROGRAMMANAGER_AUTORUNEXTRACT_ONESECONDELAPSED_IDX            0U
#define PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_IDX                1U
#define PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_IDX                  2U
#define PROGRAMMANAGER_AUTORUNEXTRACT_MOTORCOUNTER_IDX                3U
#define PROGRAMMANAGER_AUTORUNEXTRACT_MOTORCOUNTERMAX_IDX             4U
#define PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSPEED_IDX                  5U

#define ProgramManager_AutoRunExtract_OneSecondElapsed                ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNEXTRACT_ONESECONDELAPSED_IDX)
#define ProgramManager_AutoRunExtract_ExtractLevel                    ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_IDX)
#define ProgramManager_AutoRunExtract_MotorState                      ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_IDX)
#define ProgramManager_AutoRunExtract_MotorCounter                    ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNEXTRACT_MOTORCOUNTER_IDX)
#define ProgramManager_AutoRunExtract_MotorCounterMax                 ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNEXTRACT_MOTORCOUNTERMAX_IDX)
#define ProgramManager_AutoRunExtract_MotorSpeed                      ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSPEED_IDX)

#define PROGRAMMANAGER_AUTORUNEXTRACT_ONESECONDELAPSED_MAX            (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_RUN_BALANCE               PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_POST_RUN                  PROGRAMMANAGER_EVENT_SUBMENU_2

#define PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_FIRSTRUN           0U
#define PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_MID                1U
#define PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH1              2U
#define PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH2              3U
#define PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH3              4U
#define PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_MAX                5U

#define PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_FWD                  0U
#define PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_STOP                 1U
#define PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_MAX                  2U

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoRunExtract_Entry              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoRunExtract_Exit               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoRunExtract_StateMachine[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoRunExtract_Entry                                             ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoRunExtract_Exit                                              ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_RUN_BALANCE,                              PROGRAMMANAGER_STATE_AUTO_RUN_BALANCE   ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_POST_RUN,                                 PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoRunExtract_InternalCommandHandler(void);

static void ProgramManager_AutoRunExtract_InternalCheckMotorCondition(void);

static void ProgramManager_AutoRunExtract_InternalCheckStateTransit(void);
static void ProgramManager_AutoRunExtract_InternalControlOutput(void);

static void ProgramManager_AutoRunExtract_InternalUpdateLcdParams(void);

static void ProgramManager_AutoRunExtract_SubMainFunction(void);
static void ProgramManager_AutoRunExtract_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_AutoRunExtract_InternalCommandHandler(void)
{
  uint8_t command = PROGRAMMANAGER_CONTROL_COMMAND_NONE;
  ProgramManager_Control_PostRunStruct *dataHierachy;
  bool stateTransit = false;

  ProgramManager_Control_RetrieveCommand(&command);

  /* Check command */
  switch (command)
  {
#if 0
    case PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME:
    {
      ProgramManager_Control_TogglePauseResumeHandler();

      break;
    }
#endif
    case PROGRAMMANAGER_CONTROL_COMMAND_STOP:
    {
      dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_POST_RUN);

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
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_POST_RUN);
      }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {
      stateTransit = ProgramManager_Control_CheckPrevStepAvailable();

      if (stateTransit == true)
      {
        dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
        dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT;
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_POST_RUN);
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
static void ProgramManager_AutoRunExtract_InternalCheckMotorCondition(void)
{
  if ((ProgramManager_AutoRunExtract_MotorCounter >= ProgramManager_AutoRunExtract_MotorCounterMax) && \
      (ProgramManager_AutoRunExtract_ExtractLevel < PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_MAX))
  {
    ProgramManager_AutoRunExtract_MotorCounter = (uint32_t)0U;

    ProgramManager_AutoRunExtract_MotorState += (uint32_t)1U;

    if (ProgramManager_AutoRunExtract_MotorState >= PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_MAX)
    {
      /* One phase completed */
      ProgramManager_AutoRunExtract_ExtractLevel += (uint32_t)1U;

      ProgramManager_AutoRunExtract_MotorState = PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_FWD;
    }

    switch (ProgramManager_AutoRunExtract_MotorState)
    {
      case PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_FWD:
      {
        switch (ProgramManager_AutoRunExtract_ExtractLevel)
        {
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_FIRSTRUN:
            ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.fwdRunTime;
            ProgramManager_AutoRunExtract_MotorSpeed = ProgramManager_gParamConfig.extractCfg.fwdRunSpeed;
            break;
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_MID:
            ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.midExtractTime;
            ProgramManager_AutoRunExtract_MotorSpeed = ProgramManager_gParamConfig.extractCfg.midExtractSpeed;
            break;
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH1:
            ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.highExtractTime1;
            ProgramManager_AutoRunExtract_MotorSpeed = ProgramManager_gParamConfig.extractCfg.highExtractSpeed1;
            break;
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH2:
            ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.highExtractTime2;
            ProgramManager_AutoRunExtract_MotorSpeed = ProgramManager_gParamConfig.extractCfg.highExtractSpeed2;
            break;
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH3:
            ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.highExtractTime3;
            ProgramManager_AutoRunExtract_MotorSpeed = ProgramManager_gParamConfig.extractCfg.highExtractSpeed3;
            break;
          default:
            ProgramManager_AutoRunExtract_MotorCounterMax = (uint32_t)0U;
            break;
        }

        break;
      }
      case PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_STOP:
      default:
      {
        switch (ProgramManager_AutoRunExtract_ExtractLevel)
        {
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_FIRSTRUN:
            ProgramManager_AutoRunExtract_MotorCounterMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_FWDRUNTIME_NORM;
            break;
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_MID:
            ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.midExtractDelayTime;
            break;
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH1:
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH2:
          case PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_HIGH3:
            ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.highExtractDelayTime;
            break;
          default:
            ProgramManager_AutoRunExtract_MotorCounterMax = (uint32_t)0U;
            break;
        }

        ProgramManager_AutoRunExtract_MotorSpeed = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;

        break;
      }
    }
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoRunExtract_InternalCheckStateTransit(void)
{
  bool conditionOk = (bool)true;
  ProgramManager_Control_PostRunStruct *dataHierachy;

  if (ProgramManager_Control_NotPauseAndError())
  {
    /* Maximum of number of extract step reached */
    if (ProgramManager_AutoRunExtract_ExtractLevel < PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_MAX)
    {
      conditionOk = (bool)false;
    }

    if (conditionOk == (bool)true)
    {
      dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_EXTRACT;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNEXTRACT_EVENT_POST_RUN);
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunExtract_InternalControlOutput(void)
{
  /* Turn off heat generator */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);

  /* Turn off water */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_WATER_MASK);

  /* Turn off all soaps */
  ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_1_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_2_MASK | PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_3_MASK);

  /* Control motor */
  if (ProgramManager_AutoRunExtract_MotorState == PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_FWD)
  {
    ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK);
  }
  else
  {
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK);
  }

  ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK, \
                                      (uint16_t)ProgramManager_AutoRunExtract_MotorSpeed << PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_OFFSET);

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
static Fsm_GuardType ProgramManager_AutoRunExtract_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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
    ProgramManager_InternalDataPush(PROGRAMMANAGER_AUTORUNEXTRACT_INTERNALDATALENGTH);

    ProgramManager_AutoRunExtract_OneSecondElapsed = (uint32_t)0U;

    ProgramManager_AutoRunExtract_ExtractLevel = PROGRAMMANAGER_AUTORUNEXTRACT_EXTRACTLEVEL_FIRSTRUN;

    ProgramManager_AutoRunExtract_MotorState = PROGRAMMANAGER_AUTORUNEXTRACT_MOTORSTATE_FWD;
    ProgramManager_AutoRunExtract_MotorCounter = (uint32_t)0U;
    ProgramManager_AutoRunExtract_MotorCounterMax = ProgramManager_gParamConfig.extractCfg.fwdRunTime;
    ProgramManager_AutoRunExtract_MotorSpeed = ProgramManager_gParamConfig.extractCfg.fwdRunSpeed;

    ProgramManager_SubMainFunctionPush(ProgramManager_AutoRunExtract_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_AutoRunExtract_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoRunExtract_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  /* Free internal data */
  ProgramManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static void ProgramManager_AutoRunExtract_InternalUpdateLcdParams(void)
{
  uint32_t timeTemp;

  timeTemp = ProgramManager_AutoRunExtract_MotorCounterMax - ProgramManager_AutoRunExtract_MotorCounter;

  ProgramManager_gTimeCountMin = timeTemp / (uint32_t)60U;
  ProgramManager_gTimeCountSec = timeTemp % (uint32_t)60U;

  ProgramManager_gSpinIndex = (uint8_t)ProgramManager_AutoRunExtract_ExtractLevel;
  ProgramManager_gMotorState = (uint8_t)ProgramManager_AutoRunExtract_MotorState;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunExtract_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoRunExtract_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoRunExtract_InternalCheckMotorCondition();

    ProgramManager_AutoRunExtract_InternalControlOutput();

    ProgramManager_AutoRunExtract_InternalUpdateLcdParams();

    ProgramManager_AutoRunExtract_InternalCheckStateTransit();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunExtract_SubTickHandler(void)
{
  ProgramManager_AutoRunExtract_OneSecondElapsed += (uint32_t)1U;

  if (ProgramManager_AutoRunExtract_OneSecondElapsed >= PROGRAMMANAGER_AUTORUNEXTRACT_ONESECONDELAPSED_MAX)
  {
    ProgramManager_AutoRunExtract_OneSecondElapsed = (uint32_t)0U;

    ProgramManager_AutoRunExtract_MotorCounter += (uint32_t)1U;
  }
}



#ifdef  __cplusplus
}
#endif
