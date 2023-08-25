/* 
 * File:   _1270000000_menu_DrainSetup.c
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
#define MENUMANAGER_DRAINSETUP_INTERNALDATALENGTH                   (uint8_t)2U

#define MENUMANAGER_DRAINSETUP_LISTINDEX_IDX                        0U
#define MENUMANAGER_DRAINSETUP_CURPOS_IDX                           1U

#define MenuManager_DrainSetup_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUP_LISTINDEX_IDX)
#define MenuManager_DrainSetup_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_ForwardDrainParam_ChildTitleStr[] =  "Forward Drain";
static const uint8_t MenuManager_BalanceDrainParam_ChildTitleStr[] =  "Balance Drain";
static const uint8_t MenuManager_ExtrLvl1DrainParam_ChildTitleStr[] = "Extr Lv.1 Drain";
static const uint8_t MenuManager_ExtrLvl2DrainParam_ChildTitleStr[] = "Extr Lv.2 Drain";
static const uint8_t MenuManager_ExtrLvl3DrainParam_ChildTitleStr[] = "Extr Lv.3 Drain";
static const uint8_t MenuManager_ExtrLvl4DrainParam_ChildTitleStr[] = "Extr Lv.4 Drain";
static const uint8_t MenuManager_DrainOffTime_ChildTitleStr[] =       "Drain Off Time";
static const uint8_t MenuManager_MaxDrainExtrTime_ChildTitleStr[] =   "Max Drain Ex.time";
static const uint8_t MenuManager_ReDrainExtrTime_ChildTitleStr[] =    "Redrain Ex.time";
static const uint8_t MenuManager_MaxDrainExtrSpeed_ChildTitleStr[] =  "Max Drain Ex.speed";

static const uint8_t MenuManager_DrainSetup_MainTitleStr[] =          "DRAIN";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_DrainSetup_ChildMenu[10] =
{
  { &MenuManager_ForwardDrainParam_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_BalanceDrainParam_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_ExtrLvl1DrainParam_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_ExtrLvl2DrainParam_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_ExtrLvl3DrainParam_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_ExtrLvl4DrainParam_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_DrainOffTime_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_7             },
  { &MenuManager_MaxDrainExtrTime_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_8             },
  { &MenuManager_ReDrainExtrTime_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_9             },
  { &MenuManager_MaxDrainExtrSpeed_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_10            }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_DrainSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)10U,
  .childMenuCfg               = &MenuManager_DrainSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DrainSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DrainSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_DrainSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DrainSetup_Entry                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetup_StartBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetup_StopBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetup_UpBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetup_DownBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DrainSetup_StateMachine[15] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DrainSetup_Entry                                                    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_DRAIN_SETUP_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_DRAIN_SETUP_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_DRAIN_SETUP_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_DRAIN_SETUP_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_DRAIN_SETUP_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_DRAIN_SETUP_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_DRAIN_OFF_TIME        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_8,                                                MENUMANAGER_STATE_MAX_DRAIN_EXTRACT_TIME),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_9,                                                MENUMANAGER_STATE_REDRAIN_EXTRACT_TIME  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_10,                                               MENUMANAGER_STATE_MAX_DRAIN_EXTRACT_SPEED),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DrainSetup_UpBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DrainSetup_DownBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DrainSetup_StartBut                                                 ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DrainSetup_StopBut,         MENUMANAGER_STATE_MACHINE_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DrainSetup_LcdShowMainTitle(void);
static void MenuManager_DrainSetup_LcdShowList(void);

static void MenuManager_DrainSetup_SubMainFunction(void);
static void MenuManager_DrainSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DrainSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DrainSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DrainSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_DrainSetup_ListIndex, MenuManager_DrainSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_DrainSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_DrainSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_DrainSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_DrainSetup_ListIndex, MenuManager_DrainSetup_CurPos, MenuManager_DrainSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP_TIME        )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_OFF_TIME          )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_DRAIN_EXTRACT_TIME  )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_REDRAIN_EXTRACT_TIME    )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_DRAIN_EXTRACT_SPEED ))
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

      MenuManager_InternalDataPush(MENUMANAGER_DRAINSETUP_INTERNALDATALENGTH);

      MenuManager_DrainSetup_ListIndex = (uint32_t)0U;
      MenuManager_DrainSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
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
    MenuManager_DrainSetup_LcdShowMainTitle();
    MenuManager_DrainSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_DrainSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_DrainSetup_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* defaultDataHierachy;
  MenuManager_Common_DrainSetupStruct* drainSetupDataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  if ((*(MenuManager_DrainSetup_ChildMenuConf.childMenuCfg))[MenuManager_DrainSetup_ListIndex].childMenuEvent > MENUMANAGER_EVENT_SUBMENU_6)
  {
    defaultDataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
    defaultDataHierachy->dataId = MENUMANAGER_STATE_DRAIN_SETUP;

    pFsmContext->dataHierachy = defaultDataHierachy;
  }
  else
  {
    drainSetupDataHierachy = (MenuManager_Common_DrainSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_DrainSetupStruct));
    drainSetupDataHierachy->dataId = MENUMANAGER_STATE_DRAIN_SETUP;
    drainSetupDataHierachy->drainStepIdx = MenuManager_DrainSetup_ListIndex;

    pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)drainSetupDataHierachy;
  }

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_DrainSetup_ChildMenuConf.childMenuCfg))[MenuManager_DrainSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DRAIN_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_DrainSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_DrainSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_DrainSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_DrainSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetup_ListIndex < ((uint32_t)(MenuManager_DrainSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_DrainSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_DrainSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_DrainSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_DrainSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_DrainSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DrainSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DrainSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_DrainSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
