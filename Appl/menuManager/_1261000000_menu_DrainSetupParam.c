/* 
 * File:   _1271000000_menu_DrainSetupParam.c
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
#define MENUMANAGER_DRAINSETUPPARAM_INTERNALDATALENGTH                (uint8_t)3U

#define MENUMANAGER_DRAINSETUPPARAM_DRAINSTEPIDX_IDX                  0U
#define MENUMANAGER_DRAINSETUPPARAM_LISTINDEX_IDX                     1U
#define MENUMANAGER_DRAINSETUPPARAM_CURPOS_IDX                        2U

#define MenuManager_DrainSetupParam_DrainStepIdx                      MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPPARAM_DRAINSTEPIDX_IDX)
#define MenuManager_DrainSetupParam_ListIndex                         MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPPARAM_LISTINDEX_IDX)
#define MenuManager_DrainSetupParam_CurPos                            MenuManager_GetInternalDataPtr(MENUMANAGER_DRAINSETUPPARAM_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_DrainSetupTime_ChildTitleStr[] =     "DRAIN TIME";
static const uint8_t MenuManager_DrainSetupSpeed_ChildTitleStr[] =    "DRAIN SPEED";

static const uint8_t MenuManager_DrainSetupParam_MainTitleStr[] =     "DRAIN PARAM";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_DrainSetupParam_ChildMenu[2] =
{
  { &MenuManager_DrainSetupTime_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_DrainSetupSpeed_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_2             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_DrainSetupParam_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_DrainSetupParam_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_DrainSetupParam_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_DrainSetupParam_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_DrainSetupParam_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_DrainSetupParam_Entry                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupParam_StartBut             (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupParam_StopBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupParam_UpBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_DrainSetupParam_DownBut              (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_DrainSetupParam_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_DrainSetupParam_Entry                                               ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_DRAIN_SETUP_TIME      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_DRAIN_SETUP_SPEED     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_DrainSetupParam_UpBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_DrainSetupParam_DownBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_DrainSetupParam_StartBut                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_DrainSetupParam_StopBut,    MENUMANAGER_STATE_DRAIN_SETUP           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_DrainSetupParam_LcdShowMainTitle(void);
static void MenuManager_DrainSetupParam_LcdShowList(void);

static void MenuManager_DrainSetupParam_SubMainFunction(void);
static void MenuManager_DrainSetupParam_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_DrainSetupParam_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_DrainSetupParam_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_DrainSetupParam_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_DrainSetupParam_ListIndex, MenuManager_DrainSetupParam_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_DrainSetupParam_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_DrainSetupParam_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_DrainSetupParam_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_DrainSetupParam_ListIndex, MenuManager_DrainSetupParam_CurPos, MenuManager_DrainSetupParam_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupParam_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_DrainSetupStruct* enterDataHierachy;
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP_TIME          )  || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP_SPEED         ))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_DRAIN_SETUP)
    {
      enterDataHierachy = (MenuManager_Common_DrainSetupStruct *)(pFsmContext->dataHierachy);

      MenuManager_InternalDataPush(MENUMANAGER_DRAINSETUPPARAM_INTERNALDATALENGTH);

      MenuManager_DrainSetupParam_DrainStepIdx = enterDataHierachy->drainStepIdx;
      MenuManager_DrainSetupParam_ListIndex = (uint32_t)0U;
      MenuManager_DrainSetupParam_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;

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
    MenuManager_DrainSetupParam_LcdShowMainTitle();
    MenuManager_DrainSetupParam_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_DrainSetupParam_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_DrainSetupParam_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupParam_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_Common_DrainSetupStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (MenuManager_Common_DrainSetupStruct *)MenuManager_malloc(sizeof(MenuManager_Common_DrainSetupStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DRAIN_SETUP_PARAM;
  dataHierachy->drainStepIdx = MenuManager_DrainSetupParam_ListIndex;

  pFsmContext->dataHierachy = (Fsm_DataHierachyStruct *)dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_DrainSetupParam_ChildMenuConf.childMenuCfg))[MenuManager_DrainSetupParam_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupParam_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_DRAIN_SETUP_PARAM;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupParam_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupParam_ListIndex > (uint32_t)0U)
  {
    MenuManager_DrainSetupParam_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_DrainSetupParam_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_DrainSetupParam_CurPos -= (uint32_t)1U;
  }

  MenuManager_DrainSetupParam_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_DrainSetupParam_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_DrainSetupParam_ListIndex < ((uint32_t)(MenuManager_DrainSetupParam_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_DrainSetupParam_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_DrainSetupParam_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_DrainSetupParam_ChildMenuConf.childMenuNum))
  {
    MenuManager_DrainSetupParam_CurPos += (uint32_t)1U;
  }
  
  MenuManager_DrainSetupParam_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_DrainSetupParam_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_DrainSetupParam_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_DrainSetupParam_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
