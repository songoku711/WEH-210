/* 
 * File:   programManager_Control.c
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

#include "eMB.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_1              (uint8_t)20U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_TEMP_PRES                   (uint8_t)40U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_2              (uint8_t)60U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_RELAY                       (uint8_t)80U
#define PROGRAMMANAGER_CONTROL_RECVSIGNAL_COUNT_MAX                   (uint8_t)100U



/*===============================================================================================
*                                        ENUMERATIONS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

static uint8_t ProgramManager_Control_RecvSignalCount;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/




/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/




/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

void ProgramManager_Control_Init(void)
{
  ProgramManager_Control_RecvSignalCount = (uint8_t)0U;
}

/* Task periodic time is 5ms */
void ProgramManager_Control_TxRxSignalSubMainFunction(void)
{
  ProgramManager_Control_RecvSignalCount++;

  if ( \
      (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_1) || \
      (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_INPUT_SENSOR_2) \
     )
  {
    (void)eMB_Master_RequestReadDiscreteInputs
    (
      PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
      PROGRAMMANAGER_CONTROL_INPUT_SENSOR_ADDR,
      PROGRAMMANAGER_CONTROL_INPUT_SENSOR_NUM
    );
  }
  else if (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_TEMP_PRES)
  {
    (void)eMB_Master_RequestReadInputRegister
    (
      PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
      PROGRAMMANAGER_CONTROL_TEMP_PRES_ADDR,
      PROGRAMMANAGER_CONTROL_TEMP_PRES_NUM
    );
  }
  else if (ProgramManager_Control_RecvSignalCount == PROGRAMMANAGER_CONTROL_RECVSIGNAL_RELAY)
  {
    (void)eMB_Master_RequestWriteHoldingRegister
    (
      PROGRAMMANAGER_CONTROL_SLAVE_ADDR,
      PROGRAMMANAGER_CONTROL_RELAY_ADDR,
      PROGRAMMANAGER_CONTROL_RELAY_NUM
    );
  }
  else if (ProgramManager_Control_RecvSignalCount >= PROGRAMMANAGER_CONTROL_RECVSIGNAL_COUNT_MAX)
  {
    ProgramManager_Control_RecvSignalCount = (uint8_t)0U;
  }
}



#ifdef  __cplusplus
}
#endif
