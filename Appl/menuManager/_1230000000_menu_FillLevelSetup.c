/* 
 * File:   _1230000000_menu_FillLevelSetup.c
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
#define MENUMANAGER_FILLLEVELSETUP_INTERNALDATALENGTH                 (uint8_t)2U

#define MENUMANAGER_FILLLEVELSETUP_LISTINDEX_IDX                      0U
#define MENUMANAGER_FILLLEVELSETUP_CURPOS_IDX                         1U

#define MenuManager_FillLevelSetup_ListIndex                          MenuManager_GetInternalDataPtr(MENUMANAGER_FILLLEVELSETUP_LISTINDEX_IDX)
#define MenuManager_FillLevelSetup_CurPos                             MenuManager_GetInternalDataPtr(MENUMANAGER_FILLLEVELSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_AutoRefillWhenLow_ChildTitleStr[] =  "AUTO REFILL W LOW";
static const uint8_t MenuManager_ZeroLevel_ChildTitleStr[] =          "ZERO LEVEL";
static const uint8_t MenuManager_LowLevel_ChildTitleStr[] =           "LOW LEVEL";
static const uint8_t MenuManager_MidLevel_ChildTitleStr[] =           "MIDDLE LEVEL";
static const uint8_t MenuManager_HighLevel_ChildTitleStr[] =          "HIGH LEVEL";
static const uint8_t MenuManager_OverbrimLevel_ChildTitleStr[] =      "OVERBRIM LEVEL";
static const uint8_t MenuManager_LevelDiffRefill_ChildTitleStr[] =    "LEVEL DIFF REFILL";
static const uint8_t MenuManager_MaxTimeFill_ChildTitleStr[] =        "MAX TIME REFILL";

static const uint8_t MenuManager_FillLevelSetup_MainTitleStr[] =      "FILL AND LEVEL";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_FillLevelSetup_ChildMenu[8] =
{
  { &MenuManager_AutoRefillWhenLow_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_ZeroLevel_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_LowLevel_ChildTitleStr,                              MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_MidLevel_ChildTitleStr,                              MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_HighLevel_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_OverbrimLevel_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_LevelDiffRefill_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_7             },
  { &MenuManager_MaxTimeFill_ChildTitleStr,                           MENUMANAGER_EVENT_SUBMENU_8             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_FillLevelSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)8U,
  .childMenuCfg               = &MenuManager_FillLevelSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_FillLevelSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_FillLevelSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_FillLevelSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_FillLevelSetup_Entry                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillLevelSetup_StartBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillLevelSetup_StopBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillLevelSetup_UpBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_FillLevelSetup_DownBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_FillLevelSetup_StateMachine[13] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_FillLevelSetup_Entry                                                ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_AUTO_REFILL_WHEN_LOW  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_ZERO_LEVEL            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_LOW_LEVEL             ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_MID_LEVEL             ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_HIGH_LEVEL            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_OVERBRIM_LEVEL        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_LEVEL_DIFF_REFILL     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_8,                                                MENUMANAGER_STATE_MAX_TIME_REFILL       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_FillLevelSetup_UpBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_FillLevelSetup_DownBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_FillLevelSetup_StartBut                                             ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_FillLevelSetup_StopBut,     MENUMANAGER_STATE_MACHINE_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_FillLevelSetup_LcdShowMainTitle(void);
static void MenuManager_FillLevelSetup_LcdShowList(void);

static void MenuManager_FillLevelSetup_SubMainFunction(void);
static void MenuManager_FillLevelSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_FillLevelSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_FillLevelSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_FillLevelSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_FillLevelSetup_ListIndex, MenuManager_FillLevelSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_FillLevelSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_FillLevelSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_FillLevelSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_FillLevelSetup_ListIndex, MenuManager_FillLevelSetup_CurPos, MenuManager_FillLevelSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillLevelSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_FillLevelSetup_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_FillLevelSetup_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_AUTO_REFILL_WHEN_LOW) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_ZERO_LEVEL)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_LOW_LEVEL)            || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MID_LEVEL)            || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_LEVEL)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_OVERBRIM_LEVEL)       || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_LEVEL_DIFF_REFILL)    || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_TIME_REFILL))
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

      MenuManager_InternalDataPush(MENUMANAGER_FILLLEVELSETUP_INTERNALDATALENGTH);

      MenuManager_FillLevelSetup_ListIndex = (uint32_t)0U;
      MenuManager_FillLevelSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_FillLevelSetup_LcdShowMainTitle();
    MenuManager_FillLevelSetup_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillLevelSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_FILL_LEVEL_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_FillLevelSetup_ChildMenuConf.childMenuCfg))[MenuManager_FillLevelSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillLevelSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_FILL_LEVEL_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillLevelSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_FillLevelSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_FillLevelSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_FillLevelSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_FillLevelSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_FillLevelSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_FillLevelSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_FillLevelSetup_ListIndex < ((uint32_t)(MenuManager_FillLevelSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_FillLevelSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_FillLevelSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_FillLevelSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_FillLevelSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_FillLevelSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_FillLevelSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_FillLevelSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_FillLevelSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
