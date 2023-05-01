/* 
 * File:   _1200000000_menu_MachineSetup.c
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
#define MENUMANAGER_MACHINESETUP_INTERNALDATALENGTH                   (uint8_t)2U

#define MENUMANAGER_MACHINESETUP_LISTINDEX_IDX                        0U
#define MENUMANAGER_MACHINESETUP_CURPOS_IDX                           1U

#define MenuManager_MachineSetup_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_MACHINESETUP_LISTINDEX_IDX)
#define MenuManager_MachineSetup_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_MACHINESETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MachineFuncSetup_ChildTitleStr[] =   "MACHINE FUNCTION";
static const uint8_t MenuManager_InputStatusSetup_ChildTitleStr[] =   "INPUT STATUS";
static const uint8_t MenuManager_FillLevelSetup_ChildTitleStr[] =     "FILL AND LEVEL";
static const uint8_t MenuManager_HeatTempSetup_ChildTitleStr[] =      "HEAT AND TEMP";
static const uint8_t MenuManager_WashSetup_ChildTitleStr[] =          "WASH SETUP";
static const uint8_t MenuManager_DrainSetup_ChildTitleStr[] =         "DRAIN SETUP";
static const uint8_t MenuManager_DoorLockSetup_ChildTitleStr[] =      "DOOR AND LOCK";

static const uint8_t MenuManager_MachineSetup_MainTitleStr[] =        "MACHINE SETUP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_MachineSetup_ChildMenu[7] =
{
  { &MenuManager_MachineFuncSetup_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_InputStatusSetup_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_FillLevelSetup_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_HeatTempSetup_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_WashSetup_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_DrainSetup_ChildTitleStr,                            MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_DoorLockSetup_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_7             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_MachineSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)7U,
  .childMenuCfg               = &MenuManager_MachineSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MachineSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MachineSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_MachineSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_MachineSetup_Entry                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineSetup_StartBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineSetup_StopBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineSetup_UpBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineSetup_DownBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_MachineSetup_StateMachine[12] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MachineSetup_Entry                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_MACHINE_FUNC_SETUP    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_INPUT_STATUS_SETUP    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_FILL_LEVEL_SETUP      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_HEAT_TEMP_SETUP       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_WASH_SETUP            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_DRAIN_SETUP           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_DOOR_LOCK_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MachineSetup_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MachineSetup_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MachineSetup_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MachineSetup_StopBut,       MENUMANAGER_STATE_MAIN_SETTING          )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MachineSetup_LcdShowMainTitle(void);
static void MenuManager_MachineSetup_LcdShowList(void);

static void MenuManager_MachineSetup_SubMainFunction(void);
static void MenuManager_MachineSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MachineSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MachineSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MachineSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_MachineSetup_ListIndex, MenuManager_MachineSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_MachineSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_MachineSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_MachineSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_MachineSetup_ListIndex, MenuManager_MachineSetup_CurPos, MenuManager_MachineSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MACHINE_FUNC_SETUP) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_INPUT_STATUS_SETUP) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_LEVEL_SETUP)   || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HEAT_TEMP_SETUP)    || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_WASH_SETUP)         || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP)        || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DOOR_LOCK_SETUP))
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

      MenuManager_InternalDataPush(MENUMANAGER_MACHINESETUP_INTERNALDATALENGTH);

      MenuManager_MachineSetup_ListIndex = (uint32_t)0U;
      MenuManager_MachineSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
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
    MenuManager_MachineSetup_LcdShowMainTitle();
    MenuManager_MachineSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_MachineSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_MachineSetup_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MACHINE_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_MachineSetup_ChildMenuConf.childMenuCfg))[MenuManager_MachineSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MACHINE_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MachineSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_MachineSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_MachineSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_MachineSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_MachineSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MachineSetup_ListIndex < ((uint32_t)(MenuManager_MachineSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_MachineSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_MachineSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_MachineSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_MachineSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_MachineSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_MachineSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MachineSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_MachineSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
