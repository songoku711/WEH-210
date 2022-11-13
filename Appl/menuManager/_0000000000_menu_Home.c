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
#define MENUMANAGER_HOME_SUBMAINFUNCCOUNTER_MAX                       (uint32_t)40U

#define MENUMANAGER_HOME_SEQUENCE_INDEX_XPOS                          (uint32_t)0U
#define MENUMANAGER_HOME_SEQUENCE_INDEX_YPOS                          (uint32_t)0U

#define MENUMANAGER_HOME_STEP_INDEX_XPOS                              (uint32_t)14U
#define MENUMANAGER_HOME_STEP_INDEX_YPOS                              (uint32_t)0U

#define MENUMANAGER_HOME_NOTIFY_STATE_XPOS                            (uint32_t)1U
#define MENUMANAGER_HOME_NOTIFY_STATE_YPOS                            (uint32_t)2U

#define MENUMANAGER_HOME_COUNTDOWN_XPOS                               (uint32_t)16U
#define MENUMANAGER_HOME_COUNTDOWN_YPOS                               (uint32_t)5U

#define MENUMANAGER_HOME_TEMPERATURE_XPOS                             (uint32_t)0U
#define MENUMANAGER_HOME_TEMPERATURE_YPOS                             (uint32_t)7U

#define MENUMANAGER_HOME_TEMPERATURE_UNIT_XPOS                        (uint32_t)7U
#define MENUMANAGER_HOME_TEMPERATURE_UNIT_YPOS                        (uint32_t)7U

#define MENUMANAGER_HOME_PRESSURE_XPOS                                (uint32_t)14U
#define MENUMANAGER_HOME_PRESSURE_YPOS                                (uint32_t)7U



/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_Home_SequenceIdxStr[] =              "PROGRAM %02d";
static const uint8_t MenuManager_Home_StepIdxStr[] =                  "STEP %02d";
static const uint8_t MenuManager_Home_NotifyStateNotAvailStr[] =      "NOT AVAILABLE";
static const uint8_t MenuManager_Home_NotifyStateDoorOpenStr[] =      "DOOR OPEN    ";
static const uint8_t MenuManager_Home_NotifyStateReadyStr[] =         "READY        ";
static const uint8_t MenuManager_Home_NotifyStateRunningStr[] =       "RUNNING      ";
static const uint8_t MenuManager_Home_CountdownEnableStr[] =          "%02d:%02d";
static const uint8_t MenuManager_Home_CountdownDisableStr[] =         "--:--";
static const uint8_t MenuManager_Home_TemperatureEnableStr[] =        "%3d/%3d";
static const uint8_t MenuManager_Home_TemperatureDisableStr[] =       "%3d/---";
static const uint8_t MenuManager_Home_PressureEnableStr[] =           "%3d/%3d";
static const uint8_t MenuManager_Home_PressureDisableStr[] =          "%3d/---";



/** Menu manager button event mapping array */
static MenuManager_ButEventMapStruct MenuManager_Home_ButEventMap[7] =
{
  { IO_LONG_PRESS(IOMANAGER_BUTTON_STATE_START),                      MENUMANAGER_EVENT_LONG_START_BUT        },
  { IO_LONG_PRESS(IOMANAGER_BUTTON_STATE_STOP),                       MENUMANAGER_EVENT_LONG_STOP_BUT         },
  { IOMANAGER_BUTTON_STATE_START,                                     MENUMANAGER_EVENT_START_BUT             },
  { IOMANAGER_BUTTON_STATE_STOP,                                      MENUMANAGER_EVENT_STOP_BUT              },
  { IOMANAGER_BUTTON_STATE_UP,                                        MENUMANAGER_EVENT_UP_BUT                },
  { IOMANAGER_BUTTON_STATE_DOWN,                                      MENUMANAGER_EVENT_DOWN_BUT              },
  { IOMANAGER_BUTTON_STATE_UP | IOMANAGER_BUTTON_STATE_DOWN,          MENUMANAGER_EVENT_UP_DOWN_BUT           }
};

/** Menu manager button event mapping configuration */
static MenuManager_ButEventMapConfStruct MenuManager_Home_ButEventMapConf =
{
  .butEventMapNum             = (uint8_t)7U,
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
static Fsm_GuardType MenuManager_Home_UpDownBut                       (Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event);

/** Menu manager state machine */
Fsm_EventEntryStruct MenuManager_Home_StateMachine[10] =
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
  FSM_TRIGGER_INTERNAL        ( MENUMANAGER_EVENT_UP_DOWN_BUT,        MenuManager_Home_UpDownBut                                                      )
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
  LCD_SetCursorPos(MENUMANAGER_HOME_NOTIFY_STATE_XPOS, MENUMANAGER_HOME_NOTIFY_STATE_YPOS, LCD_CURSOR_BY_FONT);

  if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == false)
  {
    LCD_PutString((uint8_t *)MenuManager_Home_NotifyStateNotAvailStr);
  }
  else if (ProgramManager_gSensorDoorOpenErr != (uint8_t)0U)
  {
    LCD_PutString((uint8_t *)MenuManager_Home_NotifyStateDoorOpenStr);
  }
  else if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    LCD_PutString((uint8_t *)MenuManager_Home_NotifyStateReadyStr);
  }
  else if ((ProgramManager_GetCurrentState() > PROGRAMMANAGER_STATE_IDLE) && \
           (ProgramManager_GetCurrentState() != PROGRAMMANAGER_STATE_FAIL))
  {
    LCD_PutString((uint8_t *)MenuManager_Home_NotifyStateRunningStr);
  }
  else
  {
    LCD_PutString((uint8_t *)MenuManager_Home_NotifyStateNotAvailStr);
  }
}

/*=============================================================================================*/
static void MenuManager_Home_LcdShowCountDown(void)
{
  uint8_t tempStr[20];

  LCD_SetCursorPos(MENUMANAGER_HOME_COUNTDOWN_XPOS, MENUMANAGER_HOME_COUNTDOWN_YPOS, LCD_CURSOR_BY_FONT);

  if (((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == true) && \
      ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].timeoutCountMode != (uint8_t)0U))
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_CountdownEnableStr, \
                             (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].timeoutMinute, \
                             (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].timeoutSecond);
  
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

  LCD_SetCursorPos(MENUMANAGER_HOME_TEMPERATURE_XPOS, MENUMANAGER_HOME_TEMPERATURE_YPOS, LCD_CURSOR_BY_FONT);

  if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == true)
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_TemperatureEnableStr, \
                             ProgramManager_gCurrentTemperature, \
                             (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].tempThreshold);
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

  LCD_SetCursorPos(MENUMANAGER_HOME_PRESSURE_XPOS, MENUMANAGER_HOME_PRESSURE_YPOS, LCD_CURSOR_BY_FONT);

  if ((ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].isActive == true)
  {
    sprintf((char *)tempStr, (const char *)MenuManager_Home_PressureEnableStr, \
                             ProgramManager_gCurrentPressure, \
                             (ProgramManager_gAutoSeqConfig.normStep)[ProgramManager_gAutoSeqConfig.currentStep].levelThreshold);
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
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_PAUSE_RESUME);
  }
  else
  {
    /* Should never executed here */
  }
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_StopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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
    ProgramManager_Control_SetCommand(PROGRAMMANAGER_CONTROL_COMMAND_STOP);
  }
  else
  {
    /* Should never executed here */
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
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_LongStopBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  
  
  return FSM_GUARD_TRUE;
}

/*=============================================================================================*/
static Fsm_GuardType MenuManager_Home_UpBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  if (ProgramManager_GetCurrentState() == PROGRAMMANAGER_STATE_IDLE)
  {
    if (ProgramManager_gAutoSeqConfig.currentStep < ((uint8_t)(PROGRAMMANAGER_STEP_NUM_MAX) - (uint8_t)1U))
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
static Fsm_GuardType MenuManager_Home_DownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
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
static Fsm_GuardType MenuManager_Home_UpDownBut(Fsm_ContextStructPtr const pFsmContext, Fsm_EventType event)
{
  Fsm_TriggerEvent(&MenuManager_FsmContext, (Fsm_EventType)MENUMANAGER_EVENT_SUBMENU_1);
  
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
  
}



#ifdef  __cplusplus
}
#endif
