/* 
 * File:   _1312000000_menu_StepExtractParam.c
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
#define MENUMANAGER_STEPEXTRACTPARAM_INTERNALDATALENGTH               (uint8_t)4U

#define MENUMANAGER_STEPEXTRACTPARAM_LISTINDEX_IDX                    0U
#define MENUMANAGER_STEPEXTRACTPARAM_CURPOS_IDX                       1U
#define MENUMANAGER_STEPEXTRACTPARAM_SEQIDX_IDX                       2U

#define MenuManager_StepExtractParam_ListIndex                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPEXTRACTPARAM_LISTINDEX_IDX)
#define MenuManager_StepExtractParam_CurPos                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPEXTRACTPARAM_CURPOS_IDX)
#define MenuManager_StepExtractParam_SeqIdx                           MenuManager_GetInternalDataPtr(MENUMANAGER_STEPEXTRACTPARAM_SEQIDX_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepBalanceTime_ChildTitleStr[] =      "BALANCE TIME";
static const uint8_t MenuManager_StepMidExtractTime_ChildTitleStr[] =   "MID EXT TIME";
static const uint8_t MenuManager_StepHighExtractTime1_ChildTitleStr[] = "HIGH EXT TIME 1";
static const uint8_t MenuManager_StepHighExtractTime2_ChildTitleStr[] = "HIGH EXT TIME 2";
static const uint8_t MenuManager_StepHighExtractTime3_ChildTitleStr[] = "HIGH EXT TIME 3";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepExtractParam_ChildMenu[5] =
{
  { &MenuManager_StepBalanceTime_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_StepMidExtractTime_ChildTitleStr,                    MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_StepHighExtractTime1_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_StepHighExtractTime2_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_StepHighExtractTime3_ChildTitleStr,                  MENUMANAGER_EVENT_SUBMENU_5             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepExtractParam_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)5U,
  .childMenuCfg               = &MenuManager_StepExtractParam_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepExtractParam_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepExtractParam_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepExtractParam_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepExtractParam_Entry               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepExtractParam_StartBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepExtractParam_StopBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepExtractParam_UpBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepExtractParam_DownBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepExtractParam_StateMachine[10] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepExtractParam_Entry                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_BALANCE_TIME         ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_STEP_MID_EXTRACT_TIME     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_1  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_2  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_3  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepExtractParam_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepExtractParam_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepExtractParam_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepExtractParam_StopBut,   MENUMANAGER_STATE_PROGRAM_STEP_SETUP        )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepExtractParam_LcdShowMainTitle(void);
static void MenuManager_StepExtractParam_LcdShowList(void);

static void MenuManager_StepExtractParam_SubMainFunction(void);
static void MenuManager_StepExtractParam_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepExtractParam_LcdShowMainTitle(void)
{
  uint8_t tempStr[20];

  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();

  sprintf((char *)tempStr, (const char *)MenuManager_Common_ProgramStepExtStr, MenuManager_StepExtractParam_SeqIdx + 1U);

  MenuManager_Common_LcdShowMainTitle(tempStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepExtractParam_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepExtractParam_ListIndex, MenuManager_StepExtractParam_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepExtractParam_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepExtractParam_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepExtractParam_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepExtractParam_ListIndex, MenuManager_StepExtractParam_CurPos, MenuManager_StepExtractParam_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepExtractParam_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_BALANCE_TIME)        || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_MID_EXTRACT_TIME)    || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_1) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_2) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_HIGH_EXTRACT_TIME_3))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_PROGRAM_STEP_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPEXTRACTPARAM_INTERNALDATALENGTH);

      MenuManager_StepExtractParam_ListIndex      = (uint32_t)0U;
      MenuManager_StepExtractParam_CurPos         = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
      MenuManager_StepExtractParam_SeqIdx         = enterDataHierachy->seqIdx;

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
    MenuManager_StepExtractParam_LcdShowMainTitle();
    MenuManager_StepExtractParam_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepExtractParam_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepExtractParam_SubTickHandler;
    
    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepExtractParam_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_EXTRACT_PARAM;
  dataHierachy->seqIdx = MenuManager_StepExtractParam_SeqIdx;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_StepExtractParam_ChildMenuConf.childMenuCfg))[MenuManager_StepExtractParam_ListIndex].childMenuEvent));

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepExtractParam_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_EXTRACT_PARAM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepExtractParam_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepExtractParam_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepExtractParam_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepExtractParam_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepExtractParam_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepExtractParam_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepExtractParam_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepExtractParam_ListIndex < ((uint32_t)(MenuManager_StepExtractParam_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepExtractParam_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepExtractParam_CurPos < MenuManager_Common_LcdMaxCurPos((uint32_t)(MenuManager_StepExtractParam_ChildMenuConf.childMenuNum)))
  {
    MenuManager_StepExtractParam_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepExtractParam_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepExtractParam_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepExtractParam_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_StepExtractParam_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
