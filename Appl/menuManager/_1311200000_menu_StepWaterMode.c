/* 
 * File:   _1311200000_menu_StepWaterMode.c
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
#define MENUMANAGER_STEPWATERMODE_INTERNALDATALENGTH                  (uint8_t)6U

#define MENUMANAGER_STEPWATERMODE_INTERNALSTATE_IDX                   0U
#define MENUMANAGER_STEPWATERMODE_COUNTER_IDX                         1U
#define MENUMANAGER_STEPWATERMODE_LISTINDEX_IDX                       2U
#define MENUMANAGER_STEPWATERMODE_CURPOS_IDX                          3U
#define MENUMANAGER_STEPWATERMODE_SEQIDX_IDX                          4U
#define MENUMANAGER_STEPWATERMODE_STEPIDX_IDX                         5U

#define MenuManager_StepWaterMode_InternalState                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWATERMODE_INTERNALSTATE_IDX)
#define MenuManager_StepWaterMode_Counter                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWATERMODE_COUNTER_IDX)
#define MenuManager_StepWaterMode_ListIndex                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWATERMODE_LISTINDEX_IDX)
#define MenuManager_StepWaterMode_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWATERMODE_CURPOS_IDX)
#define MenuManager_StepWaterMode_SeqIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWATERMODE_SEQIDX_IDX)
#define MenuManager_StepWaterMode_StepIdx                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWATERMODE_STEPIDX_IDX)



#define MENUMANAGER_STEPWATERMODE_INTERNALSTATE_READY                 (uint32_t)0U
#define MENUMANAGER_STEPWATERMODE_INTERNALSTATE_RUNNING               (uint32_t)1U
#define MENUMANAGER_STEPWATERMODE_INTERNALSTATE_DONE                  (uint32_t)2U
#define MENUMANAGER_STEPWATERMODE_INTERNALSTATE_EXITING               (uint32_t)3U

#define MENUMANAGER_STEPWATERMODE_COUNTER_MAX                         (uint32_t)50U

#define MENUMANAGER_STEPWATERMODE_LISTINDEX_COLD                      (uint32_t)0U
#define MENUMANAGER_STEPWATERMODE_LISTINDEX_HOT                       (uint32_t)1U
#define MENUMANAGER_STEPWATERMODE_LISTINDEX_COLD_HOT                  (uint32_t)2U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  uint8_t                     mapValue;
} MenuManager_StepWaterMode_IntValMapStruct;

static MenuManager_StepWaterMode_IntValMapStruct MenuManager_StepWaterMode_IntValMapConf[3] =
{
  { MENUMANAGER_STEPWATERMODE_LISTINDEX_COLD,                         (uint8_t)0x01U                          },
  { MENUMANAGER_STEPWATERMODE_LISTINDEX_HOT,                          (uint8_t)0x02U                          },
  { MENUMANAGER_STEPWATERMODE_LISTINDEX_COLD_HOT,                     (uint8_t)0x03U                          }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepWaterMode_ColdStr[] =            "COLD";
static const uint8_t MenuManager_StepWaterMode_HotStr[] =             "HOT";
static const uint8_t MenuManager_StepWaterMode_ColdHotStr[] =         "COLD & HOT";

static const uint8_t MenuManager_StepWaterMode_MainTitleStr[] =       "WATER MODE";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepWaterMode_ChildMenu[3] =
{
  { &MenuManager_StepWaterMode_ColdStr,                               MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepWaterMode_HotStr,                                MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepWaterMode_ColdHotStr,                            MENUMANAGER_EVENT_PREV                  }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepWaterMode_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)3U,
  .childMenuCfg               = &MenuManager_StepWaterMode_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepWaterMode_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepWaterMode_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepWaterMode_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepWaterMode_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWaterMode_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWaterMode_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWaterMode_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWaterMode_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWaterMode_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepWaterMode_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepWaterMode_Entry                                                 ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepWaterMode_Exit                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepWaterMode_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepWaterMode_StopBut,      MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepWaterMode_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepWaterMode_DownBut                                               )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepWaterMode_LcdShowMainTitle(void);
static void MenuManager_StepWaterMode_LcdShowList(void);
static void MenuManager_StepWaterMode_LcdShowDone(void);

static void MenuManager_StepWaterMode_SubMainFunction(void);
static void MenuManager_StepWaterMode_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepWaterMode_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepWaterMode_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepWaterMode_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepWaterMode_ListIndex, MenuManager_StepWaterMode_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepWaterMode_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepWaterMode_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepWaterMode_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepWaterMode_ListIndex, MenuManager_StepWaterMode_CurPos, MenuManager_StepWaterMode_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepWaterMode_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_StepWaterMode_ChildMenuConf.childMenuCfg))[MenuManager_StepWaterMode_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_StepWaterMode_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_StepWaterMode_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWaterMode_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint8_t tempWaterMode;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_NORM_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPWATERMODE_INTERNALDATALENGTH);

      MenuManager_StepWaterMode_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepWaterMode_StepIdx = enterDataHierachy->stepIdx;

      MenuManager_StepWaterMode_InternalState = MENUMANAGER_STEPWATERMODE_INTERNALSTATE_READY;
      MenuManager_StepWaterMode_Counter = (uint32_t)0U;

      ProgramManager_NormStepConfig_WaterMode_GetData((uint8_t)MenuManager_StepWaterMode_SeqIdx, (uint8_t)MenuManager_StepWaterMode_StepIdx, &tempWaterMode);
      
      for ( MenuManager_StepWaterMode_ListIndex = (uint32_t)0U; \
            MenuManager_StepWaterMode_ListIndex < MenuManager_StepWaterMode_ChildMenuConf.childMenuNum; \
            MenuManager_StepWaterMode_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_StepWaterMode_IntValMapConf[MenuManager_StepWaterMode_ListIndex].mapValue == tempWaterMode)
        {
          break;
        }
      }
      
      MenuManager_StepWaterMode_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_StepWaterMode_ListIndex, MenuManager_StepWaterMode_ChildMenuConf.childMenuNum);

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
    MenuManager_StepWaterMode_LcdShowMainTitle();
    MenuManager_StepWaterMode_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepWaterMode_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepWaterMode_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWaterMode_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_WATER_MODE;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWaterMode_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWaterMode_InternalState == MENUMANAGER_STEPWATERMODE_INTERNALSTATE_READY)
  {
    MenuManager_StepWaterMode_InternalState = MENUMANAGER_STEPWATERMODE_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWaterMode_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWaterMode_InternalState != MENUMANAGER_STEPWATERMODE_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWaterMode_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWaterMode_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepWaterMode_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepWaterMode_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepWaterMode_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepWaterMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWaterMode_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWaterMode_ListIndex < ((uint32_t)(MenuManager_StepWaterMode_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepWaterMode_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepWaterMode_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_StepWaterMode_ChildMenuConf.childMenuNum))
  {
    MenuManager_StepWaterMode_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepWaterMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepWaterMode_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepWaterMode_ButEventMapConf);

  switch (MenuManager_StepWaterMode_InternalState)
  {
    case MENUMANAGER_STEPWATERMODE_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPWATERMODE_INTERNALSTATE_RUNNING:
    {
      ProgramManager_NormStepConfig_WaterMode_SetData \
      ( \
        (uint8_t)MenuManager_StepWaterMode_SeqIdx, \
        (uint8_t)MenuManager_StepWaterMode_StepIdx, \
        &(MenuManager_StepWaterMode_IntValMapConf[MenuManager_StepWaterMode_ListIndex].mapValue) \
      );

      if ((uint8_t)MenuManager_StepWaterMode_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_WaterMode_GetData \
        ( \
          (uint8_t)MenuManager_StepWaterMode_SeqIdx, \
          (uint8_t)MenuManager_StepWaterMode_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepWaterMode_StepIdx].waterMode) \
        );
      }
      
      MenuManager_StepWaterMode_InternalState = MENUMANAGER_STEPWATERMODE_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPWATERMODE_INTERNALSTATE_DONE:
    {
      MenuManager_StepWaterMode_LcdShowDone();

      MenuManager_StepWaterMode_InternalState = MENUMANAGER_STEPWATERMODE_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPWATERMODE_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepWaterMode_SubTickHandler(void)
{
  if (MenuManager_StepWaterMode_InternalState == MENUMANAGER_STEPWATERMODE_INTERNALSTATE_EXITING)
  {
    MenuManager_StepWaterMode_Counter += (uint32_t)1U;

    if (MenuManager_StepWaterMode_Counter >= MENUMANAGER_STEPWATERMODE_COUNTER_MAX)
    {
      MenuManager_StepWaterMode_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_StepWaterMode_ChildMenuConf.childMenuCfg))[MenuManager_StepWaterMode_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
