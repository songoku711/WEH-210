/* 
 * File:   _1311400000_menu_StepWashMode.c
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
#define MENUMANAGER_STEPWASHMODE_INTERNALDATALENGTH                   (uint8_t)7U

#define MENUMANAGER_STEPWASHMODE_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_STEPWASHMODE_COUNTER_IDX                          1U
#define MENUMANAGER_STEPWASHMODE_LISTINDEX_IDX                        2U
#define MENUMANAGER_STEPWASHMODE_LASTLISTINDEX_IDX                    3U
#define MENUMANAGER_STEPWASHMODE_CURPOS_IDX                           4U
#define MENUMANAGER_STEPWASHMODE_SEQIDX_IDX                           5U
#define MENUMANAGER_STEPWASHMODE_STEPIDX_IDX                          6U

#define MenuManager_StepWashMode_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODE_INTERNALSTATE_IDX)
#define MenuManager_StepWashMode_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODE_COUNTER_IDX)
#define MenuManager_StepWashMode_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODE_LISTINDEX_IDX)
#define MenuManager_StepWashMode_LastListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODE_LASTLISTINDEX_IDX)
#define MenuManager_StepWashMode_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODE_CURPOS_IDX)
#define MenuManager_StepWashMode_SeqIdx                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODE_SEQIDX_IDX)
#define MenuManager_StepWashMode_StepIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODE_STEPIDX_IDX)



#define MENUMANAGER_STEPWASHMODE_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_STEPWASHMODE_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_STEPWASHMODE_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_STEPWASHMODE_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_STEPWASHMODE_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_STEPWASHMODE_LISTINDEX_STANDARD                   (uint32_t)0U
#define MENUMANAGER_STEPWASHMODE_LISTINDEX_DELICATE                   (uint32_t)1U
#define MENUMANAGER_STEPWASHMODE_LISTINDEX_HEAVY                      (uint32_t)2U
#define MENUMANAGER_STEPWASHMODE_LISTINDEX_CUSTOM                     (uint32_t)3U
#define MENUMANAGER_STEPWASHMODE_LISTINDEX_UNDEFINED                  (uint32_t)255U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_WashModeType mapValue;
} MenuManager_StepWashMode_IntValMapStruct;

static MenuManager_StepWashMode_IntValMapStruct MenuManager_StepWashMode_IntValMapConf[4] =
{
  { MENUMANAGER_STEPWASHMODE_LISTINDEX_STANDARD,                      PROGRAMMANAGER_WASH_MODE_STANDARD       },
  { MENUMANAGER_STEPWASHMODE_LISTINDEX_DELICATE,                      PROGRAMMANAGER_WASH_MODE_DELICATE       },
  { MENUMANAGER_STEPWASHMODE_LISTINDEX_HEAVY,                         PROGRAMMANAGER_WASH_MODE_HEAVY          },
  { MENUMANAGER_STEPWASHMODE_LISTINDEX_CUSTOM,                        PROGRAMMANAGER_WASH_MODE_CUSTOM         }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepWashMode_StandardStr[] =         "STANDARD";
static const uint8_t MenuManager_StepWashMode_DelicateStr[] =         "DELICATE";
static const uint8_t MenuManager_StepWashMode_HeavyStr[] =            "HEAVY";
static const uint8_t MenuManager_StepWashMode_CustomStr[] =           "CUSTOM";

static const uint8_t MenuManager_StepWashMode_MainTitleStr[] =        "WASH MODE";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepWashMode_ChildMenu[4] =
{
  { &MenuManager_StepWashMode_StandardStr,                            MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepWashMode_DelicateStr,                            MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepWashMode_HeavyStr,                               MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepWashMode_CustomStr,                              MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepWashMode_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)4U,
  .childMenuCfg               = &MenuManager_StepWashMode_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepWashMode_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepWashMode_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepWashMode_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepWashMode_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashMode_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashMode_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashMode_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashMode_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashMode_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepWashMode_StateMachine[8] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepWashMode_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepWashMode_Exit                                                   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepWashMode_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepWashMode_StopBut,       MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepWashMode_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepWashMode_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepWashMode_LcdShowMainTitle(void);
static void MenuManager_StepWashMode_LcdShowList(void);
static void MenuManager_StepWashMode_LcdShowDone(void);

static void MenuManager_StepWashMode_SubMainFunction(void);
static void MenuManager_StepWashMode_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepWashMode_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepWashMode_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepWashMode_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepWashMode_ListIndex, MenuManager_StepWashMode_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepWashMode_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepWashMode_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepWashMode_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepWashMode_ListIndex, MenuManager_StepWashMode_CurPos, MenuManager_StepWashMode_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepWashMode_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_StepWashMode_ChildMenuConf.childMenuCfg))[MenuManager_StepWashMode_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_StepWashMode_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_StepWashMode_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashMode_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  ProgramManager_WashModeType tempWashMode;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_NORM_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPWASHMODE_INTERNALDATALENGTH);

      MenuManager_StepWashMode_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepWashMode_StepIdx = enterDataHierachy->stepIdx;

      MenuManager_StepWashMode_InternalState = MENUMANAGER_STEPWASHMODE_INTERNALSTATE_READY;
      MenuManager_StepWashMode_Counter = (uint32_t)0U;

      ProgramManager_NormStepConfig_WashMode_GetData((uint8_t)MenuManager_StepWashMode_SeqIdx, (uint8_t)MenuManager_StepWashMode_StepIdx, &tempWashMode);
      
      for ( MenuManager_StepWashMode_ListIndex = (uint32_t)0U; \
            MenuManager_StepWashMode_ListIndex < MenuManager_StepWashMode_ChildMenuConf.childMenuNum; \
            MenuManager_StepWashMode_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_StepWashMode_IntValMapConf[MenuManager_StepWashMode_ListIndex].mapValue == tempWashMode)
        {
          break;
        }
      }
      
      MenuManager_StepWashMode_LastListIndex = MenuManager_StepWashMode_ListIndex;
      MenuManager_StepWashMode_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_StepWashMode_ListIndex, MenuManager_StepWashMode_ChildMenuConf.childMenuNum);

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
    MenuManager_StepWashMode_LcdShowMainTitle();
    MenuManager_StepWashMode_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepWashMode_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepWashMode_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashMode_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* inDataHierachy;
  Fsm_DataHierachyStruct* outDataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  if (MenuManager_StepWashMode_ListIndex == MENUMANAGER_STEPWASHMODE_LISTINDEX_CUSTOM)
  {
    inDataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
    inDataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_MODE;
    inDataHierachy->seqIdx = MenuManager_StepWashMode_SeqIdx;
    inDataHierachy->stepIdx = MenuManager_StepWashMode_StepIdx;

    pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)inDataHierachy;
  }
  else
  {
    outDataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
    outDataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_MODE;

    pFsmContext->dataHierachy = outDataHierachy;
  }

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashMode_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashMode_InternalState == MENUMANAGER_STEPWASHMODE_INTERNALSTATE_READY)
  {
    MenuManager_StepWashMode_InternalState = MENUMANAGER_STEPWASHMODE_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashMode_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashMode_InternalState != MENUMANAGER_STEPWASHMODE_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }

  MenuManager_StepWashMode_ListIndex = MENUMANAGER_STEPWASHMODE_LISTINDEX_UNDEFINED;
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashMode_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashMode_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepWashMode_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepWashMode_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepWashMode_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepWashMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashMode_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashMode_ListIndex < ((uint32_t)(MenuManager_StepWashMode_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepWashMode_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepWashMode_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_StepWashMode_ChildMenuConf.childMenuNum))
  {
    MenuManager_StepWashMode_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepWashMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepWashMode_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepWashMode_ButEventMapConf);

  switch (MenuManager_StepWashMode_InternalState)
  {
    case MENUMANAGER_STEPWASHMODE_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPWASHMODE_INTERNALSTATE_RUNNING:
    {
      if (MenuManager_StepWashMode_LastListIndex == MENUMANAGER_STEPWASHMODE_LISTINDEX_CUSTOM)
      {
        if (MenuManager_StepWashMode_ListIndex == MenuManager_StepWashMode_LastListIndex)
        {
          Fsm_TriggerEvent( &MenuManager_FsmContext, \
                            (Fsm_EventType)((*(MenuManager_StepWashMode_ChildMenuConf.childMenuCfg))[MenuManager_StepWashMode_ListIndex].childMenuEvent));
          
          MenuManager_StepWashMode_InternalState = MENUMANAGER_STEPWASHMODE_INTERNALSTATE_EXITING;

          break;
        }
      }

      ProgramManager_NormStepConfig_WashMode_SetData \
      ( \
        (uint8_t)MenuManager_StepWashMode_SeqIdx, \
        (uint8_t)MenuManager_StepWashMode_StepIdx, \
        &(MenuManager_StepWashMode_IntValMapConf[MenuManager_StepWashMode_ListIndex].mapValue) \
      );

      if ((uint8_t)MenuManager_StepWashMode_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_WashMode_GetData \
        ( \
          (uint8_t)MenuManager_StepWashMode_SeqIdx, \
          (uint8_t)MenuManager_StepWashMode_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepWashMode_StepIdx].washMode) \
        );
      }
      
      MenuManager_StepWashMode_InternalState = MENUMANAGER_STEPWASHMODE_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPWASHMODE_INTERNALSTATE_DONE:
    {
      MenuManager_StepWashMode_LcdShowDone();

      MenuManager_StepWashMode_InternalState = MENUMANAGER_STEPWASHMODE_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPWASHMODE_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepWashMode_SubTickHandler(void)
{
  if (MenuManager_StepWashMode_InternalState == MENUMANAGER_STEPWASHMODE_INTERNALSTATE_EXITING)
  {
    MenuManager_StepWashMode_Counter += (uint32_t)1U;

    if (MenuManager_StepWashMode_Counter >= MENUMANAGER_STEPWASHMODE_COUNTER_MAX)
    {
      MenuManager_StepWashMode_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_StepWashMode_ChildMenuConf.childMenuCfg))[MenuManager_StepWashMode_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
