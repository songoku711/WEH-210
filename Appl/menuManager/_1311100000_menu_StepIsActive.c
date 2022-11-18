/* 
 * File:   _1311100000_menu_StepIsActive.c
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
#define MENUMANAGER_STEPISACTIVE_INTERNALDATALENGTH                   (uint8_t)6U

#define MENUMANAGER_STEPISACTIVE_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_STEPISACTIVE_COUNTER_IDX                          1U
#define MENUMANAGER_STEPISACTIVE_LISTINDEX_IDX                        2U
#define MENUMANAGER_STEPISACTIVE_CURPOS_IDX                           3U
#define MENUMANAGER_STEPISACTIVE_SEQIDX_IDX                           4U
#define MENUMANAGER_STEPISACTIVE_STEPIDX_IDX                          5U

#define MenuManager_StepIsActive_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPISACTIVE_INTERNALSTATE_IDX)
#define MenuManager_StepIsActive_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPISACTIVE_COUNTER_IDX)
#define MenuManager_StepIsActive_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPISACTIVE_LISTINDEX_IDX)
#define MenuManager_StepIsActive_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPISACTIVE_CURPOS_IDX)
#define MenuManager_StepIsActive_SeqIdx                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPISACTIVE_SEQIDX_IDX)
#define MenuManager_StepIsActive_StepIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPISACTIVE_STEPIDX_IDX)



#define MENUMANAGER_STEPISACTIVE_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_STEPISACTIVE_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_STEPISACTIVE_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_STEPISACTIVE_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_STEPISACTIVE_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_STEPISACTIVE_LISTINDEX_ON                         (uint32_t)0U
#define MENUMANAGER_STEPISACTIVE_LISTINDEX_OFF                        (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  bool                        mapValue;
} MenuManager_StepIsActive_IntValMapStruct;

static MenuManager_StepIsActive_IntValMapStruct MenuManager_StepIsActive_IntValMapConf[2] =
{
  { MENUMANAGER_STEPISACTIVE_LISTINDEX_ON,                            true                                    },
  { MENUMANAGER_STEPISACTIVE_LISTINDEX_OFF,                           false                                   }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepIsActive_MainTitleStr[] =        "STEP ACTIVE";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepIsActive_ChildMenu[2] =
{
  { &MenuManager_Common_OnStr,                                        MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_Common_OffStr,                                       MENUMANAGER_EVENT_PREV                  }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepIsActive_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_StepIsActive_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepIsActive_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepIsActive_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepIsActive_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepIsActive_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepIsActive_Exit                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepIsActive_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepIsActive_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepIsActive_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepIsActive_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepIsActive_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepIsActive_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepIsActive_Exit                                                   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepIsActive_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepIsActive_StopBut,       MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepIsActive_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepIsActive_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepIsActive_LcdShowMainTitle(void);
static void MenuManager_StepIsActive_LcdShowList(void);
static void MenuManager_StepIsActive_LcdShowDone(void);

static void MenuManager_StepIsActive_SubMainFunction(void);
static void MenuManager_StepIsActive_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepIsActive_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepIsActive_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepIsActive_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepIsActive_ListIndex, MenuManager_StepIsActive_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepIsActive_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepIsActive_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepIsActive_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepIsActive_ListIndex, MenuManager_StepIsActive_CurPos, MenuManager_StepIsActive_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepIsActive_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_StepIsActive_ChildMenuConf.childMenuCfg))[MenuManager_StepIsActive_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_StepIsActive_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_StepIsActive_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepIsActive_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  bool tempIsActive;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_NORM_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPISACTIVE_INTERNALDATALENGTH);

      MenuManager_StepIsActive_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepIsActive_StepIdx = enterDataHierachy->stepIdx;

      MenuManager_StepIsActive_InternalState = MENUMANAGER_STEPISACTIVE_INTERNALSTATE_READY;
      MenuManager_StepIsActive_Counter = (uint32_t)0U;

      ProgramManager_NormStepConfig_IsActive_GetData((uint8_t)MenuManager_StepIsActive_SeqIdx, (uint8_t)MenuManager_StepIsActive_StepIdx, &tempIsActive);
      
      for ( MenuManager_StepIsActive_ListIndex = (uint32_t)0U; \
            MenuManager_StepIsActive_ListIndex < MenuManager_StepIsActive_ChildMenuConf.childMenuNum; \
            MenuManager_StepIsActive_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_StepIsActive_IntValMapConf[MenuManager_StepIsActive_ListIndex].mapValue == tempIsActive)
        {
          break;
        }
      }
      
      MenuManager_StepIsActive_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_StepIsActive_ListIndex, MenuManager_StepIsActive_ChildMenuConf.childMenuNum);

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
    MenuManager_StepIsActive_LcdShowMainTitle();
    MenuManager_StepIsActive_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepIsActive_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepIsActive_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepIsActive_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_IS_ACTIVE;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepIsActive_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepIsActive_InternalState == MENUMANAGER_STEPISACTIVE_INTERNALSTATE_READY)
  {
    MenuManager_StepIsActive_InternalState = MENUMANAGER_STEPISACTIVE_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepIsActive_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepIsActive_InternalState != MENUMANAGER_STEPISACTIVE_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepIsActive_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepIsActive_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepIsActive_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepIsActive_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepIsActive_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepIsActive_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepIsActive_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepIsActive_ListIndex < ((uint32_t)(MenuManager_StepIsActive_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepIsActive_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepIsActive_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_StepIsActive_ChildMenuConf.childMenuNum))
  {
    MenuManager_StepIsActive_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepIsActive_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepIsActive_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepIsActive_ButEventMapConf);

  switch (MenuManager_StepIsActive_InternalState)
  {
    case MENUMANAGER_STEPISACTIVE_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPISACTIVE_INTERNALSTATE_RUNNING:
    {
      ProgramManager_NormStepConfig_IsActive_SetData \
      ( \
        (uint8_t)MenuManager_StepIsActive_SeqIdx, \
        (uint8_t)MenuManager_StepIsActive_StepIdx, \
        &(MenuManager_StepIsActive_IntValMapConf[MenuManager_StepIsActive_ListIndex].mapValue) \
      );

      if ((uint8_t)MenuManager_StepIsActive_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_IsActive_GetData \
        ( \
          (uint8_t)MenuManager_StepIsActive_SeqIdx, \
          (uint8_t)MenuManager_StepIsActive_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepIsActive_StepIdx].isActive) \
        );
      }
      
      MenuManager_StepIsActive_InternalState = MENUMANAGER_STEPISACTIVE_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPISACTIVE_INTERNALSTATE_DONE:
    {
      MenuManager_StepIsActive_LcdShowDone();

      MenuManager_StepIsActive_InternalState = MENUMANAGER_STEPISACTIVE_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPISACTIVE_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepIsActive_SubTickHandler(void)
{
  if (MenuManager_StepIsActive_InternalState == MENUMANAGER_STEPISACTIVE_INTERNALSTATE_EXITING)
  {
    MenuManager_StepIsActive_Counter += (uint32_t)1U;

    if (MenuManager_StepIsActive_Counter >= MENUMANAGER_STEPISACTIVE_COUNTER_MAX)
    {
      MenuManager_StepIsActive_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_StepIsActive_ChildMenuConf.childMenuCfg))[MenuManager_StepIsActive_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
