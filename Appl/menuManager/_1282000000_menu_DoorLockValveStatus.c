/* 
 * File:   _1282000000_menu_DoorLockValveStatus.c
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
#define MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALDATALENGTH            (uint8_t)4U

#define MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_IDX             0U
#define MENUMANAGER_DOORLOCKVALVESTATUS_COUNTER_IDX                   1U
#define MENUMANAGER_DOORLOCKVALVESTATUS_LISTINDEX_IDX                 2U
#define MENUMANAGER_DOORLOCKVALVESTATUS_CURPOS_IDX                    3U

#define MenuManager_DoorLockValveStatus_InternalState                 MenuManager_GetInternalDataPtr(MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_IDX)
#define MenuManager_DoorLockValveStatus_Counter                       MenuManager_GetInternalDataPtr(MENUMANAGER_DOORLOCKVALVESTATUS_COUNTER_IDX)
#define MenuManager_DoorLockValveStatus_ListIndex                     MenuManager_GetInternalDataPtr(MENUMANAGER_DOORLOCKVALVESTATUS_LISTINDEX_IDX)
#define MenuManager_DoorLockValveStatus_CurPos                        MenuManager_GetInternalDataPtr(MENUMANAGER_DOORLOCKVALVESTATUS_CURPOS_IDX)



#define MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_READY           (uint32_t)0U
#define MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_RUNNING         (uint32_t)1U
#define MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_DONE            (uint32_t)2U
#define MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_EXITING         (uint32_t)3U

#define MENUMANAGER_DOORLOCKVALVESTATUS_COUNTER_MAX                   (uint32_t)50U

#define MENUMANAGER_DOORLOCKVALVESTATUS_LISTINDEX_NO                  (uint32_t)0U
#define MENUMANAGER_DOORLOCKVALVESTATUS_LISTINDEX_NC                  (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_RelayEnableStatusType mapValue;
} MenuManager_DoorLockValveStatus_IntValMapStruct;

static MenuManager_DoorLockValveStatus_IntValMapStruct MenuManager_DoorLockValveStatus_IntValMapConf[2] =
{
  { MENUMANAGER_DOORLOCKVALVESTATUS_LISTINDEX_NO,                     PROGRAMMANAGER_RELAY_ENABLE_STAT_NO     },
  { MENUMANAGER_DOORLOCKVALVESTATUS_LISTINDEX_NC,                     PROGRAMMANAGER_RELAY_ENABLE_STAT_NC     }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DoorLockValveStatus_MainTitleStr[] = "DOOR LOCK STAT";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_DoorLockValveStatus_ChildMenu[2] =
{
  { &MenuManager_Common_NormalOpenStr,                                MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_NormalCloseStr,                               MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_DoorLockValveStatus_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_DoorLockValveStatus_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DoorLockValveStatus_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DoorLockValveStatus_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_DoorLockValveStatus_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DoorLockValveStatus_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockValveStatus_Exit                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockValveStatus_Submenu1             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockValveStatus_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockValveStatus_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockValveStatus_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockValveStatus_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DoorLockValveStatus_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DoorLockValveStatus_Entry                                           ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_DoorLockValveStatus_Exit                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_DoorLockValveStatus_Submenu1, MENUMANAGER_STATE_DOOR_LOCK_SETUP     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DoorLockValveStatus_StartBut                                        ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DoorLockValveStatus_StopBut,  MENUMANAGER_STATE_DOOR_LOCK_SETUP     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DoorLockValveStatus_UpBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DoorLockValveStatus_DownBut                                         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DoorLockValveStatus_LcdShowMainTitle(void);
static void MenuManager_DoorLockValveStatus_LcdShowList(void);
static void MenuManager_DoorLockValveStatus_LcdShowDone(void);

static void MenuManager_DoorLockValveStatus_SubMainFunction(void);
static void MenuManager_DoorLockValveStatus_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DoorLockValveStatus_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DoorLockValveStatus_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DoorLockValveStatus_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_DoorLockValveStatus_ListIndex, MenuManager_DoorLockValveStatus_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_DoorLockValveStatus_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_DoorLockValveStatus_ListIndex, MenuManager_DoorLockValveStatus_CurPos, MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_DoorLockValveStatus_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuCfg))[MenuManager_DoorLockValveStatus_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_DoorLockValveStatus_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_DoorLockValveStatus_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockValveStatus_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_DoorLockValveStatus_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_DoorLockValveStatus_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DOOR_LOCK_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALDATALENGTH);

      MenuManager_DoorLockValveStatus_InternalState = MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_READY;
      MenuManager_DoorLockValveStatus_Counter = (uint32_t)0U;
      
      for ( MenuManager_DoorLockValveStatus_ListIndex = (uint32_t)0U; \
            MenuManager_DoorLockValveStatus_ListIndex < MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuNum; \
            MenuManager_DoorLockValveStatus_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_DoorLockValveStatus_IntValMapConf[MenuManager_DoorLockValveStatus_ListIndex].mapValue == ProgramManager_gParamConfig.doorLockCfg.doorLockValveStatus)
        {
          break;
        }
      }
      
      MenuManager_DoorLockValveStatus_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_DoorLockValveStatus_ListIndex, MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuNum);
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_DoorLockValveStatus_LcdShowMainTitle();
    MenuManager_DoorLockValveStatus_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockValveStatus_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DOOR_LOCK_VALVE_STATUS;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockValveStatus_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockValveStatus_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DoorLockValveStatus_InternalState == MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_READY)
  {
    MenuManager_DoorLockValveStatus_InternalState = MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockValveStatus_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DoorLockValveStatus_InternalState != MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockValveStatus_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DoorLockValveStatus_ListIndex > (uint32_t)0U)
  {
    MenuManager_DoorLockValveStatus_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_DoorLockValveStatus_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_DoorLockValveStatus_CurPos -= (uint32_t)1U;
  }

  MenuManager_DoorLockValveStatus_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockValveStatus_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DoorLockValveStatus_ListIndex < ((uint32_t)(MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_DoorLockValveStatus_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_DoorLockValveStatus_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_DoorLockValveStatus_ChildMenuConf.childMenuNum))
  {
    MenuManager_DoorLockValveStatus_CurPos += (uint32_t)1U;
  }
  
  MenuManager_DoorLockValveStatus_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DoorLockValveStatus_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DoorLockValveStatus_ButEventMapConf);

  switch (MenuManager_DoorLockValveStatus_InternalState)
  {
    case MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_RUNNING:
    {
      ProgramManager_DoorLockSetup_DoorLockValveStatus_SetData(&(MenuManager_DoorLockValveStatus_IntValMapConf[MenuManager_DoorLockValveStatus_ListIndex].mapValue));

      ProgramManager_DoorLockSetup_DoorLockValveStatus_GetData(&(ProgramManager_gParamConfig.doorLockCfg.doorLockValveStatus));

      MenuManager_DoorLockValveStatus_InternalState = MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_DONE:
    {
      MenuManager_DoorLockValveStatus_LcdShowDone();

      MenuManager_DoorLockValveStatus_InternalState = MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_DoorLockValveStatus_SubTickHandler(void)
{
  if (MenuManager_DoorLockValveStatus_InternalState == MENUMANAGER_DOORLOCKVALVESTATUS_INTERNALSTATE_EXITING)
  {
    MenuManager_DoorLockValveStatus_Counter += (uint32_t)1U;

    if (MenuManager_DoorLockValveStatus_Counter >= MENUMANAGER_DOORLOCKVALVESTATUS_COUNTER_MAX)
    {
      MenuManager_DoorLockValveStatus_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
