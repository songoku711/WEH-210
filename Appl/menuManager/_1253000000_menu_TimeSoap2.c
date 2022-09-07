/* 
 * File:   _1253000000_menu_TimeSoap2.c
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
#define MENUMANAGER_TIMESOAP2_INTERNALDATALENGTH                      (uint8_t)10U

#define MENUMANAGER_TIMESOAP2_INTERNALSTATE_IDX                       0U
#define MENUMANAGER_TIMESOAP2_COUNTER_IDX                             1U
#define MENUMANAGER_TIMESOAP2_CURPOS_IDX                              2U
#define MENUMANAGER_TIMESOAP2_VALUE_MIN_IDX                           3U
#define MENUMANAGER_TIMESOAP2_VALUE_MAX_IDX                           4U
#define MENUMANAGER_TIMESOAP2_VALUE_IDX                               5U
#define MENUMANAGER_TIMESOAP2_UNITVAL_IDX                             6U

#define MenuManager_TimeSoap2_InternalState                           MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP2_INTERNALSTATE_IDX)
#define MenuManager_TimeSoap2_Counter                                 MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP2_COUNTER_IDX)
#define MenuManager_TimeSoap2_CurPos                                  MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP2_CURPOS_IDX)
#define MenuManager_TimeSoap2_ValueMin                                MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP2_VALUE_MIN_IDX)
#define MenuManager_TimeSoap2_ValueMax                                MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP2_VALUE_MAX_IDX)
#define MenuManager_TimeSoap2_Value                                   MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP2_VALUE_IDX)
#define MenuManager_TimeSoap2_UnitVal(idx)                            MenuManager_GetInternalDataPtr(MENUMANAGER_TIMESOAP2_UNITVAL_IDX + idx)



#define MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY                     (uint32_t)0U
#define MENUMANAGER_TIMESOAP2_INTERNALSTATE_RUNNING                   (uint32_t)1U
#define MENUMANAGER_TIMESOAP2_INTERNALSTATE_DONE                      (uint32_t)2U
#define MENUMANAGER_TIMESOAP2_INTERNALSTATE_EXITING                   (uint32_t)3U

#define MENUMANAGER_TIMESOAP2_COUNTER_MAX                             (uint32_t)50U

#define MENUMANAGER_TIMESOAP2_CURPOS_XPOS                             (uint32_t)5U
#define MENUMANAGER_TIMESOAP2_CURPOS_YPOS                             (uint32_t)5U

#define MENUMANAGER_TIMESOAP2_UNIT_XPOS                               (uint32_t)9U
#define MENUMANAGER_TIMESOAP2_UNIT_YPOS                               (uint32_t)5U

#define MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH                          (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_TimeSoap2_MainTitleStr[] =           "TIME SOAP 2";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_TimeSoap2_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_TimeSoap2_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_TimeSoap2_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_TimeSoap2_Entry                          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_Exit                           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_Submenu1                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_StartBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_StopBut                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_UpBut                          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_DownBut                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_AddBut                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_TimeSoap2_SubBut                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_TimeSoap2_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_TimeSoap2_Entry                                                     ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_TimeSoap2_Exit                                                      ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_TimeSoap2_Submenu1,         MENUMANAGER_STATE_SOAP_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_TimeSoap2_StartBut                                                  ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_TimeSoap2_StopBut,          MENUMANAGER_STATE_SOAP_SETUP            ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_TimeSoap2_UpBut                                                     ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_TimeSoap2_DownBut                                                   ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_TimeSoap2_AddBut                                                    ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_TimeSoap2_SubBut                                                    )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_TimeSoap2_LcdShowMainTitle(void);
static void MenuManager_TimeSoap2_LcdShowAdjust(void);
static void MenuManager_TimeSoap2_LcdShowDone(void);

static void MenuManager_TimeSoap2_SubMainFunction(void);
static void MenuManager_TimeSoap2_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_TimeSoap2_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_TimeSoap2_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_TimeSoap2_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_TIMESOAP2_CURPOS_XPOS,
    MENUMANAGER_TIMESOAP2_CURPOS_YPOS,
    &(MenuManager_TimeSoap2_UnitVal(0)),
    MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_TIMESOAP2_CURPOS_XPOS,
    MENUMANAGER_TIMESOAP2_CURPOS_YPOS,
    MenuManager_TimeSoap2_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_TIMESOAP2_UNIT_XPOS, MENUMANAGER_TIMESOAP2_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_TimeSoap2_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_TIMESOAP2_CURPOS_XPOS,
    MENUMANAGER_TIMESOAP2_CURPOS_YPOS,
    &(MenuManager_TimeSoap2_UnitVal(0)),
    MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_TIMESOAP2_UNIT_XPOS, MENUMANAGER_TIMESOAP2_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString((uint8_t *)MenuManager_Common_SecondStr);

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = MenuManager_TimeSoap2_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_TimeSoap2_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_SOAP_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_TIMESOAP2_INTERNALDATALENGTH);

      MenuManager_TimeSoap2_InternalState = MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY;
      MenuManager_TimeSoap2_Counter = (uint32_t)0U;
      MenuManager_TimeSoap2_CurPos = (uint32_t)0U;

      MenuManager_TimeSoap2_ValueMin = (uint32_t)PROGRAMMANAGER_SOAPSETUP_TIME_SOAP_MIN;
      MenuManager_TimeSoap2_ValueMax = (uint32_t)PROGRAMMANAGER_SOAPSETUP_TIME_SOAP_MAX;
      MenuManager_TimeSoap2_Value = (uint32_t)(ProgramManager_gParamConfig.soapCfg.timeSoap2);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_TimeSoap2_Value),
        &(MenuManager_TimeSoap2_UnitVal(0)),
        (uint8_t)MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH
      );
    }
    else
    {
      return FSM_GUARD_FALSE;
    }

    MenuManager_TimeSoap2_LcdShowMainTitle();
    MenuManager_TimeSoap2_LcdShowAdjust();

    return FSM_GUARD_TRUE;
  }

  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  Fsm_DataHierachyStruct* dataHierachy;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_TIME_SOAP_2;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_Submenu1(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TimeSoap2_InternalState == MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY)
  {
    MenuManager_TimeSoap2_InternalState = MENUMANAGER_TIMESOAP2_INTERNALSTATE_RUNNING;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TimeSoap2_InternalState != MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY)
  {
    return FSM_GUARD_FALSE;
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_TimeSoap2_InternalState == MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos);

    MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) += (uint32_t)1U;

    if (MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) > (uint32_t)9U)
    {
      MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_TimeSoap2_UnitVal(0)), (uint8_t)MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_TimeSoap2_ValueMin) || (tempVal > MenuManager_TimeSoap2_ValueMax))
    {
      MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) = tempUnitVal;
    }

    MenuManager_TimeSoap2_LcdShowAdjust();
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_TimeSoap2_InternalState == MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos);

    MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) -= (uint32_t)1U;

    if (MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) > (uint32_t)9U)
    {
      MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_TimeSoap2_UnitVal(0)), (uint8_t)MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH);

    if ((tempVal < MenuManager_TimeSoap2_ValueMin) || (tempVal > MenuManager_TimeSoap2_ValueMax))
    {
      MenuManager_TimeSoap2_UnitVal(MenuManager_TimeSoap2_CurPos) = tempUnitVal;
    }

    MenuManager_TimeSoap2_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TimeSoap2_InternalState == MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY)
  {
    if (MenuManager_TimeSoap2_CurPos > (uint32_t)0U)
    {
      MenuManager_TimeSoap2_CurPos -= (uint32_t)1U;
    }

    MenuManager_TimeSoap2_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_TimeSoap2_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (MenuManager_TimeSoap2_InternalState == MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY)
  {
    if (MenuManager_TimeSoap2_CurPos < (MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_TimeSoap2_CurPos += (uint32_t)1U;
    }
    
    MenuManager_TimeSoap2_LcdShowAdjust();
  }
  
  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_TimeSoap2_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_TimeSoap2_ButEventMapConf);

  switch (MenuManager_TimeSoap2_InternalState)
  {
    case MENUMANAGER_TIMESOAP2_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_TIMESOAP2_INTERNALSTATE_RUNNING:
    {
      uint16_t tempTimeSoap2;

      MenuManager_Common_BcdToDecConv(&(MenuManager_TimeSoap2_Value), &(MenuManager_TimeSoap2_UnitVal(0)), (uint8_t)MENUMANAGER_TIMESOAP2_UNITVAL_LENGTH);

      tempTimeSoap2 = (uint16_t)MenuManager_TimeSoap2_Value;

      ProgramManager_SoapSetup_TimeSoap2_SetData(&tempTimeSoap2);

      ProgramManager_SoapSetup_TimeSoap2_GetData(&(ProgramManager_gParamConfig.soapCfg.timeSoap2));

      MenuManager_TimeSoap2_InternalState = MENUMANAGER_TIMESOAP2_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_TIMESOAP2_INTERNALSTATE_DONE:
    {
      MenuManager_TimeSoap2_LcdShowDone();

      MenuManager_TimeSoap2_InternalState = MENUMANAGER_TIMESOAP2_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_TIMESOAP2_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_TimeSoap2_SubTickHandler(void)
{
  if (MenuManager_TimeSoap2_InternalState == MENUMANAGER_TIMESOAP2_INTERNALSTATE_EXITING)
  {
    MenuManager_TimeSoap2_Counter += (uint32_t)1U;

    if (MenuManager_TimeSoap2_Counter >= MENUMANAGER_TIMESOAP2_COUNTER_MAX)
    {
      MenuManager_TimeSoap2_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
