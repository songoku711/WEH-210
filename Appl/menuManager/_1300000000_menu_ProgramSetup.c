/* 
 * File:   _1300000000_menu_ProgramSetup.c
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
#define MENUMANAGER_PROGRAMSETUP_INTERNALDATALENGTH                   (uint8_t)2U

#define MENUMANAGER_PROGRAMSETUP_LISTINDEX_IDX                        0U
#define MENUMANAGER_PROGRAMSETUP_CURPOS_IDX                           1U

#define MenuManager_ProgramSetup_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMSETUP_LISTINDEX_IDX)
#define MenuManager_ProgramSetup_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_ProgramSetup_MainTitleStr[] =        "PROGRAM SETUP";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ProgramSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ProgramSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_ProgramSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_ProgramSetup_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramSetup_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramSetup_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramSetup_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramSetup_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_ProgramSetup_StateMachine[6] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ProgramSetup_Entry                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_PROGRAM_STEP_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ProgramSetup_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ProgramSetup_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ProgramSetup_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ProgramSetup_StopBut,       MENUMANAGER_STATE_MAIN_SETTING          )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ProgramSetup_LcdShowMainTitle(void);
static void MenuManager_ProgramSetup_LcdShowList(void);

static void MenuManager_ProgramSetup_SubMainFunction(void);
static void MenuManager_ProgramSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ProgramSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_ProgramSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ProgramSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  uint8_t  tempStr[20];
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_ProgramSetup_ListIndex, MenuManager_ProgramSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)PROGRAMMANAGER_SEQUENCE_NUM_MAX);
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    sprintf(tempStr, MenuManager_Common_ProgramStr, currentListIndex + 1U);

    MenuManager_Common_LcdShowListElementName
    (
      tempStr,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_ProgramSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_ProgramSetup_ListIndex, MenuManager_ProgramSetup_CurPos, PROGRAMMANAGER_SEQUENCE_NUM_MAX);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_ProgramSetup_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_ProgramSetup_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_STEP_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAIN_SETTING)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_PROGRAMSETUP_INTERNALDATALENGTH);

      MenuManager_ProgramSetup_ListIndex = (uint32_t)0U;
      MenuManager_ProgramSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_ProgramSetup_LcdShowMainTitle();
    MenuManager_ProgramSetup_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* dataHierachy;

  dataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_SETUP;
  dataHierachy->seqIdx = MenuManager_ProgramSetup_ListIndex;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ProgramSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_ProgramSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_ProgramSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_ProgramSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_ProgramSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ProgramSetup_ListIndex < ((uint32_t)(PROGRAMMANAGER_SEQUENCE_NUM_MAX) - (uint32_t)1U))
  {
    MenuManager_ProgramSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_ProgramSetup_CurPos < MenuManager_Common_LcdMaxCurPos((uint32_t)(PROGRAMMANAGER_SEQUENCE_NUM_MAX)))
  {
    MenuManager_ProgramSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_ProgramSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_ProgramSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ProgramSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_ProgramSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
