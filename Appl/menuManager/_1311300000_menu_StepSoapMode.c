/* 
 * File:   _1311300000_menu_StepSoapMode.c
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
#define MENUMANAGER_STEPSOAPMODE_INTERNALDATALENGTH                   (uint8_t)7U

#define MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_STEPSOAPMODE_COUNTER_IDX                          1U
#define MENUMANAGER_STEPSOAPMODE_LISTINDEX_IDX                        2U
#define MENUMANAGER_STEPSOAPMODE_CURPOS_IDX                           3U
#define MENUMANAGER_STEPSOAPMODE_SEQIDX_IDX                           4U
#define MENUMANAGER_STEPSOAPMODE_STEPIDX_IDX                          5U
#define MENUMANAGER_STEPSOAPMODE_SOAPMODE_IDX                         6U

#define MenuManager_StepSoapMode_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_IDX)
#define MenuManager_StepSoapMode_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPSOAPMODE_COUNTER_IDX)
#define MenuManager_StepSoapMode_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_STEPSOAPMODE_LISTINDEX_IDX)
#define MenuManager_StepSoapMode_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPSOAPMODE_CURPOS_IDX)
#define MenuManager_StepSoapMode_SeqIdx                               MenuManager_GetInternalDataPtr(MENUMANAGER_STEPSOAPMODE_SEQIDX_IDX)
#define MenuManager_StepSoapMode_StepIdx                              MenuManager_GetInternalDataPtr(MENUMANAGER_STEPSOAPMODE_STEPIDX_IDX)
#define MenuManager_StepSoapMode_SoapMode                             MenuManager_GetInternalDataPtr(MENUMANAGER_STEPSOAPMODE_SOAPMODE_IDX)



#define MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_STEPSOAPMODE_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_STEPSOAPMODE_LISTINDEX_SOAP_1                     (uint32_t)0U
#define MENUMANAGER_STEPSOAPMODE_LISTINDEX_SOAP_2                     (uint32_t)1U
#define MENUMANAGER_STEPSOAPMODE_LISTINDEX_SOAP_3                     (uint32_t)2U

#define MENUMANAGER_STEPSOAPMODE_LCD_ELEMENT_MENU_LENGTH              (10U)

#define MENUMANAGER_STEPSOAPMODE_LCD_LEFT_ARROW_XPOS                  (12U)
#define MENUMANAGER_STEPSOAPMODE_LCD_SOAP_STAT_XPOS                   (14U)
#define MENUMANAGER_STEPSOAPMODE_LCD_RIGHT_ARROW_XPOS                 (18U)

#define MENUMANAGER_STEPSOAPMODE_LCD_DONE_XPOS                        (9U)
#define MENUMANAGER_STEPSOAPMODE_LCD_DONE_YPOS                        (3U)



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  uint32_t                    mapValue;
} MenuManager_StepSoapMode_IntValMapStruct;

static MenuManager_StepSoapMode_IntValMapStruct MenuManager_StepSoapMode_IntValMapConf[3] =
{
  { MENUMANAGER_STEPSOAPMODE_LISTINDEX_SOAP_1,                        (uint32_t)0x01U                         },
  { MENUMANAGER_STEPSOAPMODE_LISTINDEX_SOAP_2,                        (uint32_t)0x02U                         },
  { MENUMANAGER_STEPSOAPMODE_LISTINDEX_SOAP_3,                        (uint32_t)0x04U                         }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepSoapMode_Soap1Str[] =            "SUPPLY 1";
static const uint8_t MenuManager_StepSoapMode_Soap2Str[] =            "SUPPLY 2";
static const uint8_t MenuManager_StepSoapMode_Soap3Str[] =            "SUPPLY 3";

static const uint8_t MenuManager_StepSoapMode_MainTitleStr[] =        "SUPPLY MODE";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepSoapMode_ChildMenu[3] =
{
  { &MenuManager_StepSoapMode_Soap1Str,                               (MenuManager_EventType)FSM_DEFAULTEVENT_NONE                },
  { &MenuManager_StepSoapMode_Soap2Str,                               (MenuManager_EventType)FSM_DEFAULTEVENT_NONE                },
  { &MenuManager_StepSoapMode_Soap3Str,                               (MenuManager_EventType)FSM_DEFAULTEVENT_NONE                }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepSoapMode_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)3U,
  .childMenuCfg               = &MenuManager_StepSoapMode_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepSoapMode_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepSoapMode_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_StepSoapMode_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepSoapMode_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepSoapMode_Exit                   (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepSoapMode_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepSoapMode_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepSoapMode_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepSoapMode_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepSoapMode_AddBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepSoapMode_SubBut                 (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepSoapMode_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepSoapMode_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_StepSoapMode_Exit                                                   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_PROGRAM_MAIN_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepSoapMode_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepSoapMode_StopBut,       MENUMANAGER_STATE_PROGRAM_MAIN_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepSoapMode_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepSoapMode_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_StepSoapMode_AddBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_StepSoapMode_SubBut                                                 )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepSoapMode_LcdShowMainTitle(void);
static void MenuManager_StepSoapMode_LcdShowList(void);
static void MenuManager_StepSoapMode_LcdShowDone(void);

static void MenuManager_StepSoapMode_SubMainFunction(void);
static void MenuManager_StepSoapMode_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepSoapMode_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepSoapMode_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepSoapMode_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepSoapMode_ListIndex, MenuManager_StepSoapMode_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepSoapMode_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepSoapMode_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_STEPSOAPMODE_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );

    LCD_SetCursorPos(MENUMANAGER_STEPSOAPMODE_LCD_LEFT_ARROW_XPOS, currentCursorPos, LCD_CURSOR_BY_FONT);
    LCD_PutChar('<');

    LCD_SetCursorPos(MENUMANAGER_STEPSOAPMODE_LCD_RIGHT_ARROW_XPOS, currentCursorPos, LCD_CURSOR_BY_FONT);
    LCD_PutChar('>');

    LCD_SetCursorPos(MENUMANAGER_STEPSOAPMODE_LCD_SOAP_STAT_XPOS, currentCursorPos, LCD_CURSOR_BY_FONT);

    if ((uint32_t)0U != (MenuManager_StepSoapMode_SoapMode & MenuManager_StepSoapMode_IntValMapConf[currentListIndex].mapValue))
    {
      LCD_PutString((uint8_t *)MenuManager_Common_OnStr);
    }
    else
    {
      LCD_PutString((uint8_t *)MenuManager_Common_OffStr);
    }
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepSoapMode_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepSoapMode_ListIndex, MenuManager_StepSoapMode_CurPos, MenuManager_StepSoapMode_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_StepSoapMode_LcdShowDone(void)
{
  MenuManager_Common_LcdClearElementMenuDynamic();

  LCD_SetCursorPos(MENUMANAGER_STEPSOAPMODE_LCD_DONE_XPOS, MENUMANAGER_STEPSOAPMODE_LCD_DONE_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_DoneStr);

  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  uint8_t tempSoapMode;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_MAIN_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPSOAPMODE_INTERNALDATALENGTH);

      MenuManager_StepSoapMode_SeqIdx = enterDataHierachy->seqIdx;
      MenuManager_StepSoapMode_StepIdx = enterDataHierachy->stepIdx;

      MenuManager_StepSoapMode_InternalState = MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_READY;
      MenuManager_StepSoapMode_Counter = (uint32_t)0U;

      ProgramManager_NormStepConfig_SoapMode_GetData((uint8_t)MenuManager_StepSoapMode_SeqIdx, (uint8_t)MenuManager_StepSoapMode_StepIdx, &tempSoapMode);
      
      MenuManager_StepSoapMode_SoapMode = (uint32_t)tempSoapMode;
      
      MenuManager_StepSoapMode_ListIndex = (uint32_t)0U;
      MenuManager_StepSoapMode_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_StepSoapMode_ListIndex, MenuManager_StepSoapMode_ChildMenuConf.childMenuNum);

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
    MenuManager_StepSoapMode_LcdShowMainTitle();
    MenuManager_StepSoapMode_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepSoapMode_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepSoapMode_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_SOAP_MODE;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepSoapMode_InternalState == MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_READY)
  {
    MenuManager_StepSoapMode_InternalState = MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepSoapMode_InternalState != MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepSoapMode_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepSoapMode_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepSoapMode_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepSoapMode_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepSoapMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepSoapMode_ListIndex < ((uint32_t)(MenuManager_StepSoapMode_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepSoapMode_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepSoapMode_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_StepSoapMode_ChildMenuConf.childMenuNum))
  {
    MenuManager_StepSoapMode_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepSoapMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_StepSoapMode_SoapMode ^= MenuManager_StepSoapMode_IntValMapConf[MenuManager_StepSoapMode_ListIndex].mapValue;

  MenuManager_StepSoapMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepSoapMode_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_StepSoapMode_SoapMode ^= MenuManager_StepSoapMode_IntValMapConf[MenuManager_StepSoapMode_ListIndex].mapValue;

  MenuManager_StepSoapMode_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepSoapMode_SubMainFunction(void)
{
  uint8_t tempSoapMode;

  MenuManager_Common_ButEventMapHandler(&MenuManager_StepSoapMode_ButEventMapConf);

  switch (MenuManager_StepSoapMode_InternalState)
  {
    case MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_RUNNING:
    {
      tempSoapMode = (uint8_t)MenuManager_StepSoapMode_SoapMode;

      ProgramManager_NormStepConfig_SoapMode_SetData \
      ( \
        (uint8_t)MenuManager_StepSoapMode_SeqIdx, \
        (uint8_t)MenuManager_StepSoapMode_StepIdx, \
        &tempSoapMode \
      );

      if ((uint8_t)MenuManager_StepSoapMode_SeqIdx == ProgramManager_gAutoSeqConfig.sequenceIndex)
      {
        ProgramManager_NormStepConfig_SoapMode_GetData \
        ( \
          (uint8_t)MenuManager_StepSoapMode_SeqIdx, \
          (uint8_t)MenuManager_StepSoapMode_StepIdx, \
          &((ProgramManager_gAutoSeqConfig.normStep)[MenuManager_StepSoapMode_StepIdx].soapMode) \
        );
      }
      
      MenuManager_StepSoapMode_InternalState = MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_DONE:
    {
      MenuManager_StepSoapMode_LcdShowDone();

      MenuManager_StepSoapMode_InternalState = MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_StepSoapMode_SubTickHandler(void)
{
  if (MenuManager_StepSoapMode_InternalState == MENUMANAGER_STEPSOAPMODE_INTERNALSTATE_EXITING)
  {
    MenuManager_StepSoapMode_Counter += (uint32_t)1U;

    if (MenuManager_StepSoapMode_Counter >= MENUMANAGER_STEPSOAPMODE_COUNTER_MAX)
    {
      MenuManager_StepSoapMode_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_PREV);
    }
  }
}



#ifdef  __cplusplus
}
#endif
