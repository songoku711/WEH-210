/* 
 * File:   _0000000000_menu_Home.c
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
#define MENUMANAGER_HOME_INTERNALDATALENGTH                           (uint8_t)2U

#define MENUMANAGER_HOME_NOTIFYSTATE_IDX                              0U
#define MENUMANAGER_HOME_SUBMAINFUNC_COUNTER_IDX                      1U

#define MenuManager_Home_NotifyState                                  MenuManager_GetInternalDataPtr(MENUMANAGER_HOME_NOTIFYSTATE_IDX)
#define MenuManager_Home_SubMainFuncCounter                           MenuManager_GetInternalDataPtr(MENUMANAGER_HOME_SUBMAINFUNC_COUNTER_IDX)

#define MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_UPDATE_1                  (uint32_t)10U
#define MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_UPDATE_2                  (uint32_t)20U
#define MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_UPDATE_3                  (uint32_t)30U
#define MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_MAX                       (uint32_t)40U

#define MENUMANAGER_HOME_DRAINLEVEL_FORWARDDRAIN                      0U
#define MENUMANAGER_HOME_DRAINLEVEL_BALANCEDRAIN                      1U
#define MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL1DRAIN                     2U
#define MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL2DRAIN                     3U
#define MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL3DRAIN                     4U
#define MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL4DRAIN                     5U
#define MENUMANAGER_HOME_DRAINLEVEL_OFFDRAIN                          6U
#define MENUMANAGER_HOME_DRAINLEVEL_MAX                               7U

#define MENUMANAGER_HOME_SEQUENCE_INDEX_XPOS                          (uint32_t)0U
#define MENUMANAGER_HOME_SEQUENCE_INDEX_YPOS                          (uint32_t)0U

#define MENUMANAGER_HOME_STEP_INDEX_XPOS                              (uint32_t)14U
#define MENUMANAGER_HOME_STEP_INDEX_YPOS                              (uint32_t)0U

#define MENUMANAGER_HOME_NOTIFY_XPOS                                  (uint32_t)1U
#define MENUMANAGER_HOME_NOTIFY_YPOS                                  (uint32_t)2U

#define MENUMANAGER_HOME_STATE_XPOS                                   (uint32_t)10U
#define MENUMANAGER_HOME_STATE_YPOS                                   (uint32_t)2U

#define MENUMANAGER_HOME_INV_XPOS                                     (uint32_t)8U
#define MENUMANAGER_HOME_INV_YPOS                                     (uint32_t)2U

#define MENUMANAGER_HOME_IMB_XPOS                                     (uint32_t)12U
#define MENUMANAGER_HOME_IMB_YPOS                                     (uint32_t)2U

#define MENUMANAGER_HOME_DOP_XPOS                                     (uint32_t)16U
#define MENUMANAGER_HOME_DOP_YPOS                                     (uint32_t)2U

#define MENUMANAGER_HOME_PAUSED_XPOS                                  (uint32_t)1U
#define MENUMANAGER_HOME_PAUSED_YPOS                                  (uint32_t)4U

#define MENUMANAGER_HOME_COUNTDOWN_XPOS                               (uint32_t)16U
#define MENUMANAGER_HOME_COUNTDOWN_YPOS                               (uint32_t)5U

#define MENUMANAGER_HOME_TEMPSTR_XPOS                                 (uint32_t)2U
#define MENUMANAGER_HOME_TEMPSTR_YPOS                                 (uint32_t)6U

#define MENUMANAGER_HOME_TEMPERATURE_XPOS                             (uint32_t)0U
#define MENUMANAGER_HOME_TEMPERATURE_YPOS                             (uint32_t)7U

#define MENUMANAGER_HOME_TEMPERATURE_UNIT_XPOS                        (uint32_t)7U
#define MENUMANAGER_HOME_TEMPERATURE_UNIT_YPOS                        (uint32_t)7U

#define MENUMANAGER_HOME_PRESSTR_XPOS                                 (uint32_t)16U
#define MENUMANAGER_HOME_PRESSTR_YPOS                                 (uint32_t)6U

#define MENUMANAGER_HOME_PRESSURE_XPOS                                (uint32_t)14U
#define MENUMANAGER_HOME_PRESSURE_YPOS                                (uint32_t)7U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_Home_SequenceIdxStr[] =              "PROGRAM %02d";
static const uint8_t MenuManager_Home_StepIdxStr[] =                  "STEP %02d";

static const uint8_t MenuManager_Home_NotifyNotAvailStr[] =           "NOT AVAILABLE      ";
static const uint8_t MenuManager_Home_NotifyDoorOpenStr[] =           "DOOR OPEN          ";
static const uint8_t MenuManager_Home_NotifyReadyStr[] =              "READY              ";

static const uint8_t MenuManager_Home_StateWaterStr[] =               "SUPPLYING WATER    ";
static const uint8_t MenuManager_Home_StateHeatStr[] =                "HEATING            ";
static const uint8_t MenuManager_Home_StateWashStr[] =                "WASHING            ";
static const uint8_t MenuManager_Home_StateDrainStr[] =               "DRAIN              ";
static const uint8_t MenuManager_Home_StateErrorStr[] =               "ERROR              ";

static const uint8_t MenuManager_Home_StateDrainFwdStr[] =            "FORWARD";
static const uint8_t MenuManager_Home_StateDrainBalStr[] =            "BALANCE";
static const uint8_t MenuManager_Home_StateDrainExtrLv1Str[] =        "EXTR LVL1";
static const uint8_t MenuManager_Home_StateDrainExtrLv2Str[] =        "EXTR LVL2";
static const uint8_t MenuManager_Home_StateDrainExtrLv3Str[] =        "EXTR LVL3";
static const uint8_t MenuManager_Home_StateDrainExtrLv4Str[] =        "EXTR LVL4";

static const uint8_t MenuManager_Home_PausedStr[] =                   "PAUSED";
static const uint8_t MenuManager_Home_NotPausedStr[] =                "      ";



static const uint8_t MenuManager_Home_InputStateInvStr[] =            "INV";
static const uint8_t MenuManager_Home_InputStateImbStr[] =            "IMB";
static const uint8_t MenuManager_Home_InputStateDopStr[] =            "DOP";
static const uint8_t MenuManager_Home_InputStateNoneStr[] =           "   ";

static const uint8_t MenuManager_Home_TempStr[] =                     "TEMP";
static const uint8_t MenuManager_Home_PresStr[] =                     "LVL";

static const uint8_t MenuManager_Home_CountdownEnableStr[] =          "%02d:%02d";
static const uint8_t MenuManager_Home_CountdownDisableStr[] =         "--:--";

static const uint8_t MenuManager_Home_TemperatureEnableStr[] =        "%3d/%3d";
static const uint8_t MenuManager_Home_TemperatureDisableStr[] =       "%3d/---";
static const uint8_t MenuManager_Home_PressureEnableStr[] =           "%3d/%3d";
static const uint8_t MenuManager_Home_PressureDisableStr[] =          "%3d/---";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_Home_ButEventMap[9] =
{
  { IO_LONG_PRESS(IOMANAGER_BUTTON_STATE_START),                      MENUMANAGER_EVENT_LONG_START_BUT        },
  { IO_LONG_PRESS(IOMANAGER_BUTTON_STATE_STOP),                       MENUMANAGER_EVENT_LONG_STOP_BUT         },
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_ADD,                                       MENUMANAGER_EVENT_ADD_BUT               },
  { IOMANAGER_BUTTON_STATE_SUB,                                       MENUMANAGER_EVENT_SUB_BUT               },
  { IOMANAGER_BUTTON_STATE_UP | IOMANAGER_BUTTON_STATE_DOWN,          MENUMANAGER_EVENT_UP_DOWN_BUT           }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_Home_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)9U,
  .butEventMapCfg             = &MenuManager_Home_ButEventMap
};



/** Menu manager event handlers */
static Fsm_GuardType MenuManager_Home_Entry                           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_Exit                            (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_StartBut                        (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_StopBut                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_LongStartBut                    (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_LongStopBut                     (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_UpBut                           (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_DownBut                         (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_AddBut                          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_SubBut                          (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);
static Fsm_GuardType MenuManager_Home_UpDownBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_Home_StateMachine[12] =
{
  FSM_TRIGGER_ENTRY           (                                       MenuManager_Home_Entry                                                          ),
  FSM_TRIGGER_EXIT            (                                       MenuManager_Home_Exit                                                           ),
  FSM_TRIGGER_TRANSITION      ( MENUMANAGER_EVENT_SUBMENU_1,                                                  MENUMANAGER_STATE_MAIN_SETTING          ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_START_BUT,          MenuManager_Home_StartBut                                                       ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_STOP_BUT,           MenuManager_Home_StopBut                                                        ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_LONG_START_BUT,     MenuManager_Home_LongStartBut                                                   ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_LONG_STOP_BUT,      MenuManager_Home_LongStopBut                                                    ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_UP_BUT,             MenuManager_Home_UpBut                                                          ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_DOWN_BUT,           MenuManager_Home_DownBut                                                        ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_UP_DOWN_BUT,        MenuManager_Home_UpDownBut                                                      ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_ADD_BUT,            MenuManager_Home_AddBut                                                         ),
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_SUB_BUT,            MenuManager_Home_SubBut                                                         )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

static void MenuManager_Home_LcdShowSequenceStep(void);
static void MenuManager_Home_LcdShowNotifyState(void);
static void MenuManager_Home_LcdShowCountDown(void);
static void MenuManager_Home_LcdShowTemperature(void);
static void MenuManager_Home_LcdShowPressure(void);

static void MenuManager_Home_SubMainFunction(void);
static void MenuManager_Home_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static void MenuManager_Home_LcdShowSequenceStep(void)
{
  uint8_t tempStr[20];

  sprintf((char *)tempStr, (const char *)MenuManager_Home_SequenceIdxStr, ProgramManager_gAutoSeqConfig.sequenceIndex + 1U);

  LCD_SetCursorPos(MENUMANAGER_HOME_SEQUENCE_INDEX_XPOS, MENUMANAGER_HOME_SEQUENCE_INDEX_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString(tempStr);

  sprintf((char *)tempStr, (const char *)MenuManager_Home_StepIdxStr, ProgramManager_gAutoSeqConfig.currentStep + 1U);

  LCD_SetCursorPos(MENUMANAGER_HOME_STEP_INDEX_XPOS, MENUMANAGER_HOME_STEP_INDEX_YPOS, LCD_CURSOR_BY_FONT);
  LCD_PutString(tempStr);
}

/*=============================================================================================*/
static void MenuManager_Home_LcdShowNotifyState(void)
{
  LCD_SetCursorPos(MENUMANAGER_HOME_NOTIFY_XPOS, MENUMANAGER_HOME_NOTIFY_YPOS, LCD_CURSOR_BY_FONT);

  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    if (ProgramManager_gSensorDoorOpenErr != PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR)
    {
      LCD_PutString((uint8_t *)MenuManager_Home_NotifyDoorOpenStr);
    }
    else if ((ProgramManager_gAutoSeqConfig.currentStep != PROGRAMMANAGER_STEP_NUM_MAX) && \
             ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == false))
    {
      LCD_PutString((uint8_t *)MenuManager_Home_NotifyNotAvailStr);
    }
    else
    {
      LCD_PutString((uint8_t *)MenuManager_Home_NotifyReadyStr);
    }
  }
  else if ((ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE) && \
           (ProgramManager_GetCurrentState() != PROGRAMMANAGER_STATE_FAIL))
  {
    switch (ProgramManager_GetCurrentState())
    {
      case PROGRAMMANAGER_STATE_AUTO_RUN_WATER:
        LCD_PutString((uint8_t *)MenuManager_Home_StateWaterStr);
        break;
      case PROGRAMMANAGER_STATE_AUTO_RUN_HEAT:
        LCD_PutString((uint8_t *)MenuManager_Home_StateHeatStr);
        break;
      case PROGRAMMANAGER_STATE_AUTO_RUN_WASH:
        LCD_PutString((uint8_t *)MenuManager_Home_StateWashStr);
        break;
      case PROGRAMMANAGER_STATE_AUTO_RUN_DRAIN:
      {
        LCD_PutString((uint8_t *)MenuManager_Home_StateDrainStr);

        LCD_SetCursorPos(MENUMANAGER_HOME_STATE_XPOS, MENUMANAGER_HOME_STATE_YPOS, LCD_CURSOR_BY_FONT);

        switch (ProgramManager_gDrainLevel)
        {
          case MENUMANAGER_HOME_DRAINLEVEL_FORWARDDRAIN:
            LCD_PutString((uint8_t *)MenuManager_Home_StateDrainFwdStr);
            break;
          case MENUMANAGER_HOME_DRAINLEVEL_BALANCEDRAIN:
            LCD_PutString((uint8_t *)MenuManager_Home_StateDrainBalStr);
            break;
          case MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL1DRAIN:
            LCD_PutString((uint8_t *)MenuManager_Home_StateDrainExtrLv1Str);
            break;
          case MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL2DRAIN:
            LCD_PutString((uint8_t *)MenuManager_Home_StateDrainExtrLv2Str);
            break;
          case MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL3DRAIN:
            LCD_PutString((uint8_t *)MenuManager_Home_StateDrainExtrLv3Str);
            break;
          case MENUMANAGER_HOME_DRAINLEVEL_EXTRLVL4DRAIN:
            LCD_PutString((uint8_t *)MenuManager_Home_StateDrainExtrLv4Str);
            break;
          default:
            break;
        }

        break;
      }
      default:
        break;
    }
  }
  else if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_FAIL)
  {
    LCD_PutString((uint8_t *)MenuManager_Home_StateErrorStr);

    LCD_SetCursorPos(MENUMANAGER_HOME_INV_XPOS, MENUMANAGER_HOME_INV_YPOS, LCD_CURSOR_BY_FONT);

    if (ProgramManager_gSensorInverterErr != PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR)
    {
      LCD_PutString((uint8_t *)MenuManager_Home_InputStateInvStr);
    }
    else
    {
      LCD_PutString((uint8_t *)MenuManager_Home_InputStateNoneStr);
    }

    LCD_SetCursorPos(MENUMANAGER_HOME_IMB_XPOS, MENUMANAGER_HOME_IMB_YPOS, LCD_CURSOR_BY_FONT);

    if (ProgramManager_gSensorImbalanceErr != PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR)
    {
      LCD_PutString((uint8_t *)MenuManager_Home_InputStateImbStr);
    }
    else
    {
      LCD_PutString((uint8_t *)MenuManager_Home_InputStateNoneStr);
    }

    LCD_SetCursorPos(MENUMANAGER_HOME_DOP_XPOS, MENUMANAGER_HOME_DOP_YPOS, LCD_CURSOR_BY_FONT);

    if (ProgramManager_gSensorDoorOpenErr != PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NO_ERROR)
    {
      LCD_PutString((uint8_t *)MenuManager_Home_InputStateDopStr);
    }
    else
    {
      LCD_PutString((uint8_t *)MenuManager_Home_InputStateNoneStr);
    }
  }
  else
  {
    /* never enter here */
  }
}

/*=============================================================================================*/
static void MenuManager_Home_LcdShowCountDown(void)
{
  uint8_t tempStr[20];

  LCD_SetCursorPos(MENUMANAGER_HOME_PAUSED_XPOS, MENUMANAGER_HOME_PAUSED_YPOS, LCD_CURSOR_BY_FONT);

  if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    if (ProgramManager_Control_NotPaused())
    {
      LCD_PutString((uint8_t *)MenuManager_Home_NotPausedStr);
    }
    else
    {
      LCD_PutString((uint8_t *)MenuManager_Home_PausedStr);
    }
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Home_NotPausedStr);
  }

  LCD_SetCursorPos(MENUMANAGER_HOME_COUNTDOWN_XPOS, MENUMANAGER_HOME_COUNTDOWN_YPOS, LCD_CURSOR_BY_FONT);

  if ((ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE) && \
      (ProgramManager_GetCurrentState() != PROGRAMMANAGER_STATE_FAIL))
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_CountdownEnableStr, \
                             ProgramManager_gTimeCountMin, \
                             ProgramManager_gTimeCountSec);
    
    LCD_PutString(tempStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Home_CountdownDisableStr);
  }
}

/*=============================================================================================*/
static void MenuManager_Home_LcdShowTemperature(void)
{
  uint8_t tempStr[20];

  LCD_SetCursorPos(MENUMANAGER_HOME_TEMPSTR_XPOS, MENUMANAGER_HOME_TEMPSTR_YPOS, LCD_CURSOR_BY_FONT);

  LCD_PutString((uint8_t *)MenuManager_Home_TempStr);

  LCD_SetCursorPos(MENUMANAGER_HOME_TEMPERATURE_XPOS, MENUMANAGER_HOME_TEMPERATURE_YPOS, LCD_CURSOR_BY_FONT);

  if (ProgramManager_gCurrentTempThreshold != PROGRAMMANAGER_CONTROL_TEMP_INVALID_VALUE)
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_TemperatureEnableStr, \
                             ProgramManager_gCurrentTemperature, \
                             ProgramManager_gCurrentTempThreshold);
  }
  else
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_TemperatureDisableStr, \
                             ProgramManager_gCurrentTemperature);
  }

  LCD_PutString(tempStr);

  LCD_SetCursorPos(MENUMANAGER_HOME_TEMPERATURE_UNIT_XPOS, MENUMANAGER_HOME_TEMPERATURE_UNIT_YPOS, LCD_CURSOR_BY_FONT);

  if (ProgramManager_gParamConfig.machineFuncCfg.tempUnit == PROGRAMMANAGER_TEMP_UNIT_CELSIUS)
  {
    LCD_PutString((uint8_t *)MenuManager_Common_CelsiusDegStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Common_FahrenheitDegStr);
  }
}

/*=============================================================================================*/
static void MenuManager_Home_LcdShowPressure(void)
{
  uint8_t tempStr[20];

  LCD_SetCursorPos(MENUMANAGER_HOME_PRESSTR_XPOS, MENUMANAGER_HOME_PRESSTR_YPOS, LCD_CURSOR_BY_FONT);

  LCD_PutString((uint8_t *)MenuManager_Home_PresStr);

  LCD_SetCursorPos(MENUMANAGER_HOME_PRESSURE_XPOS, MENUMANAGER_HOME_PRESSURE_YPOS, LCD_CURSOR_BY_FONT);

  if (ProgramManager_gCurrentPresThreshold != PROGRAMMANAGER_CONTROL_PRES_INVALID_VALUE)
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_PressureEnableStr, \
                             ProgramManager_gCurrentPressure, \
                             ProgramManager_gCurrentPresThreshold);
  }
  else
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_PressureDisableStr, \
                             ProgramManager_gCurrentPressure);
  }

  LCD_PutString(tempStr);
}



/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_Entry(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  HAL_StatusTypeDef retVal = HAL_OK;

  /* Check if previous state data hierachy is not empty */
  if (pFsmContext->dataHierachy != NULL)
  {
    if (pFsmContext->dataHierachy->dataId == MENUMANAGER_STATE_MAIN_SETTING)
    {
      /* Release previous state data hierachy */
      MenuManager_free(pFsmContext->dataHierachy);
      pFsmContext->dataHierachy = NULL;
    }
    else
    {
      retVal = HAL_ERROR;
    }
  }
  /* First time come here from INIT state */
  else
  {
    MenuManager_InternalDataPush(MENUMANAGER_HOME_INTERNALDATALENGTH);
  }

  if (retVal == HAL_OK)
  {
    MenuManager_Home_NotifyState = (uint32_t)0U;
    MenuManager_Home_SubMainFuncCounter = (uint32_t)0U;
    
    MenuManager_Common_LcdClearAll();

    LCD_SetFont(LCD_FONT_SMALL);

    MenuManager_SubMainFunction = MenuManager_Home_SubMainFunction;
    MenuManager_SubTickHandler = MenuManager_Home_SubTickHandler;

    return FSM_GUARD_TRUE;
  }
  
  return FSM_GUARD_FALSE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_Exit(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_DataHierachyStruct* dataHierachy;

  MenuManager_SubMainFunction = NULL;
  MenuManager_SubTickHandler = NULL;

  dataHierachy = (Fsm_DataHierachyStruct *)MenuManager_malloc(sizeof(Fsm_DataHierachyStruct));
  dataHierachy->dataId = MENUMANAGER_STATE_HOME;

  pFsmContext->dataHierachy = dataHierachy;
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_StartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_START);
  }
  else if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME);
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_STOP);
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_LongStartBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_START);
  }
  else if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME);
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_LongStopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_STOP);
  }

  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    if (ProgramManager_gAutoSeqConfig.sequenceIndex < ((uint8_t)(PROGRAMMANAGER_SEQUENCE_NUM_MAX) - (uint8_t)1U))
    {
      (ProgramManager_gAutoSeqConfig.sequenceIndex)++;

      ProgramManager_SequenceIndex_SetData(ProgramManager_gAutoSeqConfig.sequenceIndex);

      ProgramManager_AutoSeqConfig_GetData(&ProgramManager_gAutoSeqConfig);
    }

    /* Reset counter */
    MenuManager_Home_SubMainFuncCounter = (uint32_t)0U;
  }
  else if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_NEXT_SUBSTEP);
  }
  else
  {
    /* Should never executed here */
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    if (ProgramManager_gAutoSeqConfig.sequenceIndex > (uint8_t)0U)
    {
      (ProgramManager_gAutoSeqConfig.sequenceIndex)--;

      ProgramManager_SequenceIndex_SetData(ProgramManager_gAutoSeqConfig.sequenceIndex);

      ProgramManager_AutoSeqConfig_GetData(&ProgramManager_gAutoSeqConfig);
    }

    /* Reset counter */
    MenuManager_Home_SubMainFuncCounter = (uint32_t)0U;
  }
  else if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_PREV_SUBSTEP);
  }
  else
  {
    /* Should never executed here */
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_UpDownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_AddBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    if (ProgramManager_gAutoSeqConfig.currentStep < ((uint8_t)(PROGRAMMANAGER_STEP_NUM_MAX) - (uint8_t)2U))
    {
      (ProgramManager_gAutoSeqConfig.currentStep)++;
    }

    /* Reset counter */
    MenuManager_Home_SubMainFuncCounter = (uint32_t)0U;
  }
  else if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_NEXT_STEP);
  }
  else
  {
    /* Should never executed here */
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_SubBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    if (ProgramManager_gAutoSeqConfig.currentStep > (uint8_t)0U)
    {
      (ProgramManager_gAutoSeqConfig.currentStep)--;
    }

    /* Reset counter */
    MenuManager_Home_SubMainFuncCounter = (uint32_t)0U;
  }
  else if (ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE)
  {
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_PREV_STEP);
  }
  else
  {
    /* Should never executed here */
  }

  return FSM_GUARD_TRUE;
}



/*=============================================================================================*/
static void MenuManager_Home_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_Home_ButEventMapConf);

  MenuManager_Home_SubMainFuncCounter += (uint32_t)1U;

  if (MenuManager_Home_SubMainFuncCounter == MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_UPDATE_1)
  {
    MenuManager_Home_LcdShowSequenceStep();
    MenuManager_Home_LcdShowNotifyState();
  }
  else if (MenuManager_Home_SubMainFuncCounter == MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_UPDATE_2)
  {
    MenuManager_Home_LcdShowCountDown();
  }
  else if (MenuManager_Home_SubMainFuncCounter == MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_UPDATE_3)
  {
    MenuManager_Home_LcdShowTemperature();
    MenuManager_Home_LcdShowPressure();
  }
  else if (MenuManager_Home_SubMainFuncCounter >= MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_MAX)
  {
    MenuManager_Common_LcdUpdateAll();

    MenuManager_Home_SubMainFuncCounter = (uint32_t)0U;
  }
}

/*=============================================================================================*/
static void MenuManager_Home_SubTickHandler(void)
{
  /* Do nothing */
}



#ifdef  __cplusplus
}
#endif
