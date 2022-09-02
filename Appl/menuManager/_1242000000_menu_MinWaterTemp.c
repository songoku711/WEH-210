/* 
 * File:   _1242000000_menu_MinWaterTemp.c
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
#define MENUMANAGER_MINWATERTEMP_INTERNALDATALENGTH                   (uint8_t)10U

#define MENUMANAGER_MINWATERTEMP_INTERNALSTATE_IDX                    0U
#define MENUMANAGER_MINWATERTEMP_COUNTER_IDX                          1U
#define MENUMANAGER_MINWATERTEMP_CURPOS_IDX                           2U
#define MENUMANAGER_MINWATERTEMP_VALUE_MIN_IDX                        3U
#define MENUMANAGER_MINWATERTEMP_VALUE_MAX_IDX                        4U
#define MENUMANAGER_MINWATERTEMP_VALUE_IDX                            5U
#define MENUMANAGER_MINWATERTEMP_UNITVAL_IDX                          6U

#define MenuManager_MinWaterTemp_InternalState                        MenuManager_GetInternalDataPtr(MENUMANAGER_MINWATERTEMP_INTERNALSTATE_IDX)
#define MenuManager_MinWaterTemp_Counter                              MenuManager_GetInternalDataPtr(MENUMANAGER_MINWATERTEMP_COUNTER_IDX)
#define MenuManager_MinWaterTemp_CurPos                               MenuManager_GetInternalDataPtr(MENUMANAGER_MINWATERTEMP_CURPOS_IDX)
#define MenuManager_MinWaterTemp_ValueMin                             MenuManager_GetInternalDataPtr(MENUMANAGER_MINWATERTEMP_VALUE_MIN_IDX)
#define MenuManager_MinWaterTemp_ValueMax                             MenuManager_GetInternalDataPtr(MENUMANAGER_MINWATERTEMP_VALUE_MAX_IDX)
#define MenuManager_MinWaterTemp_Value                                MenuManager_GetInternalDataPtr(MENUMANAGER_MINWATERTEMP_VALUE_IDX)
#define MenuManager_MinWaterTemp_UnitVal(idx)                         MenuManager_GetInternalDataPtr(MENUMANAGER_MINWATERTEMP_UNITVAL_IDX + idx)



#define MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY                  (uint32_t)0U
#define MENUMANAGER_MINWATERTEMP_INTERNALSTATE_RUNNING                (uint32_t)1U
#define MENUMANAGER_MINWATERTEMP_INTERNALSTATE_DONE                   (uint32_t)2U
#define MENUMANAGER_MINWATERTEMP_INTERNALSTATE_EXITING                (uint32_t)3U

#define MENUMANAGER_MINWATERTEMP_COUNTER_MAX                          (uint32_t)50U

#define MENUMANAGER_MINWATERTEMP_CURPOS_XPOS                          (uint32_t)8U
#define MENUMANAGER_MINWATERTEMP_CURPOS_YPOS                          (uint32_t)5U

#define MENUMANAGER_MINWATERTEMP_UNIT_XPOS                            (uint32_t)12U
#define MENUMANAGER_MINWATERTEMP_UNIT_YPOS                            (uint32_t)5U

#define MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH                       (uint32_t)3U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_MinWaterTemp_MainTitleStr[] =        "MIN WATER TEMP";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_MinWaterTemp_ButEventMap[6] =
{
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_MinWaterTemp_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)6U,
  .butEventMapCfg             = &MenuManager_MinWaterTemp_ButEventMap
};



/** Menu manager event handlers */
static tFsmGuard MenuManager_MinWaterTemp_Entry                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_Exit                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_Submenu1                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_StartBut                    (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_StopBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_UpBut                       (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_DownBut                     (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_AddBut                      (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_MinWaterTemp_SubBut                      (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_MinWaterTemp_StateMachine[9] =
{
  FSM_TRIGGER_ENTRY             (                                     MenuManager_MinWaterTemp_Entry                                                  ),
  FSM_TRIGGER_EXIT              (                                     MenuManager_MinWaterTemp_Exit                                                   ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_SUBMENU_1,        MenuManager_MinWaterTemp_Submenu1,      MENUMANAGER_STATE_HEAT_TEMP_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_START_BUT,        MenuManager_MinWaterTemp_StartBut                                               ),
  FSM_TRIGGER_TRANSITION_ACTION ( MENUMANAGER_EVENT_STOP_BUT,         MenuManager_MinWaterTemp_StopBut,       MENUMANAGER_STATE_HEAT_TEMP_SETUP       ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_UP_BUT,           MenuManager_MinWaterTemp_UpBut                                                  ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_DOWN_BUT,         MenuManager_MinWaterTemp_DownBut                                                ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_ADD_BUT,          MenuManager_MinWaterTemp_AddBut                                                 ),
  FSM_TRIGGER_INTERNAL          ( MENUMANAGER_EVENT_SUB_BUT,          MenuManager_MinWaterTemp_SubBut                                                 )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_MinWaterTemp_LcdShowMainTitle(void);
static void MenuManager_MinWaterTemp_LcdShowAdjust(void);
static void MenuManager_MinWaterTemp_LcdShowDone(void);

static void MenuManager_MinWaterTemp_SubMainFunction(void);
static void MenuManager_MinWaterTemp_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_MinWaterTemp_LcdShowMainTitle(void)
{
  /* Print main title */
  MenuManager_Common_LcdClearAdjustMenuStatic();
  MenuManager_Common_LcdShowMainTitle(MenuManager_MinWaterTemp_MainTitleStr);
  MenuManager_Common_LcdShowOkExitChoice();
  MenuManager_Common_LcdUpdateAdjustMenuStatic();
}

/*=============================================================================================*/
static void MenuManager_MinWaterTemp_LcdShowAdjust(void)
{
  MenuManager_Common_LcdClearAdjustMenuDynamic();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MINWATERTEMP_CURPOS_XPOS,
    MENUMANAGER_MINWATERTEMP_CURPOS_YPOS,
    &(MenuManager_MinWaterTemp_UnitVal(0)),
    MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH
  );
  
  MenuManager_Common_LcdShowAdjustArrow
  (
    MENUMANAGER_MINWATERTEMP_CURPOS_XPOS,
    MENUMANAGER_MINWATERTEMP_CURPOS_YPOS,
    MenuManager_MinWaterTemp_CurPos
  );

  LCD_SetCursorPos(MENUMANAGER_MINWATERTEMP_UNIT_XPOS, MENUMANAGER_MINWATERTEMP_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    LCD_PutString((uint8_t *)MenuManager_Common_CelsiusDegStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Common_FahrenheitDegStr);
  }

  MenuManager_Common_LcdUpdateAdjustMenuDynamic();
}

/*=============================================================================================*/
static void MenuManager_MinWaterTemp_LcdShowDone(void)
{
  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowAdjustUnitVal
  (
    MENUMANAGER_MINWATERTEMP_CURPOS_XPOS,
    MENUMANAGER_MINWATERTEMP_CURPOS_YPOS,
    &(MenuManager_MinWaterTemp_UnitVal(0)),
    MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH
  );

  LCD_SetCursorPos(MENUMANAGER_MINWATERTEMP_UNIT_XPOS, MENUMANAGER_MINWATERTEMP_UNIT_YPOS, LCD_CURSOR_BY_FONT);
  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    LCD_PutString((uint8_t *)MenuManager_Common_CelsiusDegStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Common_FahrenheitDegStr);
  }

  MenuManager_Common_LcdUpdateAll();
}



/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_MinWaterTemp_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_MinWaterTemp_SubTickHandler;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_HEAT_TEMP_SETUP)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;

      MenuManager_InternalDataPush(MENUMANAGER_MINWATERTEMP_INTERNALDATALENGTH);

      MenuManager_MinWaterTemp_InternalState = MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY;
      MenuManager_MinWaterTemp_Counter = (uint32_t)0U;
      MenuManager_MinWaterTemp_CurPos = (uint32_t)0U;

      if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
      {
        MenuManager_MinWaterTemp_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_C_MIN;
      }
      else
      {
        MenuManager_MinWaterTemp_ValueMin = (uint32_t)PROGRAMMANAGER_HEATTEMPSETUP_WATER_TEMP_F_MIN;
      }

      MenuManager_MinWaterTemp_ValueMax = (uint32_t)(ProgramManager_gParamConfig.heatTempCfg.maxWaterTemp);
      MenuManager_MinWaterTemp_Value = (uint32_t)(ProgramManager_gParamConfig.heatTempCfg.minWaterTemp);

      MenuManager_Common_DecToBcdConv
      (
        &(MenuManager_MinWaterTemp_Value),
        &(MenuManager_MinWaterTemp_UnitVal(0)),
        (uint8_t)MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH
      );
    }
    else
    {
      return kFsmGuard_False;
    }

    MenuManager_MinWaterTemp_LcdShowMainTitle();
    MenuManager_MinWaterTemp_LcdShowAdjust();

    return kFsmGuard_True;
  }

  return kFsmGuard_False;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  tFsmDataHierachy* dataHierachy;

  dataHierachy = (tFsmDataHierachy *)MenuManager_malloc(sizeof(tFsmDataHierachy));
  dataHierachy->dataId = MENUMANAGER_STATE_MIN_WATER_TEMP;

  pFsmContext->dataHierachy = dataHierachy;

  /* Free internal data */
  MenuManager_InternalDataPop();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_Submenu1(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MinWaterTemp_InternalState == MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY)
  {
    MenuManager_MinWaterTemp_InternalState = MENUMANAGER_MINWATERTEMP_INTERNALSTATE_RUNNING;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MinWaterTemp_InternalState != MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY)
  {
    return kFsmGuard_False;
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MinWaterTemp_InternalState == MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos);

    MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) += (uint32_t)1U;

    if (MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) > (uint32_t)9U)
    {
      MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) = (uint32_t)0U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MinWaterTemp_UnitVal(0)), (uint8_t)MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MinWaterTemp_ValueMin) || (tempVal >= MenuManager_MinWaterTemp_ValueMax))
    {
      MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) = tempUnitVal;
    }

    MenuManager_MinWaterTemp_LcdShowAdjust();
  }

  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint32_t tempUnitVal;
  uint32_t tempVal;

  if (MenuManager_MinWaterTemp_InternalState == MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY)
  {
    tempUnitVal = MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos);

    MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) -= (uint32_t)1U;

    if (MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) > (uint32_t)9U)
    {
      MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) = (uint32_t)9U;
    }

    MenuManager_Common_BcdToDecConv(&tempVal, &(MenuManager_MinWaterTemp_UnitVal(0)), (uint8_t)MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH);

    if ((tempVal <= MenuManager_MinWaterTemp_ValueMin) || (tempVal >= MenuManager_MinWaterTemp_ValueMax))
    {
      MenuManager_MinWaterTemp_UnitVal(MenuManager_MinWaterTemp_CurPos) = tempUnitVal;
    }

    MenuManager_MinWaterTemp_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_AddBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MinWaterTemp_InternalState == MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY)
  {
    if (MenuManager_MinWaterTemp_CurPos > (uint32_t)0U)
    {
      MenuManager_MinWaterTemp_CurPos -= (uint32_t)1U;
    }

    MenuManager_MinWaterTemp_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_MinWaterTemp_SubBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  if (MenuManager_MinWaterTemp_InternalState == MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY)
  {
    if (MenuManager_MinWaterTemp_CurPos < (MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH - (uint32_t)1U))
    {
      MenuManager_MinWaterTemp_CurPos += (uint32_t)1U;
    }
    
    MenuManager_MinWaterTemp_LcdShowAdjust();
  }
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_MinWaterTemp_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_MinWaterTemp_ButEventMapConf);

  switch (MenuManager_MinWaterTemp_InternalState)
  {
    case MENUMANAGER_MINWATERTEMP_INTERNALSTATE_READY:
      break;
    case MENUMANAGER_MINWATERTEMP_INTERNALSTATE_RUNNING:
    {
      uint8_t tempMinWaterTemp;

      MenuManager_Common_BcdToDecConv(&(MenuManager_MinWaterTemp_Value), &(MenuManager_MinWaterTemp_UnitVal(0)), (uint8_t)MENUMANAGER_MINWATERTEMP_UNITVAL_LENGTH);

      tempMinWaterTemp = (uint8_t)MenuManager_MinWaterTemp_Value;

      ProgramManager_HeatTempSetup_MinWaterTemp_SetData(&tempMinWaterTemp, ProgramManager_gParamConfig.machineFuncCfg.tempUnit);

      ProgramManager_HeatTempSetup_MinWaterTemp_GetData(&(ProgramManager_gParamConfig.heatTempCfg.minWaterTemp), ProgramManager_gParamConfig.machineFuncCfg.tempUnit);

      MenuManager_MinWaterTemp_InternalState = MENUMANAGER_MINWATERTEMP_INTERNALSTATE_DONE;
      
      break;
    }
    case MENUMANAGER_MINWATERTEMP_INTERNALSTATE_DONE:
    {
      MenuManager_MinWaterTemp_LcdShowDone();

      MenuManager_MinWaterTemp_InternalState = MENUMANAGER_MINWATERTEMP_INTERNALSTATE_EXITING;

      break;
    }
    case MENUMANAGER_MINWATERTEMP_INTERNALSTATE_EXITING:
    default:
      break;
  }
}

/*=============================================================================================*/
static void MenuManager_MinWaterTemp_SubTickHandler(void)
{
  if (MenuManager_MinWaterTemp_InternalState == MENUMANAGER_MINWATERTEMP_INTERNALSTATE_EXITING)
  {
    MenuManager_MinWaterTemp_Counter += (uint32_t)1U;

    if (MenuManager_MinWaterTemp_Counter >= MENUMANAGER_MINWATERTEMP_COUNTER_MAX)
    {
      MenuManager_MinWaterTemp_Counter = (uint32_t)0U;
      
      Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
    }
  }
}



#ifdef  __cplusplus
}
#endif
