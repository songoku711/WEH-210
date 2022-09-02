/* 
 * File:   programManager_Common.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef PROGRAMMANAGER_COMMON_H
#define PROGRAMMANAGER_COMMON_H

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




/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

void ProgramManager_Common_CelsiusToFahrenheitConv(uint8_t *inCel, uint8_t *outFah);
void ProgramManager_Common_FahrenheitToCelsiusConv(uint8_t *inFah, uint8_t *outCel);



#ifdef  __cplusplus
}
#endif

#endif  /* PROGRAMMANAGER_COMMON_H */
