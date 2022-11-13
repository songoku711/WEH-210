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



/** Program manager event handlers */
static Fsm_GuardType ProgramManager_AutoPreRun_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType ProgramManager_AutoPreRun_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Program manager state machine */
Fsm_EventEntryStruct ProgramManager_AutoPreRun_StateMachine[2] =
{
  FSM_TRIGGER_ENTRY           (                                       ProgramManager_AutoPreRun_Entry                                                   ),
  FSM_TRIGGER_EXIT            (                                       ProgramManager_AutoPreRun_Exit                                                    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void ProgramManager_AutoPreRun_SubMainFunction(void);
static void ProgramManager_AutoPreRun_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static Fsm_GuardType ProgramManager_AutoPreRun_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPush(ProgramManager_AutoPreRun_SubMainFunction);
  ProgramManager_SubTickHandler = ProgramManager_AutoPreRun_SubTickHandler;

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType ProgramManager_AutoPreRun_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  ProgramManager_SubMainFunctionPop();
  ProgramManager_SubTickHandler = NULL;

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void ProgramManager_AutoPreRun_SubMainFunction(void)
{
  uint8_t command = PROGRAMMANAGER_CONTROL_COMMAND_NONE;

  ProgramManager_Control_RetrieveCommand(&command);

  switch (command)
  {
    case PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME:
    {
      ProgramManager_gIsPaused ^= (uint8_t)0x01U;

      break;
    }
    case PROGRAMMANAGER_CONTROL_COMMAND_STOP:
    {

    }
    case PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP:
    {

    }
    case PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP:
    {

    }
    default:
      break;
  }

  
}

/*=============================================================================================*/
static void ProgramManager_AutoPreRun_SubTickHandler(void)
{

}



#ifdef  __cplusplus
}
#endif
