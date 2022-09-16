/* 
 * File:   _1215000000_menu_TempUnit.c
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
#define MENUMANAGER_TEMPUNIT_INTERNALDATALENGTH                       (uint8_t)4U

#define MENUMANAGER_TEMPUNIT_INTERNALSTATE_IDX                        0U
#define MENUMANAGER_TEMPUNIT_COUNTER_IDX                              1U
#define MENUMANAGER_TEMPUNIT_LISTINDEX_IDX                            2U
#define MENUMANAGER_TEMPUNIT_CURPOS_IDX                               3U

#define MenuManager_TempUnit_InternalState                            MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPUNIT_INTERNALSTATE_IDX)
#define MenuManager_TempUnit_Counter                                  MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPUNIT_COUNTER_IDX)
#define MenuManager_TempUnit_ListIndex                                MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPUNIT_LISTINDEX_IDX)
#define MenuManager_TempUnit_CurPos                                   MenuManager_GetInternalDataPtr(MENUMANAGER_TEMPUNIT_CURPOS_IDX)



#define MENUMANAGER_TEMPUNIT_INTERNALSTATE_READY                      (uint32_t)0U
#define MENUMANAGER_TEMPUNIT_INTERNALSTATE_RUNNING                    (uint32_t)1U
#define MENUMANAGER_TEMPUNIT_INTERNALSTATE_DONE                       (uint32_t)2U
#define MENUMANAGER_TEMPUNIT_INTERNALSTATE_EXITING                    (uint32_t)3U

#define MENUMANAGER_TEMPUNIT_COUNTER_MAX                              (uint32_t)50U

#define MENUMANAGER_TEMPUNIT_LISTINDEX_CELSIUS                        (uint32_t)0U
#define MENUMANAGER_TEMPUNIT_LISTINDEX_FAHRENHEIT                     (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  ProgramManager_TempUnitType mapValue;
} MenuManager_TempUnit_IntValMapStruct;

static MenuManager_TempUnit_IntValMapStruct MenuManager_TempUnit_IntValMapConf[2] =
{
  { MENUMANAGER_TEMPUNIT_LISTINDEX_CELSIUS,                           PROGRAMMANAGER_TEMP_UNIT_CELSIUS        },
  { MENUMANAGER_TEMPUNIT_LISTINDEX_FAHRENHEIT,                        PROGRAMMANAGER_TEMP_UNIT_FAHRENHEIT     }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_TempUnit_MainTitleStr[] =            "TEMP UNIT";

static const uint8_t MenuManager_TempUnit_CelsiusStr[] =              "CELSIUS (C)";
static const uint8_t MenuManager_TempUnit_FahrenheitStr[] =           "FAHRENHEIT (F)";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_TempUnit_ChildMenu[2] =
{
  { &MenuManager_TempUnit_CelsiusStr,                                 MENUMANAGER_EVENT_PREV                  },
  { &MenuManager_TempUnit_FahrenheitStr,                              MENUMANAGER_EVENT_PREV                  }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_TempUnit_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_TempUnit_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_TempUnit_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_TempUnit_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_TempUnit_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_TempUnit_Entry                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempUnit_Exit                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempUnit_StartBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempUnit_StopBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempUnit_UpBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TempUnit_DownBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_TempUnit_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_TempUnit_Entry                                                      ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_TempUnit_Exit                                                       ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_PREV,                                                     MENUMANAGER_STATE_MACHINE_FUNC_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_TempUnit_StartBut                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_TempUnit_StopBut,           MENUMANAGER_STATE_MACHINE_FUNC_SETUP    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_TempUnit_UpBut                                                      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_TempUnit_DownBut                                                    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_TempUnit_LcdShowMainTitle(void);
static void MenuManager_TempUnit_LcdShowList(void);
static void MenuManager_TempUnit_LcdShowDone(void);

static void MenuManager_TempUnit_SubMainFunction(void);
static void MenuManager_TempUnit_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_TempUnit_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_TempUnit_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_TempUnit_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_TempUnit_ListIndex, MenuManager_TempUnit_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_TempUnit_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_TempUnit_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_TempUnit_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_TempUnit_ListIndex, MenuManager_TempUnit_CurPos, MenuManager_TempUnit_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_TempUnit_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_TempUnit_ChildMenuConf.childMenuCfg))[MenuManager_TempUnit_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_TempUnit_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_TempUnit_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempUnit_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MACHINE_FUNC_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_TEMPUNIT_INTERNALDATALENGTH);

      MenuManager_TempUnit_InternalState = MENUMANAGER_TEMPUNIT_INTERNALSTATE_READY;
      MenuManager_TempUnit_Counter = (uint32_t)0U;
      
      for ( MenuManager_TempUnit_ListIndex = (uint32_t)0U; \
            MenuManager_TempUnit_ListIndex < MenuManager_TempUnit_ChildMenuConf.childMenuNum; \
            MenuManager_TempUnit_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_TempUnit_IntValMapConf[MenuManager_TempUnit_ListIndex].mapValue == ProgramManager_gParamConfig.machineFuncCfg.tempUnit)
        {
          break;
        }
      }
      
      MenuManager_TempUnit_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_TempUnit_ListIndex, MenuManager_TempUnit_ChildMenuConf.childMenuNum);
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
    MenuManager_TempUnit_LcdShowMainTitle();
    MenuManager_TempUnit_LcdShowList();

    MenuManager_SubMainFunction = MenuManager_TempUnit_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_TempUnit_SubTickHandler;
    
    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempUnit_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_TEMP_UNIT;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempUnit_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempUnit_InternalState == MENUMANAGER_TEMPUNIT_INTERNALSTATE_READY)
  {
    MenuManager_TempUnit_InternalState = MENUMANAGER_TEMPUNIT_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempUnit_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempUnit_InternalState != MENUMANAGER_TEMPUNIT_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempUnit_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempUnit_ListIndex > (uint32_t)0U)
  {
    MenuManager_TempUnit_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_TempUnit_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_TempUnit_CurPos -= (uint32_t)1U;
  }

  MenuManager_TempUnit_LcdShowList();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TempUnit_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TempUnit_ListIndex < ((uint32_t)(MenuManager_TempUnit_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_TempUnit_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_TempUnit_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_TempUnit_ChildMenuConf.childMenuNum))
  {
    MenuManager_TempUnit_CurPos += (uint32_t)1U;
  }
  
  MenuManager_TempUnit_LcdShowList();
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_TempUnit_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_TempUnit_ButEventMapConf);

  switch (MenuManager_TempUnit_InternalState)
  {
    case MENUMANAGER_TEMPUNIT_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_TEMPUNIT_INTERNALSTATE_RUNNING:
    {
      ProgramManager_MachineFuncSetup_TempUnit_SetData(&(MenuManager_TempUnit_IntValMapConf[MenuManager_TempUnit_ListIndex].mapValue));

      /* Read program parameters and current sequence */
      ProgramManager_ParamConfigSetup_GetData(&ProgramManager_gParamConfig);
      ProgramManager_AutoSeqConfig_GetData(&ProgramManager_gAutoSeqConfig);
      ProgramManager_ManualSeqConfig_GetData(&ProgramManager_gManualSeqConfig);

      MenuManager_TempUnit_InternalState = MENUMANAGER_TEMPUNIT_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_TEMPUNIT_INTERNALSTATE_DONE:
    {
      MenuManager_TempUnit_LcdShowDone();

      MenuManager_TempUnit_InternalState = MENUMANAGER_TEMPUNIT_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_TEMPUNIT_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_TempUnit_SubTickHandler(void)
{
  if (MenuManager_TempUnit_InternalState == MENUMANAGER_TEMPUNIT_INTERNALSTATE_EXITING)
  {
    MenuManager_TempUnit_Counter += (uint32_t)1U;

    if (MenuManager_TempUnit_Counter >= MENUMANAGER_TEMPUNIT_COUNTER_MAX)
    {
      MenuManager_TempUnit_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent( &MenuManager_FsmContext, \
                        (Fsm_EventType)((*(MenuManager_TempUnit_ChildMenuConf.childMenuCfg))[MenuManager_TempUnit_ListIndex].childMenuEvent));
    }
  }
}



#ifdef  __cplusplus
}
#endif
