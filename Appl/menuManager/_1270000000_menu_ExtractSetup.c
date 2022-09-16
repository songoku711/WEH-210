/* 
 * File:   _1270000000_menu_ExtractSetup.c
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
#define MENUMANAGER_EXTRACTSETUP_INTERNALDATALENGTH                   (uint8_t)2U

#define MENUMANAGER_EXTRACTSETUP_LISTINDEX_IDX                        0U
#define MENUMANAGER_EXTRACTSETUP_CURPOS_IDX                           1U

#define MenuManager_ExtractSetup_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_EXTRACTSETUP_LISTINDEX_IDX)
#define MenuManager_ExtractSetup_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_EXTRACTSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_BalanceLevel_ChildTitleStr[] =                           "BALANCE LEVEL";
static const uint8_t MenuManager_BalanceDrainLevel_ChildTitleStr[] =                      "BAL DRAIN LEVEL";
static const uint8_t MenuManager_BalanceWithWaterTime_ChildTitleStr[] =                   "BAL W WATER TIME";
static const uint8_t MenuManager_BalanceDrainWaterTime_ChildTitleStr[] =                  "BAL D WATER TIME";
static const uint8_t MenuManager_BalanceDelayTime_ChildTitleStr[] =                       "BAL DELAY TIME";
static const uint8_t MenuManager_FwdRunTime_ChildTitleStr[] =                             "FWD RUN TIME";
static const uint8_t MenuManager_MidExtractTime_ChildTitleStr[] =                         "MID EXTRACT TIME";
static const uint8_t MenuManager_MidExtractDelayTime_ChildTitleStr[] =                    "MID EXT DELAY TIME";
static const uint8_t MenuManager_HighExtractTime1_ChildTitleStr[] =                       "HIGH EXT TIME 1";
static const uint8_t MenuManager_HighExtractTime2_ChildTitleStr[] =                       "HIGH EXT TIME 2";
static const uint8_t MenuManager_HighExtractTime3_ChildTitleStr[] =                       "HIGH EXT TIME 3";
static const uint8_t MenuManager_HighExtractDelayTime_ChildTitleStr[] =                   "HIGH EXT DLY TIME";
static const uint8_t MenuManager_MaxExtractTime_ChildTitleStr[] =                         "MAX EXTRACT TIME";
static const uint8_t MenuManager_ReextractTime_ChildTitleStr[] =                          "RE-EXTRACT TIME";
static const uint8_t MenuManager_ReextractWashTime_ChildTitleStr[] =                      "RE-EXT WASH TIME";
static const uint8_t MenuManager_BalanceSpeed_ChildTitleStr[] =                           "BALANCE SPEED";
static const uint8_t MenuManager_FwdRunSpeed_ChildTitleStr[] =                            "FWD RUN SPEED";
static const uint8_t MenuManager_MidExtractSpeed_ChildTitleStr[] =                        "MID EXTRACT SPEED";
static const uint8_t MenuManager_HighExtractSpeed1_ChildTitleStr[] =                      "HIGH EXT SPEED 1";
static const uint8_t MenuManager_HighExtractSpeed2_ChildTitleStr[] =                      "HIGH EXT SPEED 2";
static const uint8_t MenuManager_HighExtractSpeed3_ChildTitleStr[] =                      "HIGH EXT SPEED 3";
static const uint8_t MenuManager_MaxMidExtractSpeed_ChildTitleStr[] =                     "MAX MID EXT SPEED";
static const uint8_t MenuManager_MaxHighExtractSpeed_ChildTitleStr[] =                    "MAX HIGH EXT SPEED";

static const uint8_t MenuManager_ExtractSetup_MainTitleStr[] =                            "EXTRACT SETUP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_ExtractSetup_ChildMenu[23] =
{
  { &MenuManager_BalanceLevel_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_BalanceDrainLevel_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_BalanceWithWaterTime_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_BalanceDrainWaterTime_ChildTitleStr,                 MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_BalanceDelayTime_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_FwdRunTime_ChildTitleStr,                            MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_MidExtractTime_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_7             },
  { &MenuManager_MidExtractDelayTime_ChildTitleStr,                   MENUMANAGER_EVENT_SUBMENU_8             },
  { &MenuManager_HighExtractTime1_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_9             },
  { &MenuManager_HighExtractTime2_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_10            },
  { &MenuManager_HighExtractTime3_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_11            },
  { &MenuManager_HighExtractDelayTime_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_12            },
  { &MenuManager_MaxExtractTime_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_13            },
  { &MenuManager_ReextractTime_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_14            },
  { &MenuManager_ReextractWashTime_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_15            },
  { &MenuManager_BalanceSpeed_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_16            },
  { &MenuManager_FwdRunSpeed_ChildTitleStr,                           MENUMANAGER_EVENT_SUBMENU_17            },
  { &MenuManager_MidExtractSpeed_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_18            },
  { &MenuManager_HighExtractSpeed1_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_19            },
  { &MenuManager_HighExtractSpeed2_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_20            },
  { &MenuManager_HighExtractSpeed3_ChildTitleStr,                     MENUMANAGER_EVENT_SUBMENU_21            },
  { &MenuManager_MaxMidExtractSpeed_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_22            },
  { &MenuManager_MaxHighExtractSpeed_ChildTitleStr,                   MENUMANAGER_EVENT_SUBMENU_23            }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_ExtractSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)23U,
  .childMenuCfg               = &MenuManager_ExtractSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ExtractSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ExtractSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_ExtractSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_ExtractSetup_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractSetup_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractSetup_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractSetup_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ExtractSetup_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_ExtractSetup_StateMachine[28] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ExtractSetup_Entry                                                    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_BALANCE_LEVEL           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_BALANCE_DRAIN_LEVEL     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_BALANCE_WITH_WATER_TIME ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_BALANCE_DRAIN_WATER_TIME),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_BALANCE_DELAY_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_FWD_RUN_TIME            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_MID_EXTRACT_TIME        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_8,                                                MENUMANAGER_STATE_MID_EXTRACT_DELAY_TIME  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_9,                                                MENUMANAGER_STATE_HIGH_EXTRACT_TIME_1     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_10,                                               MENUMANAGER_STATE_HIGH_EXTRACT_TIME_2     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_11,                                               MENUMANAGER_STATE_HIGH_EXTRACT_TIME_3     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_12,                                               MENUMANAGER_STATE_HIGH_EXTRACT_DELAY_TIME ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_13,                                               MENUMANAGER_STATE_MAX_EXTRACT_TIME        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_14,                                               MENUMANAGER_STATE_REEXTRACT_TIME          ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_15,                                               MENUMANAGER_STATE_REEXTRACT_WASH_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_16,                                               MENUMANAGER_STATE_BALANCE_SPEED           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_17,                                               MENUMANAGER_STATE_FWD_RUN_SPEED           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_18,                                               MENUMANAGER_STATE_MID_EXTRACT_SPEED       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_19,                                               MENUMANAGER_STATE_HIGH_EXTRACT_SPEED_1    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_20,                                               MENUMANAGER_STATE_HIGH_EXTRACT_SPEED_2    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_21,                                               MENUMANAGER_STATE_HIGH_EXTRACT_SPEED_3    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_22,                                               MENUMANAGER_STATE_MAX_MID_EXTRACT_SPEED   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_23,                                               MENUMANAGER_STATE_MAX_HIGH_EXTRACT_SPEED  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ExtractSetup_UpBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ExtractSetup_DownBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ExtractSetup_StartBut                                                 ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ExtractSetup_StopBut,       MENUMANAGER_STATE_MACHINE_SETUP           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ExtractSetup_LcdShowMainTitle(void);
static void MenuManager_ExtractSetup_LcdShowList(void);

static void MenuManager_ExtractSetup_SubMainFunction(void);
static void MenuManager_ExtractSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ExtractSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_ExtractSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ExtractSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_ExtractSetup_ListIndex, MenuManager_ExtractSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_ExtractSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_ExtractSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_ExtractSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_ExtractSetup_ListIndex, MenuManager_ExtractSetup_CurPos, MenuManager_ExtractSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_BALANCE_LEVEL           )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_BALANCE_DRAIN_LEVEL     )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_BALANCE_WITH_WATER_TIME )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_BALANCE_DRAIN_WATER_TIME)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_BALANCE_DELAY_TIME      )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FWD_RUN_TIME            )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MID_EXTRACT_TIME        )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MID_EXTRACT_DELAY_TIME  )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_EXTRACT_TIME_1     )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_EXTRACT_TIME_2     )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_EXTRACT_TIME_3     )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_EXTRACT_DELAY_TIME )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_EXTRACT_TIME        )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_REEXTRACT_TIME          )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_REEXTRACT_WASH_TIME     )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_BALANCE_SPEED           )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_FWD_RUN_SPEED           )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MID_EXTRACT_SPEED       )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_EXTRACT_SPEED_1    )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_EXTRACT_SPEED_2    )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HIGH_EXTRACT_SPEED_3    )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_MID_EXTRACT_SPEED   )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_HIGH_EXTRACT_SPEED  ))
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

      MenuManager_InternalDataPush(MENUMANAGER_EXTRACTSETUP_INTERNALDATALENGTH);

      MenuManager_ExtractSetup_ListIndex = (uint32_t)0U;
      MenuManager_ExtractSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
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
    MenuManager_ExtractSetup_LcdShowMainTitle();
    MenuManager_ExtractSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_ExtractSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_ExtractSetup_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_EXTRACT_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_ExtractSetup_ChildMenuConf.childMenuCfg))[MenuManager_ExtractSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_EXTRACT_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ExtractSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_ExtractSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_ExtractSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_ExtractSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_ExtractSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ExtractSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ExtractSetup_ListIndex < ((uint32_t)(MenuManager_ExtractSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_ExtractSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_ExtractSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_ExtractSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_ExtractSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_ExtractSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_ExtractSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ExtractSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_ExtractSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
