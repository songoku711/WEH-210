/* 
 * File:   _1311000000_menu_ProgramMainSetup.c
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
#define MENUMANAGER_PROGRAMMAINSETUP_INTERNALDATALENGTH               (uint8_t)2U

#define MENUMANAGER_PROGRAMMAINSETUP_LISTINDEX_IDX                    0U
#define MENUMANAGER_PROGRAMMAINSETUP_CURPOS_IDX                       1U
#define MENUMANAGER_PROGRAMMAINSETUP_SEQIDX_IDX                       2U
#define MENUMANAGER_PROGRAMMAINSETUP_STEPIDX_IDX                      3U

#define MenuManager_ProgramMainSetup_ListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_LISTINDEX_IDX)
#define MenuManager_ProgramMainSetup_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_CURPOS_IDX)
#define MenuManager_ProgramMainSetup_SeqIdx                           MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_SEQIDX_IDX)
#define MenuManager_ProgramMainSetup_StepIdx                          MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_STEPIDX_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_IsActive_ChildTitleStr[] =           "ACTIVE STATE";
static const uint8_t MenuManager_WaterMode_ChildTitleStr[] =          "WATER MODE";
static const uint8_t MenuManager_SoapMode_ChildTitleStr[] =           "SUPPLY MODE";
static const uint8_t MenuManager_WashMode_ChildTitleStr[] =           "WASH MODE";
static const uint8_t MenuManager_TempMode_ChildTitleStr[] =           "TEMP MODE";
static const uint8_t MenuManager_LevelMode_ChildTitleStr[] =          "LEVEL MODE";
static const uint8_t MenuManager_BalanceTime_ChildTitleStr[] =        "BALANCE TIME";
static const uint8_t MenuManager_MidExtractTime_ChildTitleStr[] =     "MID EXT TIME";
static const uint8_t MenuManager_HighExtractTime1_ChildTitleStr[] =   "HIGH EXT TIME 1";
static const uint8_t MenuManager_HighExtractTime2_ChildTitleStr[] =   "HIGH EXT TIME 2";
static const uint8_t MenuManager_HighExtractTime3_ChildTitleStr[] =   "HIGH EXT TIME 3";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_ProgramMainSetup_ChildMenu[11] =
{
  { &MenuManager_IsActive_ChildTitleStr,                              MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_WaterMode_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_SoapMode_ChildTitleStr,                              MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_WashMode_ChildTitleStr,                              MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_TempMode_ChildTitleStr,                              MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_LevelMode_ChildTitleStr,                             MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_BalanceTime_ChildTitleStr,                           MENUMANAGER_EVENT_SUBMENU_7             },
  { &MenuManager_MidExtractTime_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_8             },
  { &MenuManager_HighExtractTime1_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_9             },
  { &MenuManager_HighExtractTime2_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_10            },
  { &MenuManager_HighExtractTime3_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_11            }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_ProgramMainSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)11U,
  .childMenuCfg               = &MenuManager_ProgramMainSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_ProgramMainSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_ProgramMainSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_ProgramMainSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_ProgramMainSetup_Entry               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramMainSetup_StartBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramMainSetup_StopBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramMainSetup_UpBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_ProgramMainSetup_DownBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_ProgramMainSetup_StateMachine[16] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ProgramMainSetup_Entry                                              ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_IS_ACTIVE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_STEP_WATER_MODE       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_STEP_SOAP_MODE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_STEP_WASH_MODE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_STEP_TEMP_MODE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_STEP_LEVEL_MODE       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_STEP_BALANCE_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_8,                                                MENUMANAGER_STATE_STEP_MID_EXTRACT_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_9,                                                MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_1  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_10,                                               MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_2  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_11,                                               MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_3  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ProgramMainSetup_UpBut                                              ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ProgramMainSetup_DownBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ProgramMainSetup_StartBut                                           ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ProgramMainSetup_StopBut,   MENUMANAGER_STATE_PROGRAM_STEP_SETUP    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_ProgramMainSetup_LcdShowMainTitle(void);
static void MenuManager_ProgramMainSetup_LcdShowList(void);

static void MenuManager_ProgramMainSetup_SubMainFunction(void);
static void MenuManager_ProgramMainSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_ProgramMainSetup_LcdShowMainTitle(void)
{
  uint8_t tempStr[20];

  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();

  sprintf(tempStr, MenuManager_Common_ProgramStepStr, MenuManager_ProgramMainSetup_SeqIdx + 1U, MenuManager_ProgramMainSetup_StepIdx + 1U);

  MenuManager_Common_LcdShowMainTitle(tempStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ProgramMainSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  uint8_t  tempStr[20];
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_ProgramMainSetup_ListIndex, MenuManager_ProgramMainSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)PROGRAMMANAGER_SEQUENCE_NUM_MAX);
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    sprintf(tempStr, MenuManager_Common_ProgramStr, currentListIndex + 1U);

    MenuManager_Common_LcdShowListElementName
    (
      tempStr,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_ProgramMainSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_ProgramMainSetup_ListIndex, MenuManager_ProgramMainSetup_CurPos, PROGRAMMANAGER_SEQUENCE_NUM_MAX);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_ProgramMainSetup_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_ProgramMainSetup_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_STEP_SETUP)
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

      MenuManager_InternalDataPush(MENUMANAGER_PROGRAMMAINSETUP_INTERNALDATALENGTH);

      MenuManager_ProgramMainSetup_ListIndex = (uint32_t)0U;
      MenuManager_ProgramMainSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_ProgramMainSetup_LcdShowMainTitle();
    MenuManager_ProgramMainSetup_LcdShowList();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramMainSetupStruct* dataHierachy;

  dataHierachy = (MenuManager_Common_ProgramMainSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramMainSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_SETUP;
  dataHierachy->seqIdx = MenuManager_ProgramMainSetup_ListIndex;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ProgramMainSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_ProgramMainSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_ProgramMainSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_ProgramMainSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_ProgramMainSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_ProgramMainSetup_ListIndex < ((uint32_t)(PROGRAMMANAGER_SEQUENCE_NUM_MAX) - (uint32_t)1U))
  {
    MenuManager_ProgramMainSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_ProgramMainSetup_CurPos < MenuManager_Common_LcdMaxCurPos((uint32_t)(PROGRAMMANAGER_SEQUENCE_NUM_MAX)))
  {
    MenuManager_ProgramMainSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_ProgramMainSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_ProgramMainSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_ProgramMainSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_ProgramMainSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
