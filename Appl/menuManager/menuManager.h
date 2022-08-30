/* 
 * File:   menuManager.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef MENUMANAGER_H
#define MENUMANAGER_H

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

#include "fsm.h"
#include "memIf.h"
#include "cmsis_os.h"

#include "ioManager.h"
#include "programManager.h"
#include "menuManager_Common.h"
#include "menuManager_Types.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define MenuManager_malloc(size)                                      pvPortMalloc(size)
#define MenuManager_free(ptr)                                         vPortFree(ptr)

#define MenuManager_GetInternalDataPtr(index)                         *(MenuManager_InternalData.internalDataArr[MenuManager_InternalData.internalDataCurIdx].ptr + index)

/** Menu internal data structure */
extern MenuManager_InternalDataStruct MenuManager_InternalData;

/** Context Structure of the Finite State Machine (FSM) */
extern tFsmContext MenuManager_FsmContext;

extern void (*MenuManager_SubMainFunction)(void);
extern void (*MenuManager_SubTickHandler)(void);



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void MenuManager_Init(void);
void MenuManager_MainFunction(void);

void MenuManager_InternalDataPush(uint8_t length);
void MenuManager_InternalDataPop(void);



#ifdef  __cplusplus
}
#endif

#endif  /* MENUMANAGER_H */
