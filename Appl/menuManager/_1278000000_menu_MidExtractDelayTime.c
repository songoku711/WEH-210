/* 
 * File:   _1278000000_menu_MidExtractDelayTime.c
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
#define MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALDATALENGTH            (uint8_t)10U

#define MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_IDX             0U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_COUNTER_IDX                   1U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_IDX                    2U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_VALUE_MIN_IDX                 3U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_VALUE_MAX_IDX                 4U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_VALUE_IDX                     5U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_IDX                   6U

#define MenuManager_MidExtractDelayTime_InternalState                 MenuManager_GetInternalDataPtr(MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_IDX)
#define MenuManager_MidExtractDelayTime_Counter                       MenuManager_GetInternalDataPtr(MENUMANAGER_MIDEXTRACTDELAYTIME_COUNTER_IDX)
#define MenuManager_MidExtractDelayTime_CurPos                        MenuManager_GetInternalDataPtr(MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_IDX)
#define MenuManager_MidExtractDelayTime_ValueMin                      MenuManager_GetInternalDataPtr(MENUMANAGER_MIDEXTRACTDELAYTIME_VALUE_MIN_IDX)
#define MenuManager_MidExtractDelayTime_ValueMax                      MenuManager_GetInternalDataPtr(MENUMANAGER_MIDEXTRACTDELAYTIME_VALUE_MAX_IDX)
#define MenuManager_MidExtractDelayTime_Value                         MenuManager_GetInternalDataPtr(MENUMANAGER_MIDEXTRACTDELAYTIME_VALUE_IDX)
#define MenuManager_MidExtractDelayTime_UnitVal(idx)                  MenuManager_GetInternalDataPtr(MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_IDX + idx)



#define MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY           (uint32_t)0U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_RUNNING         (uint32_t)1U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_DONE            (uint32_t)2U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_EXITING         (uint32_t)3U

#define MENUMANAGER_MIDEXTRACTDELAYTIME_COUNTER_MAX                   (uint32_t)50U

#define MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_XPOS                   (uint32_t)5U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_YPOS                   (uint32_t)5U

#define MENUMANAGER_MIDEXTRACTDELAYTIME_UNIT_XPOS                     (uint32_t)9U
#define MENUMANAGER_MIDEXTRACTDELAYTIME_UNIT_YPOS                     (uint32_t)5U

#define MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH                (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MidExtractDelayTime_MainTitleStr[] = "MID EXT DELAY";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MidExtractDelayTime_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MidExtractDelayTime_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_MidExtractDelayTime_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_MidExtractDelayTime_Entry                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_Exit                 (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_Submenu1             (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_StartBut             (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_StopBut              (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_UpBut                (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_DownBut              (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_AddBut               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MidExtractDelayTime_SubBut               (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_MidExtractDelayTime_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MidExtractDelayTime_Entry                                           ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MidExtractDelayTime_Exit                                            ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_MidExtractDelayTime_Submenu1, MENUMANAGER_STATE_EXTRACT_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MidExtractDelayTime_StartBut                                        ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MidExtractDelayTime_StopBut,  MENUMANAGER_STATE_EXTRACT_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MidExtractDelayTime_UpBut                                           ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MidExtractDelayTime_DownBut                                         ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_MidExtractDelayTime_AddBut                                          ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_MidExtractDelayTime_SubBut                                          )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MidExtractDelayTime_LcdShowMainTitle(void);
static void MenuManager_MidExtractDelayTime_LcdShowAdjust(void);
static void MenuManager_MidExtractDelayTime_LcdShowDone(void);

static void MenuManager_MidExtractDelayTime_SubMainFunction(void);
static void MenuManager_MidExtractDelayTime_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MidExtractDelayTime_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MidExtractDelayTime_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MidExtractDelayTime_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_XPOS,
    MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_YPOS,
    &(MenuManager_MidExtractDelayTime_UnitVal(0)),
    MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_XPOS,
    MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_YPOS,
    MenuManager_MidExtractDelayTime_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_MIDEXTRACTDELAYTIME_UNIT_XPOS, MENUMANAGER_MIDEXTRACTDELAYTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MidExtractDelayTime_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_XPOS,
    MENUMANAGER_MIDEXTRACTDELAYTIME_CURPOS_YPOS,
    &(MenuManager_MidExtractDelayTime_UnitVal(0)),
    MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_MIDEXTRACTDELAYTIME_UNIT_XPOS, MENUMANAGER_MIDEXTRACTDELAYTIME_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_MidExtractDelayTime_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_MidExtractDelayTime_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_EXTRACT_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALDATALENGTH);

      MenuManager_MidExtractDelayTime_InternalState = MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY;
      MenuManager_MidExtractDelayTime_Counter = (uint32_t)0U;
      MenuManager_MidExtractDelayTime_CurPos = (uint32_t)0U;

      MenuManager_MidExtractDelayTime_ValueMin = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_MIN;
      MenuManager_MidExtractDelayTime_ValueMax = (uint32_t)PROGRAMMANAGER_EXTRACTSETUP_MIDEXTRACTDELAYTIME_MAX;
      MenuManager_MidExtractDelayTime_Value = (uint32_t)(ProgramManager_gParamConfig.extractCfg.midExtractDelayTime);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_MidExtractDelayTime_Value),
        &(MenuManager_MidExtractDelayTime_UnitVal(0)),
        (uint8_t)MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_MidExtractDelayTime_LcdShowMainTitle();
    MenuManager_MidExtractDelayTime_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_MID_EXTRACT_DELAY_TIME;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MidExtractDelayTime_InternalState == MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    MenuManager_MidExtractDelayTime_InternalState = MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MidExtractDelayTime_InternalState != MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MidExtractDelayTime_InternalState == MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos);

    MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) += (uint32_t)1U;

    if (MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MidExtractDelayTime_UnitVal(0)), (uint8_t)MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_MidExtractDelayTime_ValueMin) || (tempVal > MenuManager_MidExtractDelayTime_ValueMax))
    {
      MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) = tempUnitVal;
    }

    MenuManager_MidExtractDelayTime_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MidExtractDelayTime_InternalState == MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos);

    MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) -= (uint32_t)1U;

    if (MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) > (uint32_t)9U)
    {
      MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MidExtractDelayTime_UnitVal(0)), (uint8_t)MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_MidExtractDelayTime_ValueMin) || (tempVal > MenuManager_MidExtractDelayTime_ValueMax))
    {
      MenuManager_MidExtractDelayTime_UnitVal(MenuManager_MidExtractDelayTime_CurPos) = tempUnitVal;
    }

    MenuManager_MidExtractDelayTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MidExtractDelayTime_InternalState == MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_MidExtractDelayTime_CurPos > (uint32_t)0U)
    {
      MenuManager_MidExtractDelayTime_CurPos -= (uint32_t)1U;
    }

    MenuManager_MidExtractDelayTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MidExtractDelayTime_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MidExtractDelayTime_InternalState == MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY)
  {
    if (MenuManager_MidExtractDelayTime_CurPos < (MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_MidExtractDelayTime_CurPos += (uint32_t)1U;
    }
    
    MenuManager_MidExtractDelayTime_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_MidExtractDelayTime_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MidExtractDelayTime_ButEventMapConf);

  switch (MenuManager_MidExtractDelayTime_InternalState)
  {
    case MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_RUNNING:
    {
      uint16_t tempMidExtractDelayTime;

      MenuManager_Common_BcdToDecConv(&(MenuManager_MidExtractDelayTime_Value), &(MenuManager_MidExtractDelayTime_UnitVal(0)), (uint8_t)MENUMANAGER_MIDEXTRACTDELAYTIME_UNITVAL_LENGTH);

      tempMidExtractDelayTime = (uint16_t)MenuManager_MidExtractDelayTime_Value;

      ProgramManager_ExtractSetup_MidExtractDelayTime_SetData(&tempMidExtractDelayTime);

      ProgramManager_ExtractSetup_MidExtractDelayTime_GetData(&(ProgramManager_gParamConfig.extractCfg.midExtractDelayTime));

      MenuManager_MidExtractDelayTime_InternalState = MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_DONE:
    {
      MenuManager_MidExtractDelayTime_LcdShowDone();

      MenuManager_MidExtractDelayTime_InternalState = MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MidExtractDelayTime_SubTickHandler(void)
{
  if (MenuManager_MidExtractDelayTime_InternalState == MENUMANAGER_MIDEXTRACTDELAYTIME_INTERNALSTATE_EXITING)
  {
    MenuManager_MidExtractDelayTime_Counter += (uint32_t)1U;

    if (MenuManager_MidExtractDelayTime_Counter >= MENUMANAGER_MIDEXTRACTDELAYTIME_COUNTER_MAX)
    {
      MenuManager_MidExtractDelayTime_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
