/* 
 * File:   memIf.h
 * Author: Long
 *
 * Created on September 15, 2019, 11:06 AM
 */

#ifndef MEMIF_H
#define MEMIF_H

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



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/




/*===============================================================================================
*                                         STRUCTURES
===============================================================================================*/

/**
 * @brief      Memory interface structure type
 *
 */
typedef struct _MemIfStruct
{
  void (*writeByte)(uint16_t address, uint8_t data);
  uint8_t (*readByte)(uint16_t address);
  void (*writeByteArray)(uint16_t address, uint8_t* data, uint8_t length);
  void (*readByteArray)(uint16_t address, uint8_t* data, uint8_t length);
  void (*writeInteger)(uint16_t address, uint16_t data);
  uint16_t (*readInteger)(uint16_t address);
  void (*writeIntegerArray)(uint16_t address, uint16_t* data, uint8_t length);
  void (*readIntegerArray)(uint16_t address, uint16_t* data, uint8_t length);
  void (*writeWord)(uint16_t address, uint32_t data);
  uint32_t (*readWord)(uint16_t address);
} MemIfStruct;



/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

extern MemIfStruct extMemIf;



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

HAL_StatusTypeDef MemIf_Init(void);



#ifdef  __cplusplus
}
#endif

#endif  /* MEMIF_H */
