/* 
 * File:   _1280000000_menu_DoorLockSetup.c
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
#define MENUMANAGER_DOORLOCKSETUP_INTERNALDATALENGTH                  (uint8_t)2U

#define MENUMANAGER_DOORLOCKSETUP_LISTINDEX_IDX                       0U
#define MENUMANAGER_DOORLOCKSETUP_CURPOS_IDX                          1U

#define MenuManager_DoorLockSetup_ListIndex                           MenuManager_GetInternalDataPtr(MENUMANAGER_DOORLOCKSETUP_LISTINDEX_IDX)
#define MenuManager_DoorLockSetup_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_DOORLOCKSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_UseLockStopButton_ChildTitleStr[] =    "USE LOCK STOP BUT";
static const uint8_t MenuManager_DoorLockValveStatus_ChildTitleStr[] =  "DOOR LOCK STAT";
static const uint8_t MenuManager_UnlockDoorTemp_ChildTitleStr[] =       "UNLOCK DOOR TEMP";
static const uint8_t MenuManager_UnlockDoorLevel_ChildTitleStr[] =      "UNLOCK DOOR LEVEL";

static const uint8_t MenuManager_DoorLockSetup_MainTitleStr[] =       "DOOR & LOCK";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_DoorLockSetup_ChildMenu[4] =
{
  { &MenuManager_UseLockStopButton_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_DoorLockValveStatus_ChildTitleStr,                   MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_UnlockDoorTemp_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_UnlockDoorLevel_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_4             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_DoorLockSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)4U,
  .childMenuCfg               = &MenuManager_DoorLockSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DoorLockSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DoorLockSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_DoorLockSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DoorLockSetup_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockSetup_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockSetup_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockSetup_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DoorLockSetup_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DoorLockSetup_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DoorLockSetup_Entry                                                 ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_USE_LOCK_STOP_BUTTON  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_DOOR_LOCK_VALVE_STATUS),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_UNLOCK_DOOR_TEMP      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_UNLOCK_DOOR_LEVEL     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DoorLockSetup_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DoorLockSetup_DownBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DoorLockSetup_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DoorLockSetup_StopBut,      MENUMANAGER_STATE_MACHINE_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DoorLockSetup_LcdShowMainTitle(void);
static void MenuManager_DoorLockSetup_LcdShowList(void);

static void MenuManager_DoorLockSetup_SubMainFunction(void);
static void MenuManager_DoorLockSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DoorLockSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DoorLockSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DoorLockSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_DoorLockSetup_ListIndex, MenuManager_DoorLockSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_DoorLockSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_DoorLockSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_DoorLockSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_DoorLockSetup_ListIndex, MenuManager_DoorLockSetup_CurPos, MenuManager_DoorLockSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_USE_LOCK_STOP_BUTTON)   || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DOOR_LOCK_VALVE_STATUS) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_UNLOCK_DOOR_TEMP)       || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_UNLOCK_DOOR_LEVEL))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MACHINE_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_DOORLOCKSETUP_INTERNALDATALENGTH);

      MenuManager_DoorLockSetup_ListIndex = (uint32_t)0U;
      MenuManager_DoorLockSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
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
    MenuManager_DoorLockSetup_LcdShowMainTitle();
    MenuManager_DoorLockSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_DoorLockSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_DoorLockSetup_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DOOR_LOCK_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_DoorLockSetup_ChildMenuConf.childMenuCfg))[MenuManager_DoorLockSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DOOR_LOCK_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DoorLockSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_DoorLockSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_DoorLockSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_DoorLockSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_DoorLockSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DoorLockSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DoorLockSetup_ListIndex < ((uint32_t)(MenuManager_DoorLockSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_DoorLockSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_DoorLockSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_DoorLockSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_DoorLockSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_DoorLockSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DoorLockSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DoorLockSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_DoorLockSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
