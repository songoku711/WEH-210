/* 
 * File:   _1311410000_menu_StepWashModeCustom.c
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
#define MENUMANAGER_STEPWASHMODECUSTOM_INTERNALDATALENGTH             (uint8_t)5U

#define MENUMANAGER_STEPWASHMODECUSTOM_LISTINDEX_IDX                  0U
#define MENUMANAGER_STEPWASHMODECUSTOM_CURPOS_IDX                     1U
#define MENUMANAGER_STEPWASHMODECUSTOM_SEQIDX_IDX                     2U
#define MENUMANAGER_STEPWASHMODECUSTOM_STEPIDX_IDX                    3U

#define MenuManager_StepWashModeCustom_ListIndex                      MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODECUSTOM_LISTINDEX_IDX)
#define MenuManager_StepWashModeCustom_CurPos                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODECUSTOM_CURPOS_IDX)
#define MenuManager_StepWashModeCustom_SeqIdx                         MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODECUSTOM_SEQIDX_IDX)
#define MenuManager_StepWashModeCustom_StepIdx                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPWASHMODECUSTOM_STEPIDX_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepWashRunTime_ChildTitleStr[] =    "USER W. RUN TIME";
static const uint8_t MenuManager_StepWashStopTime_ChildTitleStr[] =   "USER W. STOP TIME";
static const uint8_t MenuManager_StepWashSpeed_ChildTitleStr[] =      "USER W. SPEED";

static const uint8_t MenuManager_StepWashModeCustom_MainTitleStr[] =  "WASH CUSTOM";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepWashModeCustom_ChildMenu[3] =
{
  { &MenuManager_StepWashRunTime_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_StepWashStopTime_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_StepWashSpeed_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_3             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepWashModeCustom_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)3U,
  .childMenuCfg               = &MenuManager_StepWashModeCustom_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepWashModeCustom_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepWashModeCustom_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepWashModeCustom_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepWashModeCustom_Entry             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashModeCustom_StartBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashModeCustom_StopBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashModeCustom_UpBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepWashModeCustom_DownBut           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepWashModeCustom_StateMachine[8] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepWashModeCustom_Entry                                            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_WASH_RUN_TIME    ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_STEP_WASH_STOP_TIME   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_STEP_WASH_SPEED       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepWashModeCustom_UpBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepWashModeCustom_DownBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepWashModeCustom_StartBut                                         ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepWashModeCustom_StopBut, MENUMANAGER_STATE_PROGRAM_MAIN_SETUP    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepWashModeCustom_LcdShowMainTitle(void);
static void MenuManager_StepWashModeCustom_LcdShowList(void);

static void MenuManager_StepWashModeCustom_SubMainFunction(void);
static void MenuManager_StepWashModeCustom_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepWashModeCustom_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepWashModeCustom_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepWashModeCustom_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepWashModeCustom_ListIndex, MenuManager_StepWashModeCustom_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepWashModeCustom_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepWashModeCustom_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepWashModeCustom_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepWashModeCustom_ListIndex, MenuManager_StepWashModeCustom_CurPos, PROGRAMMANAGER_SEQUENCE_NUM_MAX);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashModeCustom_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_RUN_TIME)   || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_STOP_TIME)  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_SPEED))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_WASH_MODE)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPWASHMODECUSTOM_INTERNALDATALENGTH);

      MenuManager_StepWashModeCustom_ListIndex      = (uint32_t)0U;
      MenuManager_StepWashModeCustom_CurPos         = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
      MenuManager_StepWashModeCustom_SeqIdx         = enterDataHierachy->seqIdx;
      MenuManager_StepWashModeCustom_StepIdx        = enterDataHierachy->stepIdx;

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
    MenuManager_StepWashModeCustom_LcdShowMainTitle();
    MenuManager_StepWashModeCustom_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepWashModeCustom_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepWashModeCustom_SubTickHandler;
    
    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashModeCustom_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM;
  dataHierachy->seqIdx = MenuManager_StepWashModeCustom_SeqIdx;
  dataHierachy->stepIdx = MenuManager_StepWashModeCustom_StepIdx;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_StepWashModeCustom_ChildMenuConf.childMenuCfg))[MenuManager_StepWashModeCustom_ListIndex].childMenuEvent));

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashModeCustom_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_WASH_MODE_CUSTOM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashModeCustom_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashModeCustom_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepWashModeCustom_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepWashModeCustom_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepWashModeCustom_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepWashModeCustom_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepWashModeCustom_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepWashModeCustom_ListIndex < ((uint32_t)(MenuManager_StepWashModeCustom_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepWashModeCustom_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepWashModeCustom_CurPos < MenuManager_Common_LcdMaxCurPos((uint32_t)(MenuManager_StepWashModeCustom_ChildMenuConf.childMenuNum)))
  {
    MenuManager_StepWashModeCustom_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepWashModeCustom_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepWashModeCustom_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepWashModeCustom_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_StepWashModeCustom_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
