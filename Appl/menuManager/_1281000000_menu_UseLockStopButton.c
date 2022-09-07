/* 
 * File:   _1281000000_menu_UseLockStopButton.c
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
#define MENUMANAGER_USELOCKSTOPBUTTON_INTERNALDATALENGTH              (uint8_t)4U

#define MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_IDX               0U
#define MENUMANAGER_USELOCKSTOPBUTTON_COUNTER_IDX                     1U
#define MENUMANAGER_USELOCKSTOPBUTTON_LISTINDEX_IDX                   2U
#define MENUMANAGER_USELOCKSTOPBUTTON_CURPOS_IDX                      3U

#define MenuManager_UseLockStopButton_InternalState                   MenuManager_GetInternalDataPtr(MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_IDX)
#define MenuManager_UseLockStopButton_Counter                         MenuManager_GetInternalDataPtr(MENUMANAGER_USELOCKSTOPBUTTON_COUNTER_IDX)
#define MenuManager_UseLockStopButton_ListIndex                       MenuManager_GetInternalDataPtr(MENUMANAGER_USELOCKSTOPBUTTON_LISTINDEX_IDX)
#define MenuManager_UseLockStopButton_CurPos                          MenuManager_GetInternalDataPtr(MENUMANAGER_USELOCKSTOPBUTTON_CURPOS_IDX)



#define MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_READY             (uint32_t)0U
#define MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_RUNNING           (uint32_t)1U
#define MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_DONE              (uint32_t)2U
#define MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_EXITING           (uint32_t)3U

#define MENUMANAGER_USELOCKSTOPBUTTON_COUNTER_MAX                     (uint32_t)50U

#define MENUMANAGER_USELOCKSTOPBUTTON_LISTINDEX_DISABLE               (uint32_t)0U
#define MENUMANAGER_USELOCKSTOPBUTTON_LISTINDEX_ENABLE                (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  bool                        mapValue;
} MenuManager_UseLockStopButton_IntValMapStruct;

static MenuManager_UseLockStopButton_IntValMapStruct MenuManager_UseLockStopButton_IntValMapConf[2] =
{
  { MENUMANAGER_USELOCKSTOPBUTTON_LISTINDEX_DISABLE,                  false                                   },
  { MENUMANAGER_USELOCKSTOPBUTTON_LISTINDEX_ENABLE,                   true                                    }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_UseLockStopButton_MainTitleStr[] =   "LOCK STOP BUT";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_UseLockStopButton_ChildMenu[2] =
{
  { &MenuManager_Common_DisableStr,                                   MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_EnableStr,                                    MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_UseLockStopButton_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_UseLockStopButton_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_UseLockStopButton_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_UseLockStopButton_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_UseLockStopButton_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_UseLockStopButton_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UseLockStopButton_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UseLockStopButton_Submenu1               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UseLockStopButton_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UseLockStopButton_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UseLockStopButton_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_UseLockStopButton_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_UseLockStopButton_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_UseLockStopButton_Entry                                             ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_UseLockStopButton_Exit                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_UseLockStopButton_Submenu1, MENUMANAGER_STATE_DOOR_LOCK_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_UseLockStopButton_StartBut                                          ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_UseLockStopButton_StopBut,  MENUMANAGER_STATE_DOOR_LOCK_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_UseLockStopButton_UpBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_UseLockStopButton_DownBut                                           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_UseLockStopButton_LcdShowMainTitle(void);
static void MenuManager_UseLockStopButton_LcdShowList(void);
static void MenuManager_UseLockStopButton_LcdShowDone(void);

static void MenuManager_UseLockStopButton_SubMainFunction(void);
static void MenuManager_UseLockStopButton_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_UseLockStopButton_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_UseLockStopButton_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_UseLockStopButton_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_UseLockStopButton_ListIndex, MenuManager_UseLockStopButton_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_UseLockStopButton_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_UseLockStopButton_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_UseLockStopButton_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_UseLockStopButton_ListIndex, MenuManager_UseLockStopButton_CurPos, MenuManager_UseLockStopButton_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_UseLockStopButton_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_UseLockStopButton_ChildMenuConf.childMenuCfg))[MenuManager_UseLockStopButton_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_UseLockStopButton_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_UseLockStopButton_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_UseLockStopButton_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_UseLockStopButton_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_UseLockStopButton_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DOOR_LOCK_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_USELOCKSTOPBUTTON_INTERNALDATALENGTH);

      MenuManager_UseLockStopButton_InternalState = MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_READY;
      MenuManager_UseLockStopButton_Counter = (uint32_t)0U;
      
      for ( MenuManager_UseLockStopButton_ListIndex = (uint32_t)0U; \
            MenuManager_UseLockStopButton_ListIndex < MenuManager_UseLockStopButton_ChildMenuConf.childMenuNum; \
            MenuManager_UseLockStopButton_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_UseLockStopButton_IntValMapConf[MenuManager_UseLockStopButton_ListIndex].mapValue == ProgramManager_gParamConfig.doorLockCfg.useLockStopButton)
        {
          break;
        }
      }
      
      MenuManager_UseLockStopButton_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_UseLockStopButton_ListIndex, MenuManager_UseLockStopButton_ChildMenuConf.childMenuNum);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_UseLockStopButton_LcdShowMainTitle();
    MenuManager_UseLockStopButton_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UseLockStopButton_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_USE_LOCK_STOP_BUTTON;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UseLockStopButton_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UseLockStopButton_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UseLockStopButton_InternalState == MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_READY)
  {
    MenuManager_UseLockStopButton_InternalState = MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UseLockStopButton_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UseLockStopButton_InternalState != MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UseLockStopButton_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UseLockStopButton_ListIndex > (uint32_t)0U)
  {
    MenuManager_UseLockStopButton_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_UseLockStopButton_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_UseLockStopButton_CurPos -= (uint32_t)1U;
  }

  MenuManager_UseLockStopButton_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_UseLockStopButton_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_UseLockStopButton_ListIndex < ((uint32_t)(MenuManager_UseLockStopButton_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_UseLockStopButton_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_UseLockStopButton_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_UseLockStopButton_ChildMenuConf.childMenuNum))
  {
    MenuManager_UseLockStopButton_CurPos += (uint32_t)1U;
  }
  
  MenuManager_UseLockStopButton_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_UseLockStopButton_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_UseLockStopButton_ButEventMapConf);

  switch (MenuManager_UseLockStopButton_InternalState)
  {
    case MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_RUNNING:
    {
      ProgramManager_DoorLockSetup_UseLockStopButton_SetData(&(MenuManager_UseLockStopButton_IntValMapConf[MenuManager_UseLockStopButton_ListIndex].mapValue));

      ProgramManager_DoorLockSetup_UseLockStopButton_GetData(&(ProgramManager_gParamConfig.doorLockCfg.useLockStopButton));

      MenuManager_UseLockStopButton_InternalState = MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_DONE:
    {
      MenuManager_UseLockStopButton_LcdShowDone();

      MenuManager_UseLockStopButton_InternalState = MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_UseLockStopButton_SubTickHandler(void)
{
  if (MenuManager_UseLockStopButton_InternalState == MENUMANAGER_USELOCKSTOPBUTTON_INTERNALSTATE_EXITING)
  {
    MenuManager_UseLockStopButton_Counter += (uint32_t)1U;

    if (MenuManager_UseLockStopButton_Counter >= MENUMANAGER_USELOCKSTOPBUTTON_COUNTER_MAX)
    {
      MenuManager_UseLockStopButton_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
