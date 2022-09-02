/* 
 * File:   programManager_Common.c
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

#include "programManager.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/




/*===============================================================================================
*                                        ENUMERATIONS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/




/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void ProgramManager_Common_CelsiusToFahrenheitConv(uint8_t *inCel, uint8_t *outFah)
{
  uint32_t tempCel;
  uint32_t tempFah;

  tempCel = (uint32_t)(*inCel);

  tempCel = tempCel * (uint32_t)9U;
  tempCel = tempCel / (uint32_t)5U;

  tempFah = tempCel + (uint32_t)32U;

  *outFah = (uint8_t)tempFah;
}

void ProgramManager_Common_FahrenheitToCelsiusConv(uint8_t *inFah, uint8_t *outCel)
{
  uint32_t tempCel;
  uint32_t tempFah;

  tempFah = (uint32_t)(*inFah);

  tempFah = tempFah - (uint32_t)32U;
  tempFah = tempFah * (uint32_t)5U;

  tempCel = tempFah / (uint32_t)9U;

  *outCel = (uint8_t)tempCel;
}



#ifdef  __cplusplus
}
#endif
