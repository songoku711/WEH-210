/* 
 * File:   at24c32.h
 * Author: Huy Long
 * Comments:  File contains definitions, structures and interfaces to AT24C32.
 * Revision history: 1.0
 */

#ifndef AT24C32_H
#define AT24C32_H

#ifdef __cplusplus
extern "C" {
#endif



/*===============================================================================================
*                                         INCLUDE FILES
* 1) system and project includes
* 2) needed interfaces from external units
* 3) internal and external interfaces from this unit
===============================================================================================*/

#include "stm32f4xx_hal.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/**
 * @brief      AT24C32 I2C device address
 *
 */
#define AT24C32_SLAVE_ADDRESS                      (uint8_t)0xA0



/*===============================================================================================
*                                     FUNCTION PROTOTYPES
===============================================================================================*/

/**
 * @brief      AT24C32 initialization function
 * @details    AT24C32 initialization function
 * @param[in]  None
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Initialization successful
 *             HAL_ERROR - Initialization failed
 */
HAL_StatusTypeDef AT24C32_Init
(
  void
);



/*=============================================================================================*/
/**
 * @brief      AT24C32 write array of bytes function
 * @details    This function will write array of bytes consecutively
 *             into AT24C32
 * @param[in]  uint16_t   address      Address of first written byte
 *             uint8_t*   data         Pointer to array of bytes
 *             uint8_t    length       Number of byte data to be written
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Write successful
 *             HAL_ERROR - Write failed
 */
HAL_StatusTypeDef AT24C32_WriteByte
(
  uint16_t     address,
  uint8_t*     data,
  uint8_t      length
);



/*=============================================================================================*/
/**
 * @brief      AT24C32 read array of bytes function
 * @details    This function will read array of bytes consecutively
 *             from AT24C32
 * @param[in]  uint16_t   address      Address of first read byte
 *             uint8_t*   data         Pointer to storing array of bytes
 *             uint8_t    length       Number of byte data to be read
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Read successful
 *             HAL_ERROR - Read failed
 */
HAL_StatusTypeDef AT24C32_ReadByte
(
  uint16_t     address,
  uint8_t*     data,
  uint8_t      length
);



/*=============================================================================================*/
/**
 * @brief      AT24C32 write array of integers (half-word) function
 * @details    This function will write array of integers consecutively
 *             into AT24C32
 * @param[in]  uint16_t   address      Address of first written integer
 *             uint16_t*  data         Pointer to array of integers
 *             uint8_t    length       Number of integer data to be written
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Write successful
 *             HAL_ERROR - Write failed
 */
HAL_StatusTypeDef AT24C32_WriteInteger
(
  uint16_t address,
  uint16_t* data,
  uint8_t length
);



/*=============================================================================================*/
/**
 * @brief      AT24C32 read array of integers (half-word) function
 * @details    This function will read array of integers consecutively
 *             from AT24C32
 * @param[in]  uint16_t   address      Address of first read integer
 *             uint16_t*  data         Pointer to storing array of integers
 *             uint8_t    length       Number of integer data to be read
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Read successful
 *             HAL_ERROR - Read failed
 */
HAL_StatusTypeDef AT24C32_ReadInteger
(
  uint16_t address,
  uint16_t* data,
  uint8_t length
);



/*=============================================================================================*/
/**
 * @brief      AT24C32 write array of word function
 * @details    This function will write array of words consecutively
 *             into AT24C32
 * @param[in]  uint16_t   address      Address of first written word
 *             uint32_t*  data         Pointer to array of words
 *             uint8_t    length       Number of word data to be written
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Write successful
 *             HAL_ERROR - Write failed
 */
HAL_StatusTypeDef AT24C32_WriteWord
(
  uint16_t address,
  uint32_t* data,
  uint8_t length
);



/*=============================================================================================*/
/**
 * @brief      AT24C32 read array of word function
 * @details    This function will read array of words consecutively
 *             from AT24C32
 * @param[in]  uint16_t   address      Address of first read word
 *             uint32_t*  data         Pointer to storing array of words
 *             uint8_t    length       Number of word data to be read
 *
 * @return     HAL_StatusTypeDef
 *             HAL_OK - Read successful
 *             HAL_ERROR - Read failed
 */
HAL_StatusTypeDef AT24C32_ReadWord
(
  uint16_t address,
  uint32_t* data,
  uint8_t length
);



#ifdef __cplusplus
}
#endif

#endif /* AT24C32_H */
