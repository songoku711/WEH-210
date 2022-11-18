/* 
 * File:   _1311400000_menu_StepTempMode.c
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
#define MENUMANAGER_STEPTEMPMODE_INTERNALDATALENGTH                   (uint8_t)7U

#define MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_STEPTEMPMODE_COUNTER_IDX                          1U
#define MENUMANAGER_STEPTEMPMODE_LISTINDEX_IDX                        2U
#define MENUMANAGER_STEPTEMPMODE_LASTLISTINDEX_IDX                    3U
#define MENUMANAGER_STEPTEMPMODE_CURPOS_IDX                           4U
#define MENUMANAGER_STEPTEMPMODE_SEQIDX_IDX                           5U
#define MENUMANAGER_STEPTEMPMODE_STEPIDX_IDX                          6U

#define MenuManager_StepTempMode_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_IDX)
#define MenuManager_StepTempMode_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODE_COUNTER_IDX)
#define MenuManager_StepTempMode_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODE_LISTINDEX_IDX)
#define MenuManager_StepTempMode_LastListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODE_LASTLISTINDEX_IDX)
#define MenuManager_StepTempMode_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODE_CURPOS_IDX)
#define MenuManager_StepTempMode_SeqIdx                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODE_SEQIDX_IDX)
#define MenuManager_StepTempMode_StepIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPTEMPMODE_STEPIDX_IDX)



#define MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_STEPTEMPMODE_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_STEPTEMPMODE_LISTINDEX_DEFAULT                    (uint32_t)0U
#define MENUMANAGER_STEPTEMPMODE_LISTINDEX_CUSTOM                     (uint32_t)1U
#define MENUMANAGER_STEPTEMPMODE_LISTINDEX_UNDEFINED                  (uint32_t)255U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_TempModeType mapValue;
} MenuManager_StepTempMode_IntValMapStruct;

static MenuManager_StepTempMode_IntValMapStruct MenuManager_StepTempMode_IntValMapConf[2] =
{
  { MENUMANAGER_STEPTEMPMODE_LISTINDEX_DEFAULT,                       PROGRAMMANAGER_TEMP_MODE_DEFAULT        },
  { MENUMANAGER_STEPTEMPMODE_LISTINDEX_CUSTOM,                        PROGRAMMANAGER_TEMP_MODE_CUSTOM         }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepTempMode_DefaultStr[] =          "DEFAULT";
static const uint8_t MenuManager_StepTempMode_CustomStr[] =           "CUSTOM";

static const uint8_t MenuManager_StepTempMode_MainTitleStr[] =        "TEMP MODE";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepTempMode_ChildMenu[2] =
{
  { &MenuManager_StepTempMode_DefaultStr,                             MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_StepTempMode_CustomStr,                              MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepTempMode_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_StepTempMode_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepTempMode_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepTempMode_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepTempMode_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepTempMode_Entry                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempMode_Exit                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempMode_StartBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempMode_StopBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempMode_UpBut                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepTempMode_DownBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepTempMode_StateMachine[8] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepTempMode_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepTempMode_Exit                                                   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_TEMP_MODE_CUSTOM ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepTempMode_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepTempMode_StopBut,       MENUMANAGER_STATE_STEP_NORM_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepTempMode_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepTempMode_DownBut                                                )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepTempMode_LcdShowMainTitle(void);
static void MenuManager_StepTempMode_LcdShowList(void);
static void MenuManager_StepTempMode_LcdShowDone(void);

static void MenuManager_StepTempMode_SubMainFunction(void);
static void MenuManager_StepTempMode_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepTempMode_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepTempMode_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepTempMode_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepTempMode_ListIndex, MenuManager_StepTempMode_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepTempMode_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepTempMode_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepTempMode_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepTempMode_ListIndex, MenuManager_StepTempMode_CurPos, MenuManager_StepTempMode_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepTempMode_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_StepTempMode_ChildMenuConf.childMenuCfg))[MenuManager_StepTempMode_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_StepTempMode_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_StepTempMode_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempMode_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  ProgramManager_TempModeType tempTempMode;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_NORM_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPTEMPMODE_INTERNALDATALENGTH);

      MenuManager_StepTempMode_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepTempMode_StepIdx = enterDataHierachy->stepIdx;

      MenuManager_StepTempMode_InternalState = MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_READY;
      MenuManager_StepTempMode_Counter = (uint32_t)0U;

      ProgramManager_NormStepConfig_TempMode_GetData((uint8_t)MenuManager_StepTempMode_SeqIdx, (uint8_t)MenuManager_StepTempMode_StepIdx, &tempTempMode);
      
      for ( MenuManager_StepTempMode_ListIndex = (uint32_t)0U; \
            MenuManager_StepTempMode_ListIndex < MenuManager_StepTempMode_ChildMenuConf.childMenuNum; \
            MenuManager_StepTempMode_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_StepTempMode_IntValMapConf[MenuManager_StepTempMode_ListIndex].mapValue == tempTempMode)
        {
          break;
        }
      }
      
      MenuManager_StepTempMode_LastListIndex = MenuManager_StepTempMode_ListIndex;
      MenuManager_StepTempMode_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_StepTempMode_ListIndex, MenuManager_StepTempMode_ChildMenuConf.childMenuNum);

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
    MenuManager_StepTempMode_LcdShowMainTitle();
    MenuManager_StepTempMode_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepTempMode_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepTempMode_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempMode_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* inDataHierachy;
  Fsm_DataHierachyStruct* outDataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  if (MenuManager_StepTempMode_ListIndex == MENUMANAGER_STEPTEMPMODE_LISTINDEX_CUSTOM)
  {
    inDataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
    inDataHierachy->dataId = MENUMANAGER_STATE_STEP_TEMP_MODE;
    inDataHierachy->seqIdx = MenuManager_StepTempMode_SeqIdx;
    inDataHierachy->stepIdx = MenuManager_StepTempMode_StepIdx;

    pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)inDataHierachy;
  }
  else
  {
    outDataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
    outDataHierachy->dataId = MENUMANAGER_STATE_STEP_TEMP_MODE;

    pFsmContext->dataHierachy = outDataHierachy;
  }

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempMode_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempMode_InternalState == MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_READY)
  {
    MenuManager_StepTempMode_InternalState = MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempMode_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempMode_InternalState != MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }

  MenuManager_StepTempMode_ListIndex = MENUMANAGER_STEPTEMPMODE_LISTINDEX_UNDEFINED;
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempMode_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempMode_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepTempMode_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepTempMode_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepTempMode_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepTempMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepTempMode_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepTempMode_ListIndex < ((uint32_t)(MenuManager_StepTempMode_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepTempMode_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepTempMode_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_StepTempMode_ChildMenuConf.childMenuNum))
  {
    MenuManager_StepTempMode_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepTempMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepTempMode_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepTempMode_ButEventMapConf);

  switch (MenuManager_StepTempMode_InternalState)
  {
    case MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_RUNNING:
    {
      if (MenuManager_StepTempMode_LastListIndex == MENUMANAGER_STEPTEMPMODE_LISTINDEX_CUSTOM)
      {
        if (MenuManager_StepTempMode_ListIndex == MenuManager_StepTempMode_LastListIndex)
        {
          Fsm_TriggerEvent( &MenuManager_FsmContext, \
                            (Fsm_EventType)((*(MenuManager_StepTempMode_ChildMenuConf.childMenuCfg))[MenuManager_StepTempMode_ListIndex].childMenuEvent));
          
          MenuManager_StepTempMode_InternalState = MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_EXITING;

          break;
        }
      }

      ProgramManager_NormStepConfig_TempMode_SetData \
      ( \
        (uint8_t)MenuManager_StepTempMode_SeqIdx, \
        (uint8_t)MenuManager_StepTempMode_StepIdx, \
        &(MenuManager_StepTempMode_IntValMapConf[MenuManager_StepTempMode_ListIndex].mapValue) \
      );

      if ((uint8_t)MenuManager_StepTempMode_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_TempMode_GetData \
        ( \
          (uint8_t)MenuManager_StepTempMode_SeqIdx, \
          (uint8_t)MenuManager_StepTempMode_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepTempMode_StepIdx].tempMode) \
        );
      }
      
      MenuManager_StepTempMode_InternalState = MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_DONE:
    {
      MenuManager_StepTempMode_LcdShowDone();

      MenuManager_StepTempMode_InternalState = MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepTempMode_SubTickHandler(void)
{
  if (MenuManager_StepTempMode_InternalState == MENUMANAGER_STEPTEMPMODE_INTERNALSTATE_EXITING)
  {
    MenuManager_StepTempMode_Counter += (uint32_t)1U;

    if (MenuManager_StepTempMode_Counter >= MENUMANAGER_STEPTEMPMODE_COUNTER_MAX)
    {
      MenuManager_StepTempMode_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_StepTempMode_ChildMenuConf.childMenuCfg))[MenuManager_StepTempMode_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
