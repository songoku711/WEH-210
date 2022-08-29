/* 
 * File:   _A000000000_menu_PowerOn.c
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
static const uint8_t MenuManager_PowerOn_TitleStr[] =                 "WASHER EXTRACTOR";



/** Menu manager event handlers */
static tFsmGuard MenuManager_PowerOn_Entry                            (tFsmContextPtr const pFsmContext, tFsmEvent event);

/** Menu manager state machine */
tFsmEventEntry MenuManager_PowerOn_StateMachine[2] =
{
  FSM_TRIGGER_ENTRY           (                                       MenuManager_PowerOn_Entry                                                       ),
  FSM_TRIGGER_TRANSITION      ( MENUMANAGER_EVENT_NEXT,                                                       MENUMANAGER_STATE_INIT                  )
};



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

static tFsmGuard MenuManager_PowerOn_Entry(tFsmContextPtr const pFsmContext, tFsmEvent event)
{
  MenuManager_Common_LcdClearAll();

  LCD_SetCursorPos(0, 0, LCD_CURSOR_IN_PIXEL);
  LCD_PutRectangle(128, 64);
  
  LCD_SetFont(LCD_FONT_SMALL);
  LCD_SetCursorPos(16, 16, LCD_CURSOR_IN_PIXEL);
  LCD_PutString((uint8_t *)MenuManager_PowerOn_TitleStr);
  
  MenuManager_Common_LcdUpdateAll();
  
	return kFsmGuard_True;
}



#ifdef  __cplusplus
}
#endif
