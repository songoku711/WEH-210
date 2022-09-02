/* 
 * File:   _1241000000_menu_AutoReheatWhenLow.c
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
#define MENUMANAGER_AUTOREHEATWHENLOW_INTERNALDATALENGTH              (uint8_t)4U

#define MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_IDX               0U
#define MENUMANAGER_AUTOREHEATWHENLOW_COUNTER_IDX                     1U
#define MENUMANAGER_AUTOREHEATWHENLOW_LISTINDEX_IDX                   2U
#define MENUMANAGER_AUTOREHEATWHENLOW_CURPOS_IDX                      3U

#define MenuManager_AutoReheatWhenLow_InternalState                   MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_IDX)
#define MenuManager_AutoReheatWhenLow_Counter                         MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREHEATWHENLOW_COUNTER_IDX)
#define MenuManager_AutoReheatWhenLow_ListIndex                       MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREHEATWHENLOW_LISTINDEX_IDX)
#define MenuManager_AutoReheatWhenLow_CurPos                          MenuManager_GetInternalDataPtr(MENUMANAGER_AUTOREHEATWHENLOW_CURPOS_IDX)



#define MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_READY             (uint32_t)0U
#define MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_RUNNING           (uint32_t)1U
#define MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_DONE              (uint32_t)2U
#define MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_EXITING           (uint32_t)3U

#define MENUMANAGER_AUTOREHEATWHENLOW_COUNTER_MAX                     (uint32_t)50U

#define MENUMANAGER_AUTOREHEATWHENLOW_LISTINDEX_DISABLE               (uint32_t)0U
#define MENUMANAGER_AUTOREHEATWHENLOW_LISTINDEX_ENABLE                (uint32_t)1U



/* Internal value mapping structure */
typedef struct
{
  uint32_t                    listIndex;
  bool                        mapValue;
} MenuManager_AutoReheatWhenLow_IntValMapStruct;

static MenuManager_AutoReheatWhenLow_IntValMapStruct MenuManager_AutoReheatWhenLow_IntValMapConf[2] =
{
  { MENUMANAGER_AUTOREHEATWHENLOW_LISTINDEX_DISABLE,                  false                                   },
  { MENUMANAGER_AUTOREHEATWHENLOW_LISTINDEX_ENABLE,                   true                                    }
};




/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_AutoReheatWhenLow_MainTitleStr[] =   "AUTO REHEAT";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_AutoReheatWhenLow_ChildMenu[2] =
{
  { &MenuManager_Common_DisableStr,                                   MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_Common_EnableStr,                                    MENUMANAGER_EVENT_SUBMENU_1             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_AutoReheatWhenLow_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_AutoReheatWhenLow_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_AutoReheatWhenLow_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_AutoReheatWhenLow_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_AutoReheatWhenLow_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_AutoReheatWhenLow_Entry                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_AutoReheatWhenLow_Exit                   (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_AutoReheatWhenLow_Submenu1               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_AutoReheatWhenLow_StartBut               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_AutoReheatWhenLow_StopBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_AutoReheatWhenLow_UpBut                  (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_AutoReheatWhenLow_DownBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_AutoReheatWhenLow_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_AutoReheatWhenLow_Entry                                             ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_AutoReheatWhenLow_Exit                                              ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_AutoReheatWhenLow_Submenu1, MENUMANAGER_STATE_HEAT_TEMP_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_AutoReheatWhenLow_StartBut                                          ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_AutoReheatWhenLow_StopBut,  MENUMANAGER_STATE_HEAT_TEMP_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_AutoReheatWhenLow_UpBut                                             ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_AutoReheatWhenLow_DownBut                                           )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_AutoReheatWhenLow_LcdShowMainTitle(void);
static void MenuManager_AutoReheatWhenLow_LcdShowList(void);
static void MenuManager_AutoReheatWhenLow_LcdShowDone(void);

static void MenuManager_AutoReheatWhenLow_SubMainFunction(void);
static void MenuManager_AutoReheatWhenLow_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_AutoReheatWhenLow_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_AutoReheatWhenLow_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_AutoReheatWhenLow_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_AutoReheatWhenLow_ListIndex, MenuManager_AutoReheatWhenLow_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_AutoReheatWhenLow_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_AutoReheatWhenLow_ListIndex, MenuManager_AutoReheatWhenLow_CurPos, MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_AutoReheatWhenLow_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowListElementName
  (
    (uint8_t *)*((*(MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuCfg))[MenuManager_AutoReheatWhenLow_ListIndex].childMenuTitleStr),
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
    MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
    (uint8_t)MenuManager_AutoReheatWhenLow_CurPos
  );

  MenuManager_Common_LcdShowChosenMark(MenuManager_AutoReheatWhenLow_CurPos);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_AutoReheatWhenLow_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_AutoReheatWhenLow_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_AutoReheatWhenLow_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HEAT_TEMP_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_AUTOREHEATWHENLOW_INTERNALDATALENGTH);

      MenuManager_AutoReheatWhenLow_InternalState = MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_READY;
      MenuManager_AutoReheatWhenLow_Counter = (uint32_t)0U;
      
      for ( MenuManager_AutoReheatWhenLow_ListIndex = (uint32_t)0U; \
            MenuManager_AutoReheatWhenLow_ListIndex < MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuNum; \
            MenuManager_AutoReheatWhenLow_ListIndex += (uint32_t)1U)
      {
        if (MenuManager_AutoReheatWhenLow_IntValMapConf[MenuManager_AutoReheatWhenLow_ListIndex].mapValue == ProgramManager_gParamConfig.heatTempCfg.autoReheatWhenLow)
        {
          break;
        }
      }
      
      MenuManager_AutoReheatWhenLow_CurPos = MenuManager_Common_LcdGetCurPosFirstTime(MenuManager_AutoReheatWhenLow_ListIndex, MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuNum);
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_AutoReheatWhenLow_LcdShowMainTitle();
    MenuManager_AutoReheatWhenLow_LcdShowList();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_AutoReheatWhenLow_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_AUTO_REHEAT_WHEN_LOW;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_AutoReheatWhenLow_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_AutoReheatWhenLow_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_AutoReheatWhenLow_InternalState == MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_READY)
  {
    MenuManager_AutoReheatWhenLow_InternalState = MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_AutoReheatWhenLow_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_AutoReheatWhenLow_InternalState != MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_AutoReheatWhenLow_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_AutoReheatWhenLow_ListIndex > (uint32_t)0U)
  {
    MenuManager_AutoReheatWhenLow_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_AutoReheatWhenLow_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_AutoReheatWhenLow_CurPos -= (uint32_t)1U;
  }

  MenuManager_AutoReheatWhenLow_LcdShowList();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_AutoReheatWhenLow_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_AutoReheatWhenLow_ListIndex < ((uint32_t)(MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_AutoReheatWhenLow_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_AutoReheatWhenLow_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_AutoReheatWhenLow_ChildMenuConf.childMenuNum))
  {
    MenuManager_AutoReheatWhenLow_CurPos += (uint32_t)1U;
  }
  
  MenuManager_AutoReheatWhenLow_LcdShowList();
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_AutoReheatWhenLow_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_AutoReheatWhenLow_ButEventMapConf);

  switch (MenuManager_AutoReheatWhenLow_InternalState)
  {
    case MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_RUNNING:
    {
      ProgramManager_HeatTempSetup_AutoReheatWhenLow_SetData(&(MenuManager_AutoReheatWhenLow_IntValMapConf[MenuManager_AutoReheatWhenLow_ListIndex].mapValue));

      ProgramManager_HeatTempSetup_AutoReheatWhenLow_GetData(&(ProgramManager_gParamConfig.heatTempCfg.autoReheatWhenLow));

      MenuManager_AutoReheatWhenLow_InternalState = MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_DONE:
    {
      MenuManager_AutoReheatWhenLow_LcdShowDone();

      MenuManager_AutoReheatWhenLow_InternalState = MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_AutoReheatWhenLow_SubTickHandler(void)
{
  if (MenuManager_AutoReheatWhenLow_InternalState == MENUMANAGER_AUTOREHEATWHENLOW_INTERNALSTATE_EXITING)
  {
    MenuManager_AutoReheatWhenLow_Counter += (uint32_t)1U;

    if (MenuManager_AutoReheatWhenLow_Counter >= MENUMANAGER_AUTOREHEATWHENLOW_COUNTER_MAX)
    {
      MenuManager_AutoReheatWhenLow_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
