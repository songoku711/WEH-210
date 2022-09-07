/* 
 * File:   _1220000000_menu_InputStatusSetup.c
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
#define MENUMANAGER_INPUTSTATUSSETUP_INTERNALDATALENGTH               (uint8_t)2U

#define MENUMANAGER_INPUTSTATUSSETUP_LISTINDEX_IDX                    0U
#define MENUMANAGER_INPUTSTATUSSETUP_CURPOS_IDX                       1U

#define MenuManager_InputStatusSetup_ListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_INPUTSTATUSSETUP_LISTINDEX_IDX)
#define MenuManager_InputStatusSetup_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_INPUTSTATUSSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DoorClosed_ChildTitleStr[] =         "DOOR CLOSED STAT";
static const uint8_t MenuManager_ExtractShock_ChildTitleStr[] =       "EXTRACT SHOCK";
static const uint8_t MenuManager_EmergencyStop_ChildTitleStr[] =      "EMERGENCY STOP";
static const uint8_t MenuManager_InverterError_ChildTitleStr[] =      "INVERTER ERROR";

static const uint8_t MenuManager_InputStatusSetup_MainTitleStr[] =    "INPUT STATUS";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_InputStatusSetup_ChildMenu[4] =
{
  { &MenuManager_DoorClosed_ChildTitleStr,                            MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_ExtractShock_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_EmergencyStop_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_InverterError_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_4             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_InputStatusSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)4U,
  .childMenuCfg               = &MenuManager_InputStatusSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_InputStatusSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_InputStatusSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_InputStatusSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_InputStatusSetup_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InputStatusSetup_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InputStatusSetup_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InputStatusSetup_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_InputStatusSetup_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_InputStatusSetup_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_InputStatusSetup_Entry                                              ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_DOOR_CLOSED           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_EXTRACT_SHOCK         ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_EMERGENCY_STOP        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_INVERTER_ERROR        ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_InputStatusSetup_UpBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_InputStatusSetup_DownBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_InputStatusSetup_StartBut                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_InputStatusSetup_StopBut,   MENUMANAGER_STATE_MACHINE_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_InputStatusSetup_LcdShowMainTitle(void);
static void MenuManager_InputStatusSetup_LcdShowList(void);

static void MenuManager_InputStatusSetup_SubMainFunction(void);
static void MenuManager_InputStatusSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_InputStatusSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_InputStatusSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_InputStatusSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_InputStatusSetup_ListIndex, MenuManager_InputStatusSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_InputStatusSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_InputStatusSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_InputStatusSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_InputStatusSetup_ListIndex, MenuManager_InputStatusSetup_CurPos, MenuManager_InputStatusSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_InputStatusSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_InputStatusSetup_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_InputStatusSetup_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DOOR_CLOSED)    || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SHOCK)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EMERGENCY_STOP) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_INVERTER_ERROR))
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

      MenuManager_InternalDataPush(MENUMANAGER_INPUTSTATUSSETUP_INTERNALDATALENGTH);

      MenuManager_InputStatusSetup_ListIndex = (uint32_t)0U;
      MenuManager_InputStatusSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_InputStatusSetup_LcdShowMainTitle();
    MenuManager_InputStatusSetup_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InputStatusSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_INPUT_STATUS_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_InputStatusSetup_ChildMenuConf.childMenuCfg))[MenuManager_InputStatusSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InputStatusSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_INPUT_STATUS_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InputStatusSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_InputStatusSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_InputStatusSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_InputStatusSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_InputStatusSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_InputStatusSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_InputStatusSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_InputStatusSetup_ListIndex < ((uint32_t)(MenuManager_InputStatusSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_InputStatusSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_InputStatusSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_InputStatusSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_InputStatusSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_InputStatusSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_InputStatusSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_InputStatusSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_InputStatusSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
