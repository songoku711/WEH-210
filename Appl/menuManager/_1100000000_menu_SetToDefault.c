/* 
 * File:   _1100000000_menu_SetToDefault.c
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
#define MENUMANAGER_SETTODEFAULT_INTERNALDATALENGTH                   (uint8_t)2U

#define MENUMANAGER_SETTODEFAULT_LISTINDEX_IDX                        0U
#define MENUMANAGER_SETTODEFAULT_CURPOS_IDX                           1U

#define MenuManager_SetToDefault_ListIndex                            MenuManager_GetInternalDataPtr(MENUMANAGER_SETTODEFAULT_LISTINDEX_IDX)
#define MenuManager_SetToDefault_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_SETTODEFAULT_CURPOS_IDX)



/** Menu manager main titles and child menu titles */
const uint8_t MenuManager_SetDefaultParam_ChildTitleStr[] =           "PARAMS TO DEFAULT";
const uint8_t MenuManager_SetDefaultProg_ChildTitleStr[] =            "PROGS TO DEFAULT";

const uint8_t MenuManager_SetToDefault_MainTitleStr[] =               "SET DEFAULT";

/** Menu manager child menu array */
static MenuManager_ChildMenuStruct MenuManager_SetToDefault_ChildMenu[2] =
{
  { &MenuManager_SetDefaultParam_ChildTitleStr,                       MENUMANAGER_EVENT_SUBMENU_1             },
  { &MenuManager_SetDefaultProg_ChildTitleStr,                        MENUMANAGER_EVENT_SUBMENU_2             }
};

/** Menu manager child menu configuration */
static MenuManager_ChildMenuConfStruct MenuManager_SetToDefault_ChildMenuConf =
{
  .childMenuNum               = (uint8_t)2U,
  .childMenuCfg               = &MenuManager_SetToDefault_ChildMenu
};



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_SetToDefault_ButEventMap[4] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_SetToDefault_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)4U,
  .butEventMapCfg             = &MenuManager_SetToDefault_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_SetToDefault_Entry                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetToDefault_StartBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetToDefault_StopBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetToDefault_UpBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_SetToDefault_DownBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_SetToDefault_StateMachine[7] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_SetToDefault_Entry                                                  ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_1,                                                MENUMANAGER_STATE_SET_DEFAULT_PARAM     ),
  FSM_TRIGGER_TRANSITION        ( MENUMANAGER_EVENT_SUBMENU_2,                                                MENUMANAGER_STATE_SET_DEFAULT_PROG      ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_SetToDefault_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_SetToDefault_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_SetToDefault_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_SetToDefault_StopBut,       MENUMANAGER_STATE_MAIN_SETTING          )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

void MenuManager_SetToDefault_LcdShowMainTitle(void);
void MenuManager_SetToDefault_LcdShowList(void);

static void MenuManager_SetToDefault_SubMainFunction(void);
static void MenuManager_SetToDefault_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

void MenuManager_SetToDefault_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearElementMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_SetToDefault_MainTitleStr);
  MenuManager_Common_LcdUpdateElementMenuStatic();
}

/*=============================================================================================*/
void MenuManager_SetToDefault_LcdShowList(void)
{
  uint32_t currentListIndex;
  uint32_t currentCursorPos;
  uint32_t listLength;
  
  currentListIndex = MenuManager_Common_LcdGetStartListIndex(MenuManager_SetToDefault_ListIndex, MenuManager_SetToDefault_CurPos);
  currentCursorPos = (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN;

  listLength = MenuManager_Common_LcdGetListLength((uint32_t)(MenuManager_SetToDefault_ChildMenuConf.childMenuNum));
  
  MenuManager_Common_LcdClearElementMenuDynamic();
  
  for (; currentCursorPos < (listLength + (uint32_t)MENUMANAGER_COMMON_LCD_CURSOR_MIN); currentCursorPos++, currentListIndex++)
  {
    MenuManager_Common_LcdShowListElementName
    (
      (uint8_t *)*((*(MenuManager_SetToDefault_ChildMenuConf.childMenuCfg))[currentListIndex].childMenuTitleStr),
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH,
      MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS,
      currentCursorPos
    );
  }
  
  MenuManager_Common_LcdShowChosenMark(MenuManager_SetToDefault_CurPos);
  
  MenuManager_Common_LcdShowScrollBar(MenuManager_SetToDefault_ListIndex, MenuManager_SetToDefault_CurPos, MenuManager_SetToDefault_ChildMenuConf.childMenuNum);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_SetToDefault_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_SetToDefault_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_SetToDefault_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if ((pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_SET_DEFAULT_PARAM) || \
        (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_SET_DEFAULT_PROG))
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAIN_SETTING)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_SETTODEFAULT_INTERNALDATALENGTH);

      MenuManager_SetToDefault_ListIndex = (uint32_t)0U;
      MenuManager_SetToDefault_CurPos = MENUMANAGER_COMMON_LCD_CURSOR_MIN;
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_SetToDefault_LcdShowMainTitle();
    MenuManager_SetToDefault_LcdShowList();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetToDefault_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_SET_TO_DEFAULT;

  pFsmContext->dataHierachy = dataHierachy;

  Fsm_TriggerEvent( &MenuManager_FsmContext, \
                    (tFsmEvent)((*(MenuManager_SetToDefault_ChildMenuConf.childMenuCfg))[MenuManager_SetToDefault_ListIndex].childMenuEvent));
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetToDefault_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_SET_TO_DEFAULT;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetToDefault_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_SetToDefault_ListIndex > (uint32_t)0U)
  {
    MenuManager_SetToDefault_ListIndex -= (uint32_t)1U;
  }

  if (MenuManager_SetToDefault_CurPos > MENUMANAGER_COMMON_LCD_CURSOR_MIN)
  {
    MenuManager_SetToDefault_CurPos -= (uint32_t)1U;
  }

  MenuManager_SetToDefault_LcdShowList();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_SetToDefault_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_SetToDefault_ListIndex < ((uint32_t)(MenuManager_SetToDefault_ChildMenuConf.childMenuNum) - (uint32_t)1U))
  {
    MenuManager_SetToDefault_ListIndex += (uint32_t)1U;
  }
  
  if (MenuManager_SetToDefault_CurPos < MenuManager_Common_LcdMaxCurPos(MenuManager_SetToDefault_ChildMenuConf.childMenuNum))
  {
    MenuManager_SetToDefault_CurPos += (uint32_t)1U;
  }
  
  MenuManager_SetToDefault_LcdShowList();
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_SetToDefault_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_SetToDefault_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_SetToDefault_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
