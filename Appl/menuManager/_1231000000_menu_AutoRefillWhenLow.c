/* 
 * File:   _1231000000_menu_AutoRefillWhenLow.c
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
#define MENUMANAGER_AUTOREFILLWHENLOW_INTERNALDATALENGTH              (uint8_t)4U

#define MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_IDX               0U
#define MENUMANAGER_AUTOREFILLWHENLOW_COUNTER_IDX                     1U
#define MENUMANAGER_AUTOREFILLWHENLOW_LISTINDEX_IDX                   2U
#define MENUMANAGER_AUTOREFILLWHENLOW_CURPOS_IDX                      3U

#define MenuManager_AutoRefillWhenLow_InternalState                   MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_IDX)
#define MenuManager_AutoRefillWhenLow_Counter                         MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREFILLWHENLOW_COUNTER_IDX)
#define MenuManager_AutoRefillWhenLow_ListIndex                       MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREFILLWHENLOW_LISTINDEX_IDX)
#define MenuManager_AutoRefillWhenLow_CurPos                          MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREFILLWHENLOW_CURPOS_IDX)



#define MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_READY             (uint32_t)0U
#define MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_RUNNING           (uint32_t)1U
#define MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_DONE              (uint32_t)2U
#define MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_EXITING           (uint32_t)3U

#define MENUMANAGER_AUTOREFILLWHENLOW_COUNTER_MAX                     (uint32_t)50U

#define MENUMANAGER_AUTOREFILLWHENLOW_LISTINDEX_DISABLE               (uint32_t)0U
#define MENUMANAGER_AUTOREFILLWHENLOW_LISTINDEX_ENABLE                (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  bool                        mapValue;
} MenuManager_AutoRefillWhenLow_IntValMapStruct;

static MenuManager_AutoRefillWhenLow_IntValMapStruct MenuManager_AutoRefillWhenLow_IntValMapConf[2] =
{
  { MENUMANAGER_AUTOREFILLWHENLOW_LISTINDEX_DISABLE,                  false                                   },
  { MENUMANAGER_AUTOREFILLWHENLOW_LISTINDEX_ENABLE,                   true                                    }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_AutoRefillWhenLow_MainTitleStr[] =   "AUTO REFILL";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_AutoRefillWhenLow_ChildMenu[2] =
{
  { &MenuManager_Common_DisableStr,                                   MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_EnableStr,                                    MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_AutoRefillWhenLow_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_AutoRefillWhenLow_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_AutoRefillWhenLow_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_AutoRefillWhenLow_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_AutoRefillWhenLow_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_AutoRefillWhenLow_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_AutoRefillWhenLow_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_AutoRefillWhenLow_Submenu1               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_AutoRefillWhenLow_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_AutoRefillWhenLow_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_AutoRefillWhenLow_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_AutoRefillWhenLow_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_AutoRefillWhenLow_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_AutoRefillWhenLow_Entry                                             ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_AutoRefillWhenLow_Exit                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_AutoRefillWhenLow_Submenu1, MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_AutoRefillWhenLow_StartBut                                          ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_AutoRefillWhenLow_StopBut,  MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_AutoRefillWhenLow_UpBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_AutoRefillWhenLow_DownBut                                           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_AutoRefillWhenLow_LcdShowMainTitle(void);
static void MenuManager_AutoRefillWhenLow_LcdShowList(void);
static void MenuManager_AutoRefillWhenLow_LcdShowDone(void);

static void MenuManager_AutoRefillWhenLow_SubMainFunction(void);
static void MenuManager_AutoRefillWhenLow_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_AutoRefillWhenLow_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_AutoRefillWhenLow_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_AutoRefillWhenLow_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_AutoRefillWhenLow_ListIndex, MenuManager_AutoRefillWhenLow_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_AutoRefillWhenLow_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_AutoRefillWhenLow_ListIndex, MenuManager_AutoRefillWhenLow_CurPos, MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_AutoRefillWhenLow_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuCfg))[MenuManager_AutoRefillWhenLow_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_AutoRefillWhenLow_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_AutoRefillWhenLow_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_AutoRefillWhenLow_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_AutoRefillWhenLow_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_AutoRefillWhenLow_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_AUTOREFILLWHENLOW_INTERNALDATALENGTH);

      MenuManager_AutoRefillWhenLow_InternalState = MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_READY;
      MenuManager_AutoRefillWhenLow_Counter = (uint32_t)0U;
      
      for ( MenuManager_AutoRefillWhenLow_ListIndex = (uint32_t)0U; \
            MenuManager_AutoRefillWhenLow_ListIndex < MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuNum; \
            MenuManager_AutoRefillWhenLow_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_AutoRefillWhenLow_IntValMapConf[MenuManager_AutoRefillWhenLow_ListIndex].mapValue == ProgramManager_gParamConfig.fillLevelCfg.autoRefillWhenLow)
        {
          break;
        }
      }
      
      MenuManager_AutoRefillWhenLow_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_AutoRefillWhenLow_ListIndex, MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuNum);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_AutoRefillWhenLow_LcdShowMainTitle();
    MenuManager_AutoRefillWhenLow_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_AutoRefillWhenLow_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_AUTO_REFILL_WHEN_LOW;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_AutoRefillWhenLow_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_AutoRefillWhenLow_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_AutoRefillWhenLow_InternalState == MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_READY)
  {
    MenuManager_AutoRefillWhenLow_InternalState = MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_AutoRefillWhenLow_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_AutoRefillWhenLow_InternalState != MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_AutoRefillWhenLow_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_AutoRefillWhenLow_ListIndex > (uint32_t)0U)
  {
    MenuManager_AutoRefillWhenLow_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_AutoRefillWhenLow_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_AutoRefillWhenLow_CurPos -= (uint32_t)1U;
  }

  MenuManager_AutoRefillWhenLow_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_AutoRefillWhenLow_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_AutoRefillWhenLow_ListIndex < ((uint32_t)(MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_AutoRefillWhenLow_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_AutoRefillWhenLow_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_AutoRefillWhenLow_ChildMenuConf.childMenuNum))
  {
    MenuManager_AutoRefillWhenLow_CurPos += (uint32_t)1U;
  }
  
  MenuManager_AutoRefillWhenLow_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_AutoRefillWhenLow_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_AutoRefillWhenLow_ButEventMapConf);

  switch (MenuManager_AutoRefillWhenLow_InternalState)
  {
    case MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_RUNNING:
    {
      ProgramManager_FillLevelSetup_AutoRefillWhenLow_SetData(&(MenuManager_AutoRefillWhenLow_IntValMapConf[MenuManager_AutoRefillWhenLow_ListIndex].mapValue));

      ProgramManager_FillLevelSetup_AutoRefillWhenLow_GetData(&(ProgramManager_gParamConfig.fillLevelCfg.autoRefillWhenLow));

      MenuManager_AutoRefillWhenLow_InternalState = MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_DONE:
    {
      MenuManager_AutoRefillWhenLow_LcdShowDone();

      MenuManager_AutoRefillWhenLow_InternalState = MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_AutoRefillWhenLow_SubTickHandler(void)
{
  if (MenuManager_AutoRefillWhenLow_InternalState == MENUMANAGER_AUTOREFILLWHENLOW_INTERNALSTATE_EXITING)
  {
    MenuManager_AutoRefillWhenLow_Counter += (uint32_t)1U;

    if (MenuManager_AutoRefillWhenLow_Counter >= MENUMANAGER_AUTOREFILLWHENLOW_COUNTER_MAX)
    {
      MenuManager_AutoRefillWhenLow_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
