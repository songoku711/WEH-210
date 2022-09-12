/* 
 * File:   _B000000000_menu_Init.c
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

#include "menuManager.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/** Menu manager internal data definitions */
#define MENUMANAGER_INIT_COUNTDOWN_MAX                                (uint32_t)100U

#define MENUMANAGER_INIT_INTERNALDATALENGTH                           (uint8_t)2U

#define MENUMANAGER_INIT_COUNTDOWN_IDX                                0U
#define MENUMANAGER_INIT_INITSTATE_IDX                                1U

#define MenuManager_Init_Countdown                                    MenuManager_GetInternalDataPtr(MENUMANAGER_INIT_COUNTDOWN_IDX)
#define MenuManager_Init_InitState                                    MenuManager_GetInternalDataPtr(MENUMANAGER_INIT_INITSTATE_IDX)

#define MENUMANAGER_INIT_INITSTATE_READY                              (uint32_t)0U
#define MENUMANAGER_INIT_INITSTATE_WAIT_IO                            (uint32_t)1U
#define MENUMANAGER_INIT_INITSTATE_WAIT_PROG                          (uint32_t)2U
#define MENUMANAGER_INIT_INITSTATE_DONE                               (uint32_t)3U
#define MENUMANAGER_INIT_INITSTATE_EXITING                            (uint32_t)4U
#define MENUMANAGER_INIT_INITSTATE_FAIL                               (uint32_t)255U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_Init_InitializeStr[] =               "INITIALIZING";



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_Init_Entry                           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Init_Exit                            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_Init_StateMachine[3] =
{
  FSM_TRIGGER_ENTRY           (                                       MenuManager_Init_Entry                                                          ),
  FSM_TRIGGER_EXIT            (                                       MenuManager_Init_Exit                                                           ),
  FSM_TRIGGER_TRANSITION      ( MENUMANAGER_EVENT_NEXT,                                                       MENUMANAGER_STATE_HOME                  )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_Init_SubMainFunction(void);
static void MenuManager_Init_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static Fsm_GuardType MenuManager_Init_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  /* Allocate and initialize internal data value */
  MenuManager_InternalDataPush(MENUMANAGER_INIT_INTERNALDATALENGTH);

  MenuManager_Init_Countdown = (uint32_t)0U;
  MenuManager_Init_InitState = MENUMANAGER_INIT_INITSTATE_READY;

  /* Store pointer function */
  MenuManager_SubMainFunction = MenuManager_Init_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_Init_SubTickHandler;
  
  LCD_SetFont(LCD_FONT_SMALL);
  LCD_SetCursorPos(4, 4, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Init_InitializeStr);
  
  MenuManager_Common_LcdUpdateAll();
  
	return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Init_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  /* Free internal data */
  MenuManager_InternalDataPop();

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_Init_SubMainFunction(void)
{
  switch (MenuManager_Init_InitState)
  {
    case MENUMANAGER_INIT_INITSTATE_READY:
    {
      MenuManager_Init_InitState = MENUMANAGER_INIT_INITSTATE_WAIT_IO;
      break;
    }
    case MENUMANAGER_INIT_INITSTATE_WAIT_IO:
    {
      if (IoManager_GetCurrentState() >= IOMANAGER_STATE_INIT)
      {
        MenuManager_Init_InitState = MENUMANAGER_INIT_INITSTATE_WAIT_PROG;
      }

      break;
    }
    case MENUMANAGER_INIT_INITSTATE_WAIT_PROG:
    {
      if (ProgramManager_GetCurrentState() >= PROGRAMMANAGER_STATE_IDLE)
      {
        MenuManager_Init_InitState = MENUMANAGER_INIT_INITSTATE_DONE;
      }

      break;
    }
    case MENUMANAGER_INIT_INITSTATE_DONE:
    case MENUMANAGER_INIT_INITSTATE_FAIL:
    case MENUMANAGER_INIT_INITSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_Init_SubTickHandler(void)
{
  MenuManager_Init_Countdown += (uint32_t)1U;

  if (MenuManager_Init_Countdown >= MENUMANAGER_INIT_COUNTDOWN_MAX)
  {
    MenuManager_Init_Countdown = (uint32_t)0U;

    if (MenuManager_Init_InitState == MENUMANAGER_INIT_INITSTATE_DONE)
    {
      MenuManager_Init_InitState = MENUMANAGER_INIT_INITSTATE_EXITING;

      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_NEXT);
    }
  }
}



#ifdef  __cplusplus
}
#endif
