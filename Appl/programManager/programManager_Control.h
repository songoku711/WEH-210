/* 
 * File:   programManager_Control.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef PROGRAMMANAGER_CONTROL_H
#define PROGRAMMANAGER_CONTROL_H

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

#include "ProgramManager_Types.h"



/*===============================================================================================
*                                     DEFINES AND MACROS
===============================================================================================*/

#define PROGRAMMANAGER_CONTROL_SLAVE_ADDR                             (uint8_t)1U

#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ADDR                      (uint16_t)1U
#define PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NUM                       (uint16_t)4U

#define PROGRAMMANAGER_CONTROL_TEMP_PRES_ADDR                         (uint16_t)1U
#define PROGRAMMANAGER_CONTROL_TEMP_PRES_NUM                          (uint16_t)2U

#define PROGRAMMANAGER_CONTROL_RELAY_ADDR                             (uint16_t)1U
#define PROGRAMMANAGER_CONTROL_RELAY_NUM                              (uint16_t)1U



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void ProgramManager_Control_Init(void);

void ProgramManager_Control_TxRxSignalSubMainFunction(void);



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_CONTROL_H */
