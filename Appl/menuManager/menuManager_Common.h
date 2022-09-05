/* 
 * File:   MenuManager_Commons.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef MENUMANAGER_COMMONS_H
#define MENUMANAGER_COMMONS_H

#ifdef  __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "menuManager_Types.h"




/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define MENUMANAGER_COMMON_LCD_CURSOR_MIN                             (2U)
#define MENUMANAGER_COMMON_LCD_CURSOR_MAX                             (7U)

#define MENUMANAGER_COMMON_LCD_CURSOR_LENGTH                          (6U)

#define MENUMANAGER_COMMON_LCD_ELEMENT_MENU_XPOS                      (1U)
#define MENUMANAGER_COMMON_LCD_ELEMENT_MENU_LENGTH                    (18U)



extern const uint8_t MenuManager_Common_DoneStr[];

extern const uint8_t MenuManager_Common_EnableStr[];
extern const uint8_t MenuManager_Common_DisableStr[];

extern const uint8_t MenuManager_Common_NormalOpenStr[];
extern const uint8_t MenuManager_Common_NormalCloseStr[];

extern const uint8_t MenuManager_Common_ActiveLowStr[];
extern const uint8_t MenuManager_Common_ActiveHighStr[];

extern const uint8_t MenuManager_Common_CelsiusDegStr[];
extern const uint8_t MenuManager_Common_FahrenheitDegStr[];

extern const uint8_t MenuManager_Common_SecondStr[];

extern const uint8_t MenuManager_Common_LevelStr[];



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void MenuManager_Common_ButEventMapHandler(MenuManager_ButEventMapConfStruct *butEventMapConf);

void MenuManager_Common_DecToBcdConv(uint32_t *inDec, uint32_t *outBcd, uint8_t unitNum);
void MenuManager_Common_BcdToDecConv(uint32_t *outDec, uint32_t *inBcd, uint8_t unitNum);

tFsmGuard MenuManager_Common_Exit(tFsmContextPtr const pFsmContext, tFsmEvent event);

uint32_t MenuManager_Common_LcdGetCurPosFirstTime(uint32_t listIndex, uint32_t listItemNumber);
uint32_t MenuManager_Common_LcdGetStartListIndex(uint32_t listIndex, uint32_t cursorPos);
uint32_t MenuManager_Common_LcdGetListLength(uint32_t listItemNumber);
uint32_t MenuManager_Common_LcdMaxCurPos(uint32_t listItemNumber);

void MenuManager_Common_LcdClearAll(void);
void MenuManager_Common_LcdUpdateAll(void);
void MenuManager_Common_LcdClearElementMenuStatic(void);
void MenuManager_Common_LcdClearElementMenuDynamic(void);
void MenuManager_Common_LcdUpdateElementMenuStatic(void);
void MenuManager_Common_LcdUpdateElementMenuDynamic(void);
void MenuManager_Common_LcdClearAdjustMenuStatic(void);
void MenuManager_Common_LcdClearAdjustMenuDynamic(void);
void MenuManager_Common_LcdUpdateAdjustMenuStatic(void);
void MenuManager_Common_LcdUpdateAdjustMenuDynamic(void);
void MenuManager_Common_LcdClearCountdownMenuStatic(void);
void MenuManager_Common_LcdClearCountdownMenuDynamic(void);
void MenuManager_Common_LcdUpdateCountdownMenuStatic(void);
void MenuManager_Common_LcdUpdateCountdownMenuDynamic(void);

void MenuManager_Common_LcdShowMainTitle(const uint8_t *mainTitle);
void MenuManager_Common_LcdShowChosenMark(uint8_t curPos);
void MenuManager_Common_LcdShowListElementName(uint8_t* str, uint8_t length, uint8_t xPos,	uint8_t yPos);
void MenuManager_Common_LcdShowAdjustUnitVal( uint32_t xPos, uint32_t yPos, uint32_t *unitVal, uint32_t length);
void MenuManager_Common_LcdShowAdjustArrow(uint32_t xPos, uint32_t yPos, uint32_t curPos);
void MenuManager_Common_LcdShowExitChoice(void);
void MenuManager_Common_LcdShowOkExitChoice(void);
void MenuManager_Common_LcdShowStopExitChoice(void);
void MenuManager_Common_LcdShowStartExitChoice(void);
void MenuManager_Common_LcdShowScrollBar(uint8_t listIndex, uint8_t cursorPosition, uint8_t listItemNumber);



#ifdef  __cplusplus
}
#endif

#endif  /* MENUMANAGER_COMMONS_H */
