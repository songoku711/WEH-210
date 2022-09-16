/* 
 * File:   _1210000000_menu_MachineFuncSetup.c
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
#define MENUMANAGER_MACHINEFUNCSETUP_INTERNALDATALENGTH               (uint8_t)2U

#define MENUMANAGER_MACHINEFUNCSETUP_LISTINDEX_IDX                    0U
#define MENUMANAGER_MACHINEFUNCSETUP_CURPOS_IDX                       1U

#define MenuManager_MachineFuncSetup_ListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_MACHINEFUNCSETUP_LISTINDEX_IDX)
#define MenuManager_MachineFuncSetup_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_MACHINEFUNCSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DrainWhileDoorOpen_ChildTitleStr[] = "DRAIN W DOOR OPEN";
static const uint8_t MenuManager_HeatUseTimeout_ChildTitleStr[] =     "HEAT USE TIMEOUT";
static const uint8_t MenuManager_FillUseTimeout_ChildTitleStr[] =     "FILL USE TIMEOUT";
static const uint8_t MenuManager_ManOperateWhenAuto_ChildTitleStr[] = "MAN OP WHEN AUTO";
static const uint8_t MenuManager_TempUnit_ChildTitleStr[] =           "TEMP UNIT";
static const uint8_t MenuManager_DrainValveStatus_ChildTitleStr[] =   "DRAIN VALVE STAT";

static const uint8_t MenuManager_MachineFuncSetup_MainTitleStr[] =    "MACHINE FUNCTION";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_MachineFuncSetup_ChildMenu[6] =
{
  { &MenuManager_DrainWhileDoorOpen_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_HeatUseTimeout_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_FillUseTimeout_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_ManOperateWhenAuto_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_TempUnit_ChildTitleStr,                              MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_DrainValveStatus_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_6             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_MachineFuncSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)6U,
  .childMenuCfg               = &MenuManager_MachineFuncSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MachineFuncSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MachineFuncSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_MachineFuncSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_MachineFuncSetup_Entry               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineFuncSetup_StartBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineFuncSetup_StopBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineFuncSetup_UpBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_MachineFuncSetup_DownBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_MachineFuncSetup_StateMachine[11] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MachineFuncSetup_Entry                                              ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_DRAIN_WHILE_DOOR_OPEN ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_HEAT_USE_TIMEOUT      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_FILL_USE_TIMEOUT      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_MAN_OPERATE_WHEN_AUTO ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_TEMP_UNIT             ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_DRAIN_VALVE_STATUS    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MachineFuncSetup_UpBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MachineFuncSetup_DownBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MachineFuncSetup_StartBut                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MachineFuncSetup_StopBut,   MENUMANAGER_STATE_MACHINE_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MachineFuncSetup_LcdShowMainTitle(void);
static void MenuManager_MachineFuncSetup_LcdShowList(void);

static void MenuManager_MachineFuncSetup_SubMainFunction(void);
static void MenuManager_MachineFuncSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MachineFuncSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MachineFuncSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MachineFuncSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_MachineFuncSetup_ListIndex, MenuManager_MachineFuncSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_MachineFuncSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_MachineFuncSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_MachineFuncSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_MachineFuncSetup_ListIndex, MenuManager_MachineFuncSetup_CurPos, MenuManager_MachineFuncSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineFuncSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_WHILE_DOOR_OPEN)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HEAT_USE_TIMEOUT)       || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FILL_USE_TIMEOUT)       || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAN_OPERATE_WHEN_AUTO)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_TEMP_UNIT)              || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_VALVE_STATUS))
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

      MenuManager_InternalDataPush(MENUMANAGER_MACHINEFUNCSETUP_INTERNALDATALENGTH);

      MenuManager_MachineFuncSetup_ListIndex = (uint32_t)0U;
      MenuManager_MachineFuncSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
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
    MenuManager_MachineFuncSetup_LcdShowMainTitle();
    MenuManager_MachineFuncSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_MachineFuncSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_MachineFuncSetup_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineFuncSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MACHINE_FUNC_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_MachineFuncSetup_ChildMenuConf.childMenuCfg))[MenuManager_MachineFuncSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineFuncSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_MACHINE_FUNC_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineFuncSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MachineFuncSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_MachineFuncSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_MachineFuncSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_MachineFuncSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_MachineFuncSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_MachineFuncSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_MachineFuncSetup_ListIndex < ((uint32_t)(MenuManager_MachineFuncSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_MachineFuncSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_MachineFuncSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_MachineFuncSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_MachineFuncSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_MachineFuncSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_MachineFuncSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MachineFuncSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_MachineFuncSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
