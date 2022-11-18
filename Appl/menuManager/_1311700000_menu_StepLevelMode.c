/* 
 * File:   _1311700000_menu_StepLevelMode.c
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
#define MENUMANAGER_STEPLEVELMODE_INTERNALDATALENGTH                  (uint8_t)7U

#define MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_IDX                   0U
#define MENUMANAGER_STEPLEVELMODE_COUNTER_IDX                         1U
#define MENUMANAGER_STEPLEVELMODE_LISTINDEX_IDX                       2U
#define MENUMANAGER_STEPLEVELMODE_LASTLISTINDEX_IDX                   3U
#define MENUMANAGER_STEPLEVELMODE_CURPOS_IDX                          4U
#define MENUMANAGER_STEPLEVELMODE_SEQIDX_IDX                          5U
#define MENUMANAGER_STEPLEVELMODE_STEPIDX_IDX                         6U

#define MenuManager_StepLevelMode_InternalState                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_IDX)
#define MenuManager_StepLevelMode_Counter                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODE_COUNTER_IDX)
#define MenuManager_StepLevelMode_ListIndex                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODE_LISTINDEX_IDX)
#define MenuManager_StepLevelMode_LastListIndex                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODE_LASTLISTINDEX_IDX)
#define MenuManager_StepLevelMode_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODE_CURPOS_IDX)
#define MenuManager_StepLevelMode_SeqIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODE_SEQIDX_IDX)
#define MenuManager_StepLevelMode_StepIdx                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPLEVELMODE_STEPIDX_IDX)



#define MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_READY                 (uint32_t)0U
#define MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_RUNNING               (uint32_t)1U
#define MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_DONE                  (uint32_t)2U
#define MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_EXITING               (uint32_t)3U

#define MENUMANAGER_STEPLEVELMODE_COUNTER_MAX                         (uint32_t)50U

#define MENUMANAGER_STEPLEVELMODE_LISTINDEX_LOW                       (uint32_t)0U
#define MENUMANAGER_STEPLEVELMODE_LISTINDEX_MID                       (uint32_t)1U
#define MENUMANAGER_STEPLEVELMODE_LISTINDEX_HIGH                      (uint32_t)2U
#define MENUMANAGER_STEPLEVELMODE_LISTINDEX_CUSTOM                    (uint32_t)3U
#define MENUMANAGER_STEPLEVELMODE_LISTINDEX_UNDEFINED                 (uint32_t)255U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_LevelModeType mapValue;
} MenuManager_StepLevelMode_IntValMapStruct;

static MenuManager_StepLevelMode_IntValMapStruct MenuManager_StepLevelMode_IntValMapConf[4] =
{
  { MENUMANAGER_STEPLEVELMODE_LISTINDEX_LOW,                          PROGRAMMANAGER_LEVEL_MODE_LOW           },
  { MENUMANAGER_STEPLEVELMODE_LISTINDEX_MID,                          PROGRAMMANAGER_LEVEL_MODE_MID           },
  { MENUMANAGER_STEPLEVELMODE_LISTINDEX_HIGH,                         PROGRAMMANAGER_LEVEL_MODE_HIGH          },
  { MENUMANAGER_STEPLEVELMODE_LISTINDEX_CUSTOM,                       PROGRAMMANAGER_LEVEL_MODE_CUSTOM        }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepLevelMode_LowStr[] =             "LOW";
static const uint8_t MenuManager_StepLevelMode_MidStr[] =             "MIDDLE";
static const uint8_t MenuManager_StepLevelMode_HighStr[] =            "HIGH";
static const uint8_t MenuManager_StepLevelMode_CustomStr[] =          "CUSTOM";

static const uint8_t MenuManager_StepLevelMode_MainTitleStr[] =       "LEVEL MODE";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepLevelMode_ChildMenu[4] =
{
  { &MenuManager_StepLevelMode_LowStr,                                MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepLevelMode_MidStr,                                MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepLevelMode_HighStr,                               MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepLevelMode_CustomStr,                             MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepLevelMode_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)4U,
  .childMenuCfg               = &MenuManager_StepLevelMode_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepLevelMode_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepLevelMode_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepLevelMode_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepLevelMode_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelMode_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelMode_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelMode_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelMode_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepLevelMode_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepLevelMode_StateMachine[8] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepLevelMode_Entry                                                 ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepLevelMode_Exit                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_LEVEL_MODE_CUSTOM  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepLevelMode_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepLevelMode_StopBut,      MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepLevelMode_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepLevelMode_DownBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepLevelMode_LcdShowMainTitle(void);
static void MenuManager_StepLevelMode_LcdShowList(void);
static void MenuManager_StepLevelMode_LcdShowDone(void);

static void MenuManager_StepLevelMode_SubMainFunction(void);
static void MenuManager_StepLevelMode_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepLevelMode_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepLevelMode_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepLevelMode_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepLevelMode_ListIndex, MenuManager_StepLevelMode_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepLevelMode_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepLevelMode_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepLevelMode_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepLevelMode_ListIndex, MenuManager_StepLevelMode_CurPos, MenuManager_StepLevelMode_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepLevelMode_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_StepLevelMode_ChildMenuConf.childMenuCfg))[MenuManager_StepLevelMode_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_StepLevelMode_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_StepLevelMode_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelMode_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  ProgramManager_LevelModeType tempLevelMode;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_NORM_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPLEVELMODE_INTERNALDATALENGTH);

      MenuManager_StepLevelMode_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepLevelMode_StepIdx = enterDataHierachy->stepIdx;

      MenuManager_StepLevelMode_InternalState = MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_READY;
      MenuManager_StepLevelMode_Counter = (uint32_t)0U;

      ProgramManager_NormStepConfig_LevelMode_GetData((uint8_t)MenuManager_StepLevelMode_SeqIdx, (uint8_t)MenuManager_StepLevelMode_StepIdx, &tempLevelMode);
      
      for ( MenuManager_StepLevelMode_ListIndex = (uint32_t)0U; \
            MenuManager_StepLevelMode_ListIndex < MenuManager_StepLevelMode_ChildMenuConf.childMenuNum; \
            MenuManager_StepLevelMode_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_StepLevelMode_IntValMapConf[MenuManager_StepLevelMode_ListIndex].mapValue == tempLevelMode)
        {
          break;
        }
      }
      
      MenuManager_StepLevelMode_LastListIndex = MenuManager_StepLevelMode_ListIndex;
      MenuManager_StepLevelMode_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_StepLevelMode_ListIndex, MenuManager_StepLevelMode_ChildMenuConf.childMenuNum);

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
    MenuManager_StepLevelMode_LcdShowMainTitle();
    MenuManager_StepLevelMode_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepLevelMode_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepLevelMode_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelMode_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* inDataHierachy;
  Fsm_DataHierachyStruct* outDataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  if (MenuManager_StepLevelMode_ListIndex == MENUMANAGER_STEPLEVELMODE_LISTINDEX_CUSTOM)
  {
    inDataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
    inDataHierachy->dataId = MENUMANAGER_STATE_STEP_LEVEL_MODE;
    inDataHierachy->seqIdx = MenuManager_StepLevelMode_SeqIdx;
    inDataHierachy->stepIdx = MenuManager_StepLevelMode_StepIdx;

    pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)inDataHierachy;
  }
  else
  {
    outDataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
    outDataHierachy->dataId = MENUMANAGER_STATE_STEP_LEVEL_MODE;

    pFsmContext->dataHierachy = outDataHierachy;
  }

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelMode_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelMode_InternalState == MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_READY)
  {
    MenuManager_StepLevelMode_InternalState = MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelMode_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelMode_InternalState != MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }

  MenuManager_StepLevelMode_ListIndex = MENUMANAGER_STEPLEVELMODE_LISTINDEX_UNDEFINED;
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelMode_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelMode_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepLevelMode_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepLevelMode_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepLevelMode_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepLevelMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepLevelMode_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepLevelMode_ListIndex < ((uint32_t)(MenuManager_StepLevelMode_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepLevelMode_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepLevelMode_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_StepLevelMode_ChildMenuConf.childMenuNum))
  {
    MenuManager_StepLevelMode_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepLevelMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepLevelMode_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepLevelMode_ButEventMapConf);

  switch (MenuManager_StepLevelMode_InternalState)
  {
    case MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_RUNNING:
    {
      if (MenuManager_StepLevelMode_LastListIndex == MENUMANAGER_STEPLEVELMODE_LISTINDEX_CUSTOM)
      {
        if (MenuManager_StepLevelMode_ListIndex == MenuManager_StepLevelMode_LastListIndex)
        {
          Fsm_TriggerEvent( &MenuManager_FsmContext, \
                            (Fsm_EventType)((*(MenuManager_StepLevelMode_ChildMenuConf.childMenuCfg))[MenuManager_StepLevelMode_ListIndex].childMenuEvent));
          
          MenuManager_StepLevelMode_InternalState = MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_EXITING;

          break;
        }
      }

      ProgramManager_NormStepConfig_LevelMode_SetData \
      ( \
        (uint8_t)MenuManager_StepLevelMode_SeqIdx, \
        (uint8_t)MenuManager_StepLevelMode_StepIdx, \
        &(MenuManager_StepLevelMode_IntValMapConf[MenuManager_StepLevelMode_ListIndex].mapValue) \
      );

      if ((uint8_t)MenuManager_StepLevelMode_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_LevelMode_GetData \
        ( \
          (uint8_t)MenuManager_StepLevelMode_SeqIdx, \
          (uint8_t)MenuManager_StepLevelMode_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepLevelMode_StepIdx].levelMode) \
        );
      }
      
      MenuManager_StepLevelMode_InternalState = MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_DONE:
    {
      MenuManager_StepLevelMode_LcdShowDone();

      MenuManager_StepLevelMode_InternalState = MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepLevelMode_SubTickHandler(void)
{
  if (MenuManager_StepLevelMode_InternalState == MENUMANAGER_STEPLEVELMODE_INTERNALSTATE_EXITING)
  {
    MenuManager_StepLevelMode_Counter += (uint32_t)1U;

    if (MenuManager_StepLevelMode_Counter >= MENUMANAGER_STEPLEVELMODE_COUNTER_MAX)
    {
      MenuManager_StepLevelMode_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_StepLevelMode_ChildMenuConf.childMenuCfg))[MenuManager_StepLevelMode_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
