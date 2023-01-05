/* 
 * File:   _0000000000_program_Idle.c
 * Author: Long
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

#define PROGRAMMANAGER_IDLE_EVENT_PRE_RUN                             PROGRAMMANAGER_EVENT_SUBMENU_1

/** Program manager event handlers */
static Fsm_GuardType ProgramManager_Idle_Entry                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_Idle_Exit                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_Idle_StateMachine[3] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_Idle_Entry                                                       ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_Idle_Exit                                                        ),
  FSM_TRIGGER_TRANSITION      ( PROGRAMMANAGER_IDLE_EVENT_PRE_RUN,                                            PROGRAMMANAGER_STATE_AUTO_PRE_RUN       )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static bool ProgramManager_Idle_InternalCommandHandler(void);

static void ProgramManager_Idle_InternalControlOutput(void);

static void ProgramManager_Idle_SubMainFunction(void);
static void ProgramManager_Idle_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static bool ProgramManager_Idle_InternalCommandHandler(void)
{
  uint8_t command = PROGRAMMANAGER_CONTROL_COMMAND_NONE;
  bool stateTransit = false;

  ProgramManager_Control_RetrieveCommand(&command);

  switch (command)
  {
    case PROGRAMMANAGER_CONTROL_COMMAND_START:
    {
      if (((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == true) && \
          (ProgramManager_gSensorDoorOpenErr == (uint8_t)0U))
      {
        Fsm_TriggerEvent(&ProgramManager_FsmContext, (Fsm_EventType)PROGRAMMANAGER_IDLE_EVENT_PRE_RUN);

        stateTransit = (bool)true;
      }

      break;
    }
    default:
      break;
  }

  return stateTransit;
}



/*=============================================================================================*/
static void ProgramManager_Idle_InternalControlOutput(void)
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
static Fsm_GuardType ProgramManager_Idle_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_AUTO_POST_RUN)
    {
      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == PROGRAMMANAGER_STATE_INIT)
    {
      /* Release previous state data hierachy */
      ProgramManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      ProgramManager_SubMainFunctionPush(ProgramManager_Control_TxRxSignalSubMainFunction);
      ProgramManager_SubMainFunctionPush(ProgramManager_Control_AnalyzeDataSubMainFunction);
      ProgramManager_SubMainFunctionPush(ProgramManager_Control_UpdateThresholdSubMainFunction);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    ProgramManager_SubMainFunctionPush(ProgramManager_Idle_SubMainFunction);
    ProgramManager_SubTickHandler = ProgramManager_Idle_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_Idle_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_Control_PreRunStruct* dataHierachy;

  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  dataHierachy = (ProgramManager_Control_PreRunStruct *)ProgramManager_malloc(sizeof(ProgramManager_Control_PreRunStruct));
  dataHierachy->dataId = PROGRAMMANAGER_STATE_IDLE;
  dataHierachy->executeStep = ProgramManager_gAutoSeqConfig.currentStep;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_Idle_SubMainFunction(void)
{
  bool stateTransit;

  stateTransit = ProgramManager_Idle_InternalCommandHandler();

  if (stateTransit == false)
  {
    ProgramManager_Idle_InternalControlOutput();
  }
}

/*=============================================================================================*/
static void ProgramManager_Idle_SubTickHandler(void)
{

}



#ifdef  __cplusplus
}
#endif
