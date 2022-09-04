/* 
 * File:   _1260000000_menu_WashSetup.c
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
#define MENUMANAGER_WASHSETUP_INTERNALDATALENGTH                      (uint8_t)2U

#define MENUMANAGER_WASHSETUP_LISTINDEX_IDX                           0U
#define MENUMANAGER_WASHSETUP_CURPOS_IDX                              1U

#define MenuManager_WashSetup_ListIndex                               MenuManager_GetInternalDataPtr(MENUMANAGER_WASHSETUP_LISTINDEX_IDX)
#define MenuManager_WashSetup_CurPos                                  MenuManager_GetInternalDataPtr(MENUMANAGER_WASHSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MinPauseFwdRev_ChildTitleStr[] =     "MIN PAUSE FWD REV";
static const uint8_t MenuManager_StdWashRunTime_ChildTitleStr[] =     "STD WASH RUN TIME";
static const uint8_t MenuManager_StdWashStopTime_ChildTitleStr[] =    "STD WASH STOP TIME";
static const uint8_t MenuManager_DelWashRunTime_ChildTitleStr[] =     "DEL WASH RUN TIME";
static const uint8_t MenuManager_DelWashStopTime_ChildTitleStr[] =    "DEL WASH STOP TIME";
static const uint8_t MenuManager_HvyWashRunTime_ChildTitleStr[] =     "HVY WASH RUN TIME";
static const uint8_t MenuManager_HvyWashStopTime_ChildTitleStr[] =    "HVY WASH STOP TIME";
static const uint8_t MenuManager_StdWashSpeed_ChildTitleStr[] =       "STD WASH SPEED";
static const uint8_t MenuManager_DelWashSpeed_ChildTitleStr[] =       "DEL WASH SPEED";
static const uint8_t MenuManager_HvyWashSpeed_ChildTitleStr[] =       "HVY WASH SPEED";
static const uint8_t MenuManager_MaxWashSpeed_ChildTitleStr[] =       "MAX WASH SPEED";

static const uint8_t MenuManager_WashSetup_MainTitleStr[] =           "WASH SETUP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_WashSetup_ChildMenu[11] =
{
  { &MenuManager_MinPauseFwdRev_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_StdWashRunTime_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_StdWashStopTime_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_DelWashRunTime_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_DelWashStopTime_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_HvyWashRunTime_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_HvyWashStopTime_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_7             },
  { &MenuManager_StdWashSpeed_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_8             },
  { &MenuManager_DelWashSpeed_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_9             },
  { &MenuManager_HvyWashSpeed_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_10            },
  { &MenuManager_MaxWashSpeed_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_11            }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_WashSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)11U,
  .childMenuCfg               = &MenuManager_WashSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_WashSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_WashSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_WashSetup_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_WashSetup_Entry                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_WashSetup_StartBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_WashSetup_StopBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_WashSetup_UpBut                          (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_WashSetup_DownBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_WashSetup_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_WashSetup_Entry                                                     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_MIN_PAUSE_FWD_REV     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_STD_WASH_RUN_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_STD_WASH_STOP_TIME    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_DEL_WASH_RUN_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_DEL_WASH_STOP_TIME    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_HVY_WASH_RUN_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_HVY_WASH_STOP_TIME    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_8,                                                MENUMANAGER_STATE_STD_WASH_SPEED        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_9,                                                MENUMANAGER_STATE_DEL_WASH_SPEED        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_10,                                               MENUMANAGER_STATE_HVY_WASH_SPEED        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_11,                                               MENUMANAGER_STATE_MAX_WASH_SPEED        ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_WashSetup_UpBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_WashSetup_DownBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_WashSetup_StartBut                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_WashSetup_StopBut,          MENUMANAGER_STATE_MACHINE_SETUP         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_WashSetup_LcdShowMainTitle(void);
static void MenuManager_WashSetup_LcdShowList(void);

static void MenuManager_WashSetup_SubMainFunction(void);
static void MenuManager_WashSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_WashSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_WashSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_WashSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_WashSetup_ListIndex, MenuManager_WashSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_WashSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_WashSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_WashSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_WashSetup_ListIndex, MenuManager_WashSetup_CurPos, MenuManager_WashSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_WashSetup_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_WashSetup_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_WashSetup_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MIN_PAUSE_FWD_REV)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STD_WASH_RUN_TIME)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STD_WASH_STOP_TIME) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DEL_WASH_RUN_TIME)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DEL_WASH_STOP_TIME) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HVY_WASH_RUN_TIME)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HVY_WASH_STOP_TIME) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STD_WASH_SPEED)     || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DEL_WASH_SPEED)     || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HVY_WASH_SPEED)     || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_WASH_SPEED))
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

      MenuManager_InternalDataPush(MENUMANAGER_WASHSETUP_INTERNALDATALENGTH);

      MenuManager_WashSetup_ListIndex = (uint32_t)0U;
      MenuManager_WashSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_WashSetup_LcdShowMainTitle();
    MenuManager_WashSetup_LcdShowList();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_WashSetup_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_WASH_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (tFsmEvent)((*(MenuManager_WashSetup_ChildMenuConf.childMenuCfg))[MenuManager_WashSetup_ListIndex].childMenuEvent));
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_WashSetup_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_WASH_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_WashSetup_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_WashSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_WashSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_WashSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_WashSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_WashSetup_LcdShowList();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_WashSetup_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_WashSetup_ListIndex < ((uint32_t)(MenuManager_WashSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_WashSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_WashSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_WashSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_WashSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_WashSetup_LcdShowList();
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_WashSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_WashSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_WashSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
