/* 
 * File:   _1311810000_menu_StepDrainModeCustom.c
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
#define MENUMANAGER_STEPDRAINMODECUSTOM_INTERNALDATALENGTH            (uint8_t)4U

#define MENUMANAGER_STEPDRAINMODECUSTOM_LISTINDEX_IDX                 0U
#define MENUMANAGER_STEPDRAINMODECUSTOM_CURPOS_IDX                    1U
#define MENUMANAGER_STEPDRAINMODECUSTOM_SEQIDX_IDX                    2U
#define MENUMANAGER_STEPDRAINMODECUSTOM_STEPIDX_IDX                   3U

#define MenuManager_StepDrainModeCustom_ListIndex                     MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODECUSTOM_LISTINDEX_IDX)
#define MenuManager_StepDrainModeCustom_CurPos                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODECUSTOM_CURPOS_IDX)
#define MenuManager_StepDrainModeCustom_SeqIdx                        MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODECUSTOM_SEQIDX_IDX)
#define MenuManager_StepDrainModeCustom_StepIdx                       MenuManager_GetInternalDataPtr(MENUMANAGER_STEPDRAINMODECUSTOM_STEPIDX_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_StepForwardDrainParam_ChildTitleStr[] =  "Forward Drain";
static const uint8_t MenuManager_StepBalanceDrainParam_ChildTitleStr[] =  "Balance Drain";
static const uint8_t MenuManager_StepExtrLvl1DrainParam_ChildTitleStr[] = "Extr Lv.1 Drain";
static const uint8_t MenuManager_StepExtrLvl2DrainParam_ChildTitleStr[] = "Extr Lv.2 Drain";
static const uint8_t MenuManager_StepExtrLvl3DrainParam_ChildTitleStr[] = "Extr Lv.3 Drain";
static const uint8_t MenuManager_StepExtrLvl4DrainParam_ChildTitleStr[] = "Extr Lv.4 Drain";
static const uint8_t MenuManager_StepDrainOffTime_ChildTitleStr[] =       "Drain Off";

static const uint8_t MenuManager_StepDrainModeCustom_MainTitleStr[] = "DRAIN STEP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_StepDrainModeCustom_ChildMenu[7] =
{
  { &MenuManager_StepForwardDrainParam_ChildTitleStr,                 MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_StepBalanceDrainParam_ChildTitleStr,                 MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_StepExtrLvl1DrainParam_ChildTitleStr,                MENUMANAGER_EVENT_SUBMENU_3             },
  { &MenuManager_StepExtrLvl2DrainParam_ChildTitleStr,                MENUMANAGER_EVENT_SUBMENU_4             },
  { &MenuManager_StepExtrLvl3DrainParam_ChildTitleStr,                MENUMANAGER_EVENT_SUBMENU_5             },
  { &MenuManager_StepExtrLvl4DrainParam_ChildTitleStr,                MENUMANAGER_EVENT_SUBMENU_6             },
  { &MenuManager_StepDrainOffTime_ChildTitleStr,                      MENUMANAGER_EVENT_SUBMENU_7             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_StepDrainModeCustom_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)7U,
  .childMenuCfg               = &MenuManager_StepDrainModeCustom_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_StepDrainModeCustom_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_StepDrainModeCustom_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_StepDrainModeCustom_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_StepDrainModeCustom_Entry            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeCustom_StartBut         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeCustom_StopBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeCustom_UpBut            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_StepDrainModeCustom_DownBut          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_StepDrainModeCustom_StateMachine[12] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_StepDrainModeCustom_Entry                                            ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_4,                                                MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_5,                                                MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_6,                                                MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME   ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_7,                                                MENUMANAGER_STATE_STEP_DRAIN_OFF_TIME    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_StepDrainModeCustom_UpBut                                            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_StepDrainModeCustom_DownBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_StepDrainModeCustom_StartBut                                         ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_StepDrainModeCustom_StopBut, MENUMANAGER_STATE_STEP_NORM_SETUP       )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_StepDrainModeCustom_LcdShowMainTitle(void);
static void MenuManager_StepDrainModeCustom_LcdShowList(void);

static void MenuManager_StepDrainModeCustom_SubMainFunction(void);
static void MenuManager_StepDrainModeCustom_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_StepDrainModeCustom_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_StepDrainModeCustom_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeCustom_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_StepDrainModeCustom_ListIndex, MenuManager_StepDrainModeCustom_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_StepDrainModeCustom_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_StepDrainModeCustom_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_StepDrainModeCustom_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_StepDrainModeCustom_ListIndex, MenuManager_StepDrainModeCustom_CurPos, PROGRAMMANAGER_SEQUENCE_NUM_MAX);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeCustom_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_DRAIN_MODE_TIME) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_DRAIN_OFF_TIME))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_STEP_DRAIN_MODE)
    {
      enterDataHierachy = (MenuManager_Common_ProgramSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_STEPDRAINMODECUSTOM_INTERNALDATALENGTH);

      MenuManager_StepDrainModeCustom_ListIndex      = (uint32_t)0U;
      MenuManager_StepDrainModeCustom_CurPos         = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
      MenuManager_StepDrainModeCustom_SeqIdx         = enterDataHierachy->seqIdx;
      MenuManager_StepDrainModeCustom_StepIdx        = enterDataHierachy->stepIdx;

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
    MenuManager_StepDrainModeCustom_LcdShowMainTitle();
    MenuManager_StepDrainModeCustom_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_StepDrainModeCustom_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_StepDrainModeCustom_SubTickHandler;
    
    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeCustom_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_ProgramSetupStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (MenuManager_Common_ProgramSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_ProgramSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_DRAIN_MODE_CUSTOM;
  dataHierachy->seqIdx = MenuManager_StepDrainModeCustom_SeqIdx;
  dataHierachy->stepIdx = MenuManager_StepDrainModeCustom_StepIdx;
  dataHierachy->drainIdx = MenuManager_StepDrainModeCustom_ListIndex;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_StepDrainModeCustom_ChildMenuConf.childMenuCfg))[MenuManager_StepDrainModeCustom_ListIndex].childMenuEvent));

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeCustom_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_STEP_DRAIN_MODE_CUSTOM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeCustom_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeCustom_ListIndex > (uint32_t)0U)
  {
    MenuManager_StepDrainModeCustom_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_StepDrainModeCustom_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_StepDrainModeCustom_CurPos -= (uint32_t)1U;
  }

  MenuManager_StepDrainModeCustom_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_StepDrainModeCustom_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_StepDrainModeCustom_ListIndex < ((uint32_t)(MenuManager_StepDrainModeCustom_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_StepDrainModeCustom_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_StepDrainModeCustom_CurPos < MenuManager_Common_LcdMaxCurPos((uint32_t)(MenuManager_StepDrainModeCustom_ChildMenuConf.childMenuNum)))
  {
    MenuManager_StepDrainModeCustom_CurPos += (uint32_t)1U;
  }
  
  MenuManager_StepDrainModeCustom_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_StepDrainModeCustom_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_StepDrainModeCustom_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_StepDrainModeCustom_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
