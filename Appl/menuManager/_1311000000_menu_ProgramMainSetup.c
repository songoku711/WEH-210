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
#define MENUMANAGER_PROGRAMMAINSETUP_INTERNALDATALENGTH               (uint8_t)5U

#define MENUMANAGER_PROGRAMMAINSETUP_LISTINDEX_IDX                    0U
#define MENUMANAGER_PROGRAMMAINSETUP_ISACTIVE_IDX                     1U
#define MENUMANAGER_PROGRAMMAINSETUP_CURPOS_IDX                       2U
#define MENUMANAGER_PROGRAMMAINSETUP_SEQIDX_IDX                       3U
#define MENUMANAGER_PROGRAMMAINSETUP_STEPIDX_IDX                      4U

#define MenuManager_ProgramMainSetup_ListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_LISTINDEX_IDX)
#define MenuManager_ProgramMainSetup_IsActive                         MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_ISACTIVE_IDX)
#define MenuManager_ProgramMainSetup_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_CURPOS_IDX)
#define MenuManager_ProgramMainSetup_SeqIdx                           MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_SEQIDX_IDX)
#define MenuManager_ProgramMainSetup_StepIdx                          MenuManager_GetInternalDataPtr(MENUMANAGER_PROGRAMMAINSETUP_STEPIDX_IDX)

#define MENUMANAGER_PROGRAMMAINSETUP_LISTINDEX_ISACTIVE               0U

#define MENUMANAGER_PROGRAMMAINSETUP_LCD_ISACTIVE_STAT_LENGTH         (15U)
#define MENUMANAGER_PROGRAMMAINSETUP_LCD_ISACTIVE_STAT_XPOS           (17U)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepIsActive_ChildTitleStr[] =                           "ACTIVE STATE";
static const uint8_t MenuManager_StepWaterMode_ChildTitleStr[] =                          "WATER MODE";
static const uint8_t MenuManager_StepSoapMode_ChildTitleStr[] =                           "SUPPLY MODE";
static const uint8_t MenuManager_StepWashMode_ChildTitleStr[] =                           "WASH MODE";
static const uint8_t MenuManager_StepWashNum_ChildTitleStr[] =                            "WASH SPIN NUMBER";
static const uint8_t MenuManager_StepTempMode_ChildTitleStr[] =                           "TEMP MODE";
static const uint8_t MenuManager_StepLevelMode_ChildTitleStr[] =                          "LEVEL MODE";
static const uint8_t MenuManager_StepBalanceTime_ChildTitleStr[] =                        "BALANCE TIME";
static const uint8_t MenuManager_StepMidExtractTime_ChildTitleStr[] =                     "MID EXT TIME";
static const uint8_t MenuManager_StepHighExtractTime1_ChildTitleStr[] =                   "HIGH EXT TIME 1";
static const uint8_t MenuManager_StepHighExtractTime2_ChildTitleStr[] =                   "HIGH EXT TIME 2";
static const uint8_t MenuManager_StepHighExtractTime3_ChildTitleStr[] =                   "HIGH EXT TIME 3";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_ProgramMainSetup_ChildMenu[12] =
{
  { &MenuManager_StepIsActive_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_StepWaterMode_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_StepSoapMode_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_StepWashMode_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_StepWashNum_ChildTitleStr,                           MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_StepTempMode_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_StepLevelMode_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_7             },
  { &MenuManager_StepBalanceTime_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_8             },
  { &MenuManager_StepMidExtractTime_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_9             },
  { &MenuManager_StepHighExtractTime1_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_10            },
  { &MenuManager_StepHighExtractTime2_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_11            },
  { &MenuManager_StepHighExtractTime3_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_12            }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_ProgramMainSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)12U,
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
Fsm_EventEntryStruct MenuManager_ProgramMainSetup_StateMachine[17] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_ProgramMainSetup_Entry                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_IS_ACTIVE            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_STEP_WATER_MODE           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_STEP_SOAP_MODE            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_STEP_WASH_MODE            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_STEP_WASH_NUM             ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_STEP_TEMP_MODE            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_STEP_LEVEL_MODE           ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_8,                                                MENUMANAGER_STATE_STEP_BALANCE_TIME         ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_9,                                                MENUMANAGER_STATE_STEP_MID_EXTRACT_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_10,                                               MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_1  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_11,                                               MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_2  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_12,                                               MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_3  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_ProgramMainSetup_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_ProgramMainSetup_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_ProgramMainSetup_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_ProgramMainSetup_StopBut,   MENUMANAGER_STATE_PROGRAM_STEP_SETUP        )
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

  sprintf((char *)tempStr, (const char *)MenuManager_Common_ProgramStepStr, MenuManager_ProgramMainSetup_SeqIdx + 1U, MenuManager_ProgramMainSetup_StepIdx + 1U);

  MenuManager_Common_LcdShowMainTitle(tempStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_ProgramMainSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_ProgramMainSetup_ListIndex, MenuManager_ProgramMainSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  if (MenuManager_ProgramMainSetup_IsActive == (uint32_t)false)
  {
    listLength = (uint32_t)1U;
  }
  else
  {
    listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_ProgramMainSetup_ChildMenuConf.childMenuNum));
  }
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    if (currentListIndex == MENUMANAGER_PROGRAMMAINSETUP_LISTINDEX_ISACTIVE)
    {
      MenuManager_Common_LcdShowListElementName
      (
        (uint8_t *)*((*(MenuManager_ProgramMainSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
        MENUMANAGER_PROGRAMMAINSETUP_LCD_ISACTIVE_STAT_LENGTH,
        MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
        currentCursorPos
      );

      LCD_SetCursorPos(MENUMANAGER_PROGRAMMAINSETUP_LCD_ISACTIVE_STAT_XPOS, currentCursorPos, LCD_CURSOR_BY_FONT);

      if (MenuManager_ProgramMainSetup_IsActive == (uint32_t)false)
      {
        LCD_PutString((uint8_t *)MenuManager_Common_OffStr);
      }
      else
      {
        LCD_PutString((uint8_t *)MenuManager_Common_OnStr);
      }
    }
    else
    {
      MenuManager_Common_LcdShowListElementName
      (
        (uint8_t *)*((*(MenuManager_ProgramMainSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
        MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
        MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
        currentCursorPos
      );
    }
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_ProgramMainSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_ProgramMainSetup_ListIndex, MenuManager_ProgramMainSetup_CurPos, MenuManager_ProgramMainSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  bool tempIsActive;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_IS_ACTIVE)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WATER_MODE)          || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_SOAP_MODE)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_MODE)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM)    || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_NUM)            || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_TEMP_MODE)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_LEVEL_MODE)          || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_BALANCE_TIME)        || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_MID_EXTRACT_TIME)    || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_1) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_2) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_3))
    {
      ProgramManager_NormStepConfig_IsActive_GetData((uint8_t)MenuManager_ProgramMainSetup_SeqIdx, (uint8_t)MenuManager_ProgramMainSetup_StepIdx, &tempIsActive);
    
      MenuManager_ProgramMainSetup_IsActive = (uint32_t)tempIsActive;
      
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_STEP_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_PROGRAMMAINSETUP_INTERNALDATALENGTH);

      MenuManager_ProgramMainSetup_ListIndex      = (uint32_t)0U;
      MenuManager_ProgramMainSetup_CurPos         = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
      MenuManager_ProgramMainSetup_SeqIdx         = enterDataHierachy->seqIdx;
      MenuManager_ProgramMainSetup_StepIdx        = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_IsActive_GetData((uint8_t)MenuManager_ProgramMainSetup_SeqIdx, (uint8_t)MenuManager_ProgramMainSetup_StepIdx, &tempIsActive);
    
      MenuManager_ProgramMainSetup_IsActive = (uint32_t)tempIsActive;

      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
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
    MenuManager_ProgramMainSetup_LcdShowMainTitle();
    MenuManager_ProgramMainSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_ProgramMainSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_ProgramMainSetup_SubTickHandler;
    
    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_MAIN_SETUP;
  dataHierachy->seqIdx = MenuManager_ProgramMainSetup_SeqIdx;
  dataHierachy->stepIdx = MenuManager_ProgramMainSetup_StepIdx;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_ProgramMainSetup_ChildMenuConf.childMenuCfg))[MenuManager_ProgramMainSetup_ListIndex].childMenuEvent));

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_ProgramMainSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_PROGRAM_MAIN_SETUP;

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
  uint32_t listLength;

  if (MenuManager_ProgramMainSetup_IsActive == (uint32_t)false)
  {
    listLength = (uint32_t)1U;
  }
  else
  {
    listLength = (uint32_t)(MenuManager_ProgramMainSetup_ChildMenuConf.childMenuNum);
  }

  if (MenuManager_ProgramMainSetup_ListIndex < (listLength - (uint32_t)1U))
  {
    MenuManager_ProgramMainSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_ProgramMainSetup_CurPos < MenuManager_Common_LcdMaxCurPos(listLength))
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
