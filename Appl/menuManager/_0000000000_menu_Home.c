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

/** Menu manager main titles and child menu titles */
static const uint8_t MenuManager_Home_MainTitleStr[] = "HOME";



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
static tFsmGuard MenuManager_Home_Entry                               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_Home_StartBut                            (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_Home_StopBut                             (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_Home_LongStartBut                        (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_Home_LongStopBut                         (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_Home_UpBut                               (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_Home_DownBut                             (tFsmContextPtr const pFsmContext, tFsmEvent event);
static tFsmGuard MenuManager_Home_UpDownBut                           (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_Home_StateMachine[10] =
{
  FSM_TRIGGER_ENTRY           (                                       MenuManager_Home_Entry                                                          ),
  FSM_TRIGGER_EXIT            (                                       MenuManager_Common_Exit                                                         ),
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

static void MenuManager_Home_SubMainFunction(void);
static void MenuManager_Home_SubTickHandler(void);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static tFsmGuard MenuManager_Home_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_SubMainFunction = MenuManager_Home_SubMainFunction;
  MenuManager_SubTickHandler = MenuManager_Home_SubTickHandler;

  MenuManager_Common_LcdClearAll();

  MenuManager_Common_LcdShowMainTitle(MenuManager_Home_MainTitleStr);
  
  MenuManager_Common_LcdUpdateAll();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_Home_StartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint8_t str[] = "START      ";
  LCD_SetCursorPos(24, 24, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)str);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_Home_StopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint8_t str[] = "STOP       ";
  LCD_SetCursorPos(24, 24, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)str);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_Home_LongStartBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint8_t str[] = "START LONG ";
  LCD_SetCursorPos(24, 24, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)str);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_Home_LongStopBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint8_t str[] = "STOP LONG  ";
  LCD_SetCursorPos(24, 24, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)str);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_Home_UpBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint8_t str[] = "UP         ";
  LCD_SetCursorPos(24, 24, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)str);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_Home_DownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  uint8_t str[] = "DOWN       ";
  LCD_SetCursorPos(24, 24, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)str);
  
  MenuManager_Common_LcdUpdateElementMenuDynamic();
  
  return kFsmGuard_True;
}

/*=============================================================================================*/
static tFsmGuard MenuManager_Home_UpDownBut(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
//  uint8_t str[] = "UP DOWN    ";
//  LCD_SetCursorPos(24, 24, LCD_CURSOR_IN_PIXEL);
//  LCD_PutString((uint8_t *)str);
//  
//  MenuManager_Common_LcdUpdateElementMenuDynamic();

  Fsm_TriggerEvent(&MenuManager_FsmContext, (tFsmEvent)MENUMANAGER_EVENT_SUBMENU_1);
  
  return kFsmGuard_True;
}



/*=============================================================================================*/
static void MenuManager_Home_SubMainFunction(void)
{
  MenuManager_Common_ButEventMapHandler(&MenuManager_Home_ButEventMapConf);
}

/*=============================================================================================*/
static void MenuManager_Home_SubTickHandler(void)
{
  
}



#ifdef  __cplusplus
}
#endif
