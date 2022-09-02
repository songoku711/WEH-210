/* 
 * File:   MenuManager_Types.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef MENUMANAGER_TYPES_H
#define MENUMANAGER_TYPES_H

#ifdef  __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "main.h"

#include "lcd.h"
#include "fsm.h"
#include "memIf.h"
#include "cmsis_os.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define MENUMANAGER_SUBMENU_TREE_DEPTH                                (10U)

/** Menu manager application states */
typedef enum
{
  MENUMANAGER_STATE_POWER_ON,
  MENUMANAGER_STATE_INIT,
  MENUMANAGER_STATE_HOME,
    MENUMANAGER_STATE_MAIN_SETTING,
      MENUMANAGER_STATE_SET_TO_DEFAULT,
        MENUMANAGER_STATE_SET_DEFAULT_PARAM,
        MENUMANAGER_STATE_SET_DEFAULT_PROG,
      MENUMANAGER_STATE_MACHINE_SETUP,
        MENUMANAGER_STATE_MACHINE_FUNC_SETUP,
          MENUMANAGER_STATE_DRAIN_WHILE_DOOR_OPEN,
          MENUMANAGER_STATE_HEAT_USE_TIMEOUT,
          MENUMANAGER_STATE_FILL_USE_TIMEOUT,
          MENUMANAGER_STATE_MAN_OPERATE_WHEN_AUTO,
          MENUMANAGER_STATE_TEMP_UNIT,
          MENUMANAGER_STATE_DRAIN_VALVE_STATUS,
        MENUMANAGER_STATE_INPUT_STATUS_SETUP,
          MENUMANAGER_STATE_DOOR_CLOSED,
          MENUMANAGER_STATE_EXTRACT_SHOCK,
          MENUMANAGER_STATE_EMERGENCY_STOP,
          MENUMANAGER_STATE_INVERTER_ERROR,
        MENUMANAGER_STATE_FILL_LEVEL_SETUP,
          MENUMANAGER_STATE_AUTO_REFILL_WHEN_LOW,
          MENUMANAGER_STATE_ZERO_LEVEL,
          MENUMANAGER_STATE_LOW_LEVEL,
          MENUMANAGER_STATE_MID_LEVEL,
          MENUMANAGER_STATE_HIGH_LEVEL,
          MENUMANAGER_STATE_OVERBRIM_LEVEL,
          MENUMANAGER_STATE_LEVEL_DIFF_REFILL,
          MENUMANAGER_STATE_MAX_TIME_REFILL,
        MENUMANAGER_STATE_HEAT_TEMP_SETUP,
          MENUMANAGER_STATE_AUTO_REHEAT_WHEN_LOW,
          MENUMANAGER_STATE_MIN_WATER_TEMP,
          MENUMANAGER_STATE_MAX_WATER_TEMP,
          MENUMANAGER_STATE_TEMP_THRESHOLD,
          MENUMANAGER_STATE_TEMP_DIFF_REHEAT,
          MENUMANAGER_STATE_MAX_TIME_HEAT,
        MENUMANAGER_STATE_SOAP_SETUP,
        MENUMANAGER_STATE_WASH_SETUP,
        MENUMANAGER_STATE_EXTRACT_SETUP,
        MENUMANAGER_STATE_DOOR_LOCK_SETUP,
  MENUMANAGER_STATE_FAIL
} MenuManager_StateType;

/** Menu manager application events */
typedef enum
{
  MENUMANAGER_EVENT_PREV = kFsmDefaultEvent_Count,
  MENUMANAGER_EVENT_NEXT,
  MENUMANAGER_EVENT_START_BUT,
  MENUMANAGER_EVENT_LONG_START_BUT,
  MENUMANAGER_EVENT_STOP_BUT,
  MENUMANAGER_EVENT_LONG_STOP_BUT,
  MENUMANAGER_EVENT_UP_BUT,
  MENUMANAGER_EVENT_DOWN_BUT,
  MENUMANAGER_EVENT_UP_DOWN_BUT,
  MENUMANAGER_EVENT_ADD_BUT,
  MENUMANAGER_EVENT_SUB_BUT,
  MENUMANAGER_EVENT_WASH_BUT,
  MENUMANAGER_EVENT_COLDWTR_BUT,
  MENUMANAGER_EVENT_HOTWTR_BUT,
  MENUMANAGER_EVENT_HEAT_BUT,
  MENUMANAGER_EVENT_LEVEL_BUT,
  MENUMANAGER_EVENT_SUPP1_BUT,
  MENUMANAGER_EVENT_SUPP2_BUT,
  MENUMANAGER_EVENT_SUPP3_BUT,
  MENUMANAGER_EVENT_DRAIN_BUT,
  MENUMANAGER_EVENT_EXTRACT_BUT,
  MENUMANAGER_EVENT_SUBMENU_1,
  MENUMANAGER_EVENT_SUBMENU_2,
  MENUMANAGER_EVENT_SUBMENU_3,
  MENUMANAGER_EVENT_SUBMENU_4,
  MENUMANAGER_EVENT_SUBMENU_5,
  MENUMANAGER_EVENT_SUBMENU_6,
  MENUMANAGER_EVENT_SUBMENU_7,
  MENUMANAGER_EVENT_SUBMENU_8,
  MENUMANAGER_EVENT_SUBMENU_9,
  MENUMANAGER_EVENT_SUBMENU_10
} MenuManager_EventType;



/** Menu manager button event mapping element structure */
typedef struct
{
  uint32_t butState;
  MenuManager_EventType butEvent;
} MenuManager_ButEventMapStruct;

/** Menu manager button event mapping configuration structure */
typedef struct
{
  uint8_t butEventMapNum;
  MenuManager_ButEventMapStruct (* butEventMapCfg)[];
} MenuManager_ButEventMapConfStruct;

/** Menu manager child menu element structure */
typedef struct
{
  const uint8_t (* childMenuTitleStr)[];
  MenuManager_EventType childMenuEvent;
} MenuManager_ChildMenuStruct;

/** Menu manager child menu configuration structure */
typedef struct
{
  uint8_t childMenuNum;
  MenuManager_ChildMenuStruct (* childMenuCfg)[];
} MenuManager_ChildMenuConfStruct;

/** Menu manager internal data structure */
typedef struct
{
  uint8_t length;
  uint32_t *ptr;
} MenuManager_InternalDataElementStruct;

/** Menu manager internal data stack structure */
typedef struct
{
  uint8_t internalDataCurIdx;
  MenuManager_InternalDataElementStruct internalDataArr[MENUMANAGER_SUBMENU_TREE_DEPTH];
} MenuManager_InternalDataStruct;



#ifdef  __cplusplus
}
#endif

#endif  /* MENUMANAGER_TYPES_H */
