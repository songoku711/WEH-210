/* 
 * File:   _1311000000_menu_StepNormSetup.c
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
#define MENUMANAGER_STEPNORMSETUP_INTERNALDATALENGTH                  (uint8_t)5U

#define MENUMANAGER_STEPNORMSETUP_LISTINDEX_IDX                       0U
#define MENUMANAGER_STEPNORMSETUP_ISACTIVE_IDX                        1U
#define MENUMANAGER_STEPNORMSETUP_CURPOS_IDX                          2U
#define MENUMANAGER_STEPNORMSETUP_SEQIDX_IDX                          3U
#define MENUMANAGER_STEPNORMSETUP_STEPIDX_IDX                         4U

#define MenuManager_StepNormSetup_ListIndex                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPNORMSETUP_LISTINDEX_IDX)
#define MenuManager_StepNormSetup_IsActive                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPNORMSETUP_ISACTIVE_IDX)
#define MenuManager_StepNormSetup_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPNORMSETUP_CURPOS_IDX)
#define MenuManager_StepNormSetup_SeqIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPNORMSETUP_SEQIDX_IDX)
#define MenuManager_StepNormSetup_StepIdx                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPNORMSETUP_STEPIDX_IDX)

#define MENUMANAGER_STEPNORMSETUP_LISTINDEX_ISACTIVE                  0U

#define MENUMANAGER_STEPNORMSETUP_LCD_ISACTIVE_STAT_LENGTH            (15U)
#define MENUMANAGER_STEPNORMSETUP_LCD_ISACTIVE_STAT_XPOS              (17U)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepIsActive_ChildTitleStr[] =       "Active";
static const uint8_t MenuManager_StepWaterMode_ChildTitleStr[] =      "Water";
static const uint8_t MenuManager_StepSoapMode_ChildTitleStr[] =       "Soap";
static const uint8_t MenuManager_StepWashMode_ChildTitleStr[] =       "Wash";
static const uint8_t MenuManager_StepWashTime_ChildTitleStr[] =       "Time";
static const uint8_t MenuManager_StepTempMode_ChildTitleStr[] =       "Temperature";
static const uint8_t MenuManager_StepLevelMode_ChildTitleStr[] =      "Level";
static const uint8_t MenuManager_StepDrainMode_ChildTitleStr[] =      "Extract";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepNormSetup_ChildMenu[8] =
{
  { &MenuManager_StepIsActive_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_StepWaterMode_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_StepSoapMode_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_StepWashMode_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_StepWashTime_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_StepTempMode_ChildTitleStr,                          MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_StepLevelMode_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_7             },
  { &MenuManager_StepDrainMode_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_8             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepNormSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)8U,
  .childMenuCfg               = &MenuManager_StepNormSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepNormSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepNormSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepNormSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepNormSetup_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepNormSetup_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepNormSetup_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepNormSetup_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepNormSetup_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepNormSetup_StateMachine[13] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepNormSetup_Entry                                                 ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_IS_ACTIVE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_STEP_WATER_MODE       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_STEP_SOAP_MODE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_STEP_WASH_MODE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_STEP_WASH_TIME         ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_STEP_TEMP_MODE        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_STEP_LEVEL_MODE       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_8,                                                MENUMANAGER_STATE_STEP_DRAIN_MODE       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepNormSetup_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepNormSetup_DownBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepNormSetup_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepNormSetup_StopBut,      MENUMANAGER_STATE_PROGRAM_STEP_SETUP    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepNormSetup_LcdShowMainTitle(void);
static void MenuManager_StepNormSetup_LcdShowList(void);

static void MenuManager_StepNormSetup_SubMainFunction(void);
static void MenuManager_StepNormSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepNormSetup_LcdShowMainTitle(void)
{
  uint8_t tempStr[20];

  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();

  sprintf((char *)tempStr, (const char *)MenuManager_Common_ProgramStepStr, MenuManager_StepNormSetup_SeqIdx + 1U, MenuManager_StepNormSetup_StepIdx + 1U);

  MenuManager_Common_LcdShowMainTitle(tempStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepNormSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepNormSetup_ListIndex, MenuManager_StepNormSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  if (MenuManager_StepNormSetup_IsActive == (uint32_t)false)
  {
    listLength = (uint32_t)1U;
  }
  else
  {
    listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepNormSetup_ChildMenuConf.childMenuNum));
  }
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    if (currentListIndex == MENUMANAGER_STEPNORMSETUP_LISTINDEX_ISACTIVE)
    {
      MenuManager_Common_LcdShowListElementName
      (
        (uint8_t *)*((*(MenuManager_StepNormSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
        MENUMANAGER_STEPNORMSETUP_LCD_ISACTIVE_STAT_LENGTH,
        MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
        currentCursorPos
      );

      LCD_SetCursorPos(MENUMANAGER_STEPNORMSETUP_LCD_ISACTIVE_STAT_XPOS, currentCursorPos, LCD_CURSOR_BY_FONT);

      if (MenuManager_StepNormSetup_IsActive == (uint32_t)false)
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
        (uint8_t *)*((*(MenuManager_StepNormSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
        MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
        MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
        currentCursorPos
      );
    }
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepNormSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepNormSetup_ListIndex, MenuManager_StepNormSetup_CurPos, MenuManager_StepNormSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepNormSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_TIME)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_TEMP_MODE)           || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_TEMP_MODE_CUSTOM)    || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_LEVEL_MODE)          || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_LEVEL_MODE_CUSTOM)   || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_DRAIN_MODE)          || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_DRAIN_MODE_CUSTOM))
    {
      ProgramManager_NormStepConfig_IsActive_GetData((uint8_t)MenuManager_StepNormSetup_SeqIdx, (uint8_t)MenuManager_StepNormSetup_StepIdx, &tempIsActive);
    
      MenuManager_StepNormSetup_IsActive = (uint32_t)tempIsActive;
      
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_STEP_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPNORMSETUP_INTERNALDATALENGTH);

      MenuManager_StepNormSetup_ListIndex      = (uint32_t)0U;
      MenuManager_StepNormSetup_CurPos         = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
      MenuManager_StepNormSetup_SeqIdx         = enterDataHierachy->seqIdx;
      MenuManager_StepNormSetup_StepIdx        = enterDataHierachy->stepIdx;

      ProgramManager_NormStepConfig_IsActive_GetData((uint8_t)MenuManager_StepNormSetup_SeqIdx, (uint8_t)MenuManager_StepNormSetup_StepIdx, &tempIsActive);
    
      MenuManager_StepNormSetup_IsActive = (uint32_t)tempIsActive;

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
    MenuManager_StepNormSetup_LcdShowMainTitle();
    MenuManager_StepNormSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepNormSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepNormSetup_SubTickHandler;
    
    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepNormSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_NORM_SETUP;
  dataHierachy->seqIdx = MenuManager_StepNormSetup_SeqIdx;
  dataHierachy->stepIdx = MenuManager_StepNormSetup_StepIdx;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_StepNormSetup_ChildMenuConf.childMenuCfg))[MenuManager_StepNormSetup_ListIndex].childMenuEvent));

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepNormSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_NORM_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepNormSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepNormSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepNormSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepNormSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepNormSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepNormSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepNormSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t listLength;

  if (MenuManager_StepNormSetup_IsActive == (uint32_t)false)
  {
    listLength = (uint32_t)1U;
  }
  else
  {
    listLength = (uint32_t)(MenuManager_StepNormSetup_ChildMenuConf.childMenuNum);
  }

  if (MenuManager_StepNormSetup_ListIndex < (listLength - (uint32_t)1U))
  {
    MenuManager_StepNormSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepNormSetup_CurPos < MenuManager_Common_LcdMaxCurPos(listLength))
  {
    MenuManager_StepNormSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepNormSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepNormSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepNormSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_StepNormSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
