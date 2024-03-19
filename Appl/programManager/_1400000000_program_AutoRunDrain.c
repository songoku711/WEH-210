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

#define PROGRAMMANAGER_AUTORUNDRAIN_INTERNALDATALENGTH                (uint8_t)7U

#define PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_IDX              0U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_IDX                    1U
#define PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_IDX                    2U
#define PROGRAMMANAGER_AUTORUNDRAIN_MOTORCOUNTER_IDX                  3U
#define PROGRAMMANAGER_AUTORUNDRAIN_MOTORCOUNTERMAX_IDX               4U
#define PROGRAMMANAGER_AUTORUNDRAIN_MOTORSPEED_IDX                    5U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_IDX               6U

#define ProgramManager_AutoRunDrain_OneSecondElapsed                  ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_IDX)
#define ProgramManager_AutoRunDrain_DrainLevel                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_IDX)
#define ProgramManager_AutoRunDrain_MotorState                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_IDX)
#define ProgramManager_AutoRunDrain_MotorCounter                      ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_MOTORCOUNTER_IDX)
#define ProgramManager_AutoRunDrain_MotorCounterMax                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_MOTORCOUNTERMAX_IDX)
#define ProgramManager_AutoRunDrain_MotorSpeed                        ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_MOTORSPEED_IDX)
#define ProgramManager_AutoRunDrain_DrainValveState                   ProgramManager_GetInternalDataPtr(PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_IDX)

#define PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_MAX              (uint32_t)50U /* 50 x 20ms = 1s */

#define PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD                    0U
#define PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP                   1U

#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_CLOSE             0U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN              1U

#define PROGRAMMANAGER_AUTORUNDRAIN_EVENT_RUN_WASH                    PROGRAMMANAGER_EVENT_SUBMENU_1
#define PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN                    PROGRAMMANAGER_EVENT_SUBMENU_2

#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_FORWARDDRAIN           0U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_BALANCEDRAIN           1U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL1DRAIN          2U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL2DRAIN          3U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL3DRAIN          4U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL4DRAIN          5U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_OFFDRAIN               6U
#define PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_MAX                    7U

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoRunDrain_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoRunDrain_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoRunDrain_StateMachine[4] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoRunDrain_Entry                                               ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoRunDrain_Exit                                                ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNDRAIN_EVENT_RUN_WASH,                                   PROGRAMMANAGER_STATE_AUTO_RUN_WASH      ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN,                                   PROGRAMMANAGER_STATE_AUTO_POST_RUN      )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_AutoRunDrain_InternalCommandHandler(void);

static void ProgramManager_AutoRunDrain_InternalCheckMotorCondition(void);

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
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;
      dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN);

      stateTransit = true;

      break;
    }
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
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_SUBSTEP:
    {
      if (ProgramManager_AutoRunDrain_DrainLevel < PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_OFFDRAIN)
      {
        ProgramManager_AutoRunDrain_DrainLevel = PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_OFFDRAIN;

        ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainOffTime;
        ProgramManager_AutoRunDrain_MotorSpeed = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN;
      }
      // else
      // {
      //   stateTransit = ProgramManager_Control_CheckNextStepAvailable();

      //   if (stateTransit == true)
      //   {
      //     dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      //     dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;
      //     dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;

      //     ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      //     Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN);
      //   }
      // }

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_SUBSTEP:
    {
      if (ProgramManager_AutoRunDrain_DrainLevel < PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_OFFDRAIN)
      {
        runDataHierachy = (ProgramManager_Control_RunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_RunStruct));
        runDataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;

        ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)runDataHierachy;

        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_RUN_WASH);

        stateTransit = true;
      }
      else
      {
        ProgramManager_AutoRunDrain_DrainLevel = PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_FORWARDDRAIN;

        ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainForwardDrainTime;
        ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_CLOSE;
      }

      break;
    }
    default:
      break;
  }

  return stateTransit;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_InternalCheckMotorCondition(void)
{
  if ((ProgramManager_AutoRunDrain_MotorCounter >= ProgramManager_AutoRunDrain_MotorCounterMax) && \
      (ProgramManager_AutoRunDrain_DrainLevel < PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_MAX))
  {
    ProgramManager_AutoRunDrain_MotorCounter = (uint32_t)0U;

    /* One phase completed */
    ProgramManager_AutoRunDrain_DrainLevel += (uint32_t)1U;

    switch (ProgramManager_AutoRunDrain_DrainLevel)
    {
      case PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_FORWARDDRAIN:
        ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainForwardDrainTime;
        ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_CLOSE;
        break;
      case PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_BALANCEDRAIN:
        if (ProgramManager_gParamConfig.machineFuncCfg.washMachine == PROGRAMMANAGER_WASHING_MACHINE_MOTOR)
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        }
        else
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
        }
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainBalanceDrainTime;
        ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_CLOSE;
        break;
      case PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL1DRAIN:
        if (ProgramManager_gParamConfig.machineFuncCfg.washMachine == PROGRAMMANAGER_WASHING_MACHINE_MOTOR)
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        }
        else
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
        }
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainExtrLvl1DrainTime;
        ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_1;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN;
        break;
      case PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL2DRAIN:
        if (ProgramManager_gParamConfig.machineFuncCfg.washMachine == PROGRAMMANAGER_WASHING_MACHINE_MOTOR)
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        }
        else
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
        }
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainExtrLvl2DrainTime;
        ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_2;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN;
        break;
      case PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL3DRAIN:
        if (ProgramManager_gParamConfig.machineFuncCfg.washMachine == PROGRAMMANAGER_WASHING_MACHINE_MOTOR)
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        }
        else
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
        }
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainExtrLvl3DrainTime;
        ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_3;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN;
        break;
      case PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_EXTRLVL4DRAIN:
        if (ProgramManager_gParamConfig.machineFuncCfg.washMachine == PROGRAMMANAGER_WASHING_MACHINE_MOTOR)
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        }
        else
        {
          ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
        }
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainExtrLvl4DrainTime;
        ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_4;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN;
        break;
      case PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_OFFDRAIN:
        ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainOffTime;
        ProgramManager_AutoRunDrain_MotorSpeed = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN;
        break;
      default:
        ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_STOP;
        ProgramManager_AutoRunDrain_MotorCounterMax = (uint32_t)0U;
        ProgramManager_AutoRunDrain_MotorSpeed = (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;
        ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN;
        break;
    }
  }
}



/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_InternalCheckStateTransit(void)
{
  bool conditionOk = (bool)true;
  ProgramManager_Control_PostRunStruct *dataHierachy;

  if (ProgramManager_Control_NotPaused())
  {
    /* Maximum of number of extract step reached */
    if (ProgramManager_AutoRunDrain_DrainLevel < PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_MAX)
    {
      conditionOk = (bool)false;
    }

    if (conditionOk == (bool)true)
    {
      dataHierachy = (ProgramManager_Control_PostRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PostRunStruct));
      dataHierachy->dataId = PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN;
      
      if ((bool)false == ProgramManager_Control_CheckNextStepAvailable())
      {
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_STOP;
      }
      else
      {
        dataHierachy->command = PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP;
      }

      ProgramManager_FsmContext.dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

      Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_AUTORUNDRAIN_EVENT_POST_RUN);
    }
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_InternalControlOutput(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    /* Turn off heat generator */
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_HEAT_MASK);

    /* Turn off water */
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_WATER_MASK);

    /* Control soap - always off */
    ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_SOAP_MASK);

    /* Control motor */
    if (ProgramManager_AutoRunDrain_MotorState == PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD)
    {
      ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK, PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_FWD_MASK);
    }
    else
    {
      ProgramManager_Control_ClearOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_DIR_MASK);
    }

    if (ProgramManager_gParamConfig.machineFuncCfg.washMachine == PROGRAMMANAGER_WASHING_MACHINE_MOTOR)
    {
      ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK, \
                                          (((ProgramManager_AutoRunDrain_MotorSpeed == (uint32_t)PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0) ? (uint16_t)0U : ((uint16_t)1U << (ProgramManager_AutoRunDrain_MotorSpeed - 1U))) << PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_OFFSET));
    }
    else
    {
      ProgramManager_Control_ModifyOutput(PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_MASK, \
                                          ((uint16_t)ProgramManager_AutoRunDrain_MotorSpeed << PROGRAMMANAGER_CONTROL_OUTPUT_MOTOR_SPEED_OFFSET));
    }

    /* Control drain valve */
    if (ProgramManager_AutoRunDrain_DrainValveState == PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_OPEN)
    {
      ProgramManager_Control_DrainOpenHandler();
    }
    else
    {
      ProgramManager_Control_DrainCloseHandler();
    }
  }
  else
  {
    ProgramManager_Control_ClearAllOutput();

    /* Control drain valve - always CLOSE */
    ProgramManager_Control_DrainCloseHandler();
  }
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

    ProgramManager_AutoRunDrain_DrainLevel = PROGRAMMANAGER_AUTORUNDRAIN_DRAINLEVEL_FORWARDDRAIN;

    ProgramManager_AutoRunDrain_MotorCounter = (uint32_t)0U;

    ProgramManager_AutoRunDrain_MotorState = PROGRAMMANAGER_AUTORUNDRAIN_MOTORSTATE_FWD;
    ProgramManager_AutoRunDrain_MotorCounterMax = ProgramManager_gCurrentDrainForwardDrainTime;
    ProgramManager_AutoRunDrain_MotorSpeed = PROGRAMMANAGER_MOTOR_SPEED_LEVEL_0;

    ProgramManager_AutoRunDrain_DrainValveState = PROGRAMMANAGER_AUTORUNDRAIN_DRAINVALVESTATE_CLOSE;

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
static void ProgramManager_AutoRunDrain_InternalUpdateLcdParams(void)
{
  uint32_t timeTemp;

  timeTemp = ProgramManager_AutoRunDrain_MotorCounterMax - ProgramManager_AutoRunDrain_MotorCounter;

  ProgramManager_gTimeCountMin = timeTemp / (uint32_t)60U;
  ProgramManager_gTimeCountSec = timeTemp % (uint32_t)60U;

  ProgramManager_gDrainLevel = (uint8_t)ProgramManager_AutoRunDrain_DrainLevel;

  ProgramManager_gMotorState = (uint8_t)ProgramManager_AutoRunDrain_MotorState;
}



/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_AutoRunDrain_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_AutoRunDrain_InternalCheckMotorCondition();

    ProgramManager_AutoRunDrain_InternalControlOutput();

    ProgramManager_AutoRunDrain_InternalUpdateLcdParams();

    ProgramManager_AutoRunDrain_InternalCheckStateTransit();
  }
}

/*=============================================================================================*/
static void ProgramManager_AutoRunDrain_SubTickHandler(void)
{
  if (ProgramManager_Control_NotPaused())
  {
    ProgramManager_AutoRunDrain_OneSecondElapsed += (uint32_t)1U;

    if (ProgramManager_AutoRunDrain_OneSecondElapsed >= PROGRAMMANAGER_AUTORUNDRAIN_ONESECONDELAPSED_MAX)
    {
      ProgramManager_AutoRunDrain_OneSecondElapsed = (uint32_t)0U;

      ProgramManager_AutoRunDrain_MotorCounter += (uint32_t)1U;
    }
  }
  else
  {
    ProgramManager_AutoRunDrain_OneSecondElapsed = (uint32_t)0U;
  }
}



#ifdef  __cplusplus
}
#endif
