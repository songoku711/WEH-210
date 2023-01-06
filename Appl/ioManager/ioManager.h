/* 
 * File:   ioManager.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef IOMANAGER_H
#define IOMANAGER_H

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

#include "tim.h"
#include "gpio.h"
#include "cmsis_os.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define IOMANAGER_BUTTON_PRESS_SHIFT                  (16U)

#define IO_LONG_PRESS(state)                          (state | (state << IOMANAGER_BUTTON_PRESS_SHIFT))

typedef enum
{
  IOMANAGER_STATE_FAIL,
  IOMANAGER_STATE_INIT,
  IOMANAGER_STATE_READY
} IoManager_StateType;

typedef enum
{
  IOMANAGER_BUTTON_STATE_START                        = (uint32_t)0x0001,
  IOMANAGER_BUTTON_STATE_STOP                         = (uint32_t)0x0002,
  IOMANAGER_BUTTON_STATE_UP                           = (uint32_t)0x0004,
  IOMANAGER_BUTTON_STATE_DOWN                         = (uint32_t)0x0008,
  IOMANAGER_BUTTON_STATE_ADD                          = (uint32_t)0x0010,
  IOMANAGER_BUTTON_STATE_SUB                          = (uint32_t)0x0020,
  IOMANAGER_BUTTON_STATE_WASH                         = (uint32_t)0x0040,
  IOMANAGER_BUTTON_STATE_COLD                         = (uint32_t)0x0080,
  IOMANAGER_BUTTON_STATE_HOT                          = (uint32_t)0x0100,
  IOMANAGER_BUTTON_STATE_HEAT                         = (uint32_t)0x0200,
  IOMANAGER_BUTTON_STATE_LEVEL                        = (uint32_t)0x0400,
  IOMANAGER_BUTTON_STATE_SUPP1                        = (uint32_t)0x0800,
  IOMANAGER_BUTTON_STATE_SUPP2                        = (uint32_t)0x1000,
  IOMANAGER_BUTTON_STATE_SUPP3                        = (uint32_t)0x2000,
  IOMANAGER_BUTTON_STATE_DRAIN                        = (uint32_t)0x4000,
  IOMANAGER_BUTTON_STATE_EXTRACT                      = (uint32_t)0x8000
} IoManager_ButStateType;

typedef enum
{
  IOMANAGER_BUTTON_PRESS_SHORT                        = (uint32_t)0x01,
  IOMANAGER_BUTTON_PRESS_LONG                         = (uint32_t)0x02
} IoManager_ButPressType;



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void IoManager_Init(void);

IoManager_StateType IoManager_GetCurrentState(void);

void IoManager_MainFunction(void);
void IoManager_ExtIrptMainFunction(void);

void IoManager_ExtIrptCallback(uint32_t arg);



#ifdef  __cplusplus
}
#endif

#endif  /* IOMANAGER_H */
