/* 
 * File:   _1240000000_menu_HeatTempSetup.c
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
#define MENUMANAGER_HEATTEMPSETUP_INTERNALDATALENGTH                  (uint8_t)2U

#define MENUMANAGER_HEATTEMPSETUP_LISTINDEX_IDX                       0U
#define MENUMANAGER_HEATTEMPSETUP_CURPOS_IDX                          1U

#define MenuManager_HeatTempSetup_ListIndex                           MenuManager_GetInternalDataPtr(MENUMANAGER_HEATTEMPSETUP_LISTINDEX_IDX)
#define MenuManager_HeatTempSetup_CurPos                              MenuManager_GetInternalDataPtr(MENUMANAGER_HEATTEMPSETUP_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_TempThreshold_ChildTitleStr[] =      "Temp Threshold";
static const uint8_t MenuManager_TempDiffReheat_ChildTitleStr[] =     "Temp Diff Reheat";
static const uint8_t MenuManager_MaxTimeHeat_ChildTitleStr[] =        "Max Time Heat";

static const uint8_t MenuManager_HeatTempSetup_MainTitleStr[] =       "HEAT AND TEMP";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_HeatTempSetup_ChildMenu[3] =
{
  { &MenuManager_TempThreshold_ChildTitleStr,                         MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_TempDiffReheat_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_2             },
  { &MenuManager_MaxTimeHeat_ChildTitleStr,                           MENUMANAGER_EVENT_SUBMENU_3             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_HeatTempSetup_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)3U,
  .childMenuCfg               = &MenuManager_HeatTempSetup_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_HeatTempSetup_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_HeatTempSetup_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_HeatTempSetup_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_HeatTempSetup_Entry                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HeatTempSetup_StartBut               (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HeatTempSetup_StopBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HeatTempSetup_UpBut                  (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_HeatTempSetup_DownBut                (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_HeatTempSetup_StateMachine[8] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_HeatTempSetup_Entry                                                 ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_TEMP_THRESHOLD        ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_TEMP_DIFF_REHEAT      ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_3,                                                MENUMANAGER_STATE_MAX_TIME_HEAT         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_HeatTempSetup_UpBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_HeatTempSetup_DownBut                                               ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_HeatTempSetup_StartBut                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_HeatTempSetup_StopBut,     MENUMANAGER_STATE_MACHINE_SETUP          )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_HeatTempSetup_LcdShowMainTitle(void);
static void MenuManager_HeatTempSetup_LcdShowList(void);

static void MenuManager_HeatTempSetup_SubMainFunction(void);
static void MenuManager_HeatTempSetup_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_HeatTempSetup_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_HeatTempSetup_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_HeatTempSetup_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_HeatTempSetup_ListIndex, MenuManager_HeatTempSetup_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_HeatTempSetup_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_HeatTempSetup_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_HeatTempSetup_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_HeatTempSetup_ListIndex, MenuManager_HeatTempSetup_CurPos, MenuManager_HeatTempSetup_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_HeatTempSetup_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_TEMP_THRESHOLD)       || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_TEMP_DIFF_REHEAT)     || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAX_TIME_HEAT))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MACHINE_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_HEATTEMPSETUP_INTERNALDATALENGTH);

      MenuManager_HeatTempSetup_ListIndex = (uint32_t)0U;
      MenuManager_HeatTempSetup_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
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
    MenuManager_HeatTempSetup_LcdShowMainTitle();
    MenuManager_HeatTempSetup_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_HeatTempSetup_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_HeatTempSetup_SubTickHandler;

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HeatTempSetup_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_HEAT_TEMP_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (Fsm_EventType)((*(MenuManager_HeatTempSetup_ChildMenuConf.childMenuCfg))[MenuManager_HeatTempSetup_ListIndex].childMenuEvent));
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HeatTempSetup_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_HEAT_TEMP_SETUP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HeatTempSetup_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HeatTempSetup_ListIndex > (uint32_t)0U)
  {
    MenuManager_HeatTempSetup_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_HeatTempSetup_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_HeatTempSetup_CurPos -= (uint32_t)1U;
  }

  MenuManager_HeatTempSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_HeatTempSetup_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_HeatTempSetup_ListIndex < ((uint32_t)(MenuManager_HeatTempSetup_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_HeatTempSetup_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_HeatTempSetup_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_HeatTempSetup_ChildMenuConf.childMenuNum))
  {
    MenuManager_HeatTempSetup_CurPos += (uint32_t)1U;
  }
  
  MenuManager_HeatTempSetup_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_HeatTempSetup_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_HeatTempSetup_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_HeatTempSetup_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
