/* 
 * File:   memIf.c
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

#include "memIf.h"

#include "at24c32.h"



/*===============================================================================================
*                                       DEFINES AND MACROS
===============================================================================================*/

/** External memory size in byte */
#define EXT_MEM_BYTE_NUM                                  (uint16_t)4096U

/** External memory byte number in sector */
#define EXT_MEM_BYTE_SECT                                 (uint16_t)32U

/** External memory magic number to manage if corrupted */
#define EXT_MEM_MAGIC_NUMBER_VAL                          (uint32_t)0x01020304

/** External memory magic number address */
#define EXT_MEM_MAGIC_NUMBER_ADDR                         (uint16_t)0x0000



/*===============================================================================================
*                                       GLOBAL VARIABLES
===============================================================================================*/

/** External memory management structure type */
MemIfStruct extMemIf;



/*===============================================================================================
*                                   LOCAL FUNCTION PROTOTYPES
===============================================================================================*/

void extMem_WriteByte(uint16_t address, uint8_t data);
uint8_t extMem_ReadByte(uint16_t address);
void extMem_WriteByteArray(uint16_t address, uint8_t* data, uint8_t length);
void extMem_ReadByteArray(uint16_t address, uint8_t* data, uint8_t length);
void extMem_WriteInteger(uint16_t address, uint16_t data);
uint16_t extMem_ReadInteger(uint16_t address);
void extMem_WriteIntegerArray(uint16_t address, uint16_t* data, uint8_t length);
void extMem_ReadIntegerArray(uint16_t address, uint16_t* data, uint8_t length);
void extMem_WriteWord(uint16_t address, uint32_t data);
uint32_t extMem_ReadWord(uint16_t address);



/*===============================================================================================
*                                       LOCAL FUNCTIONS
===============================================================================================*/

void extMem_WriteByte(uint16_t address, uint8_t data)
{
  AT24C32_WriteByte(address, &data, (uint8_t)1U);
}

uint8_t extMem_ReadByte(uint16_t address)
{
  uint8_t data;

  AT24C32_ReadByte(address, &data, (uint8_t)1U);

  return data;
}



/*=============================================================================================*/
void extMem_WriteByteArray(uint16_t address, uint8_t* data, uint8_t length)
{
  AT24C32_WriteByte(address, data, length);
}

void extMem_ReadByteArray(uint16_t address, uint8_t* data, uint8_t length)
{
  AT24C32_ReadByte(address, data, length);
}



/*=============================================================================================*/
void extMem_WriteInteger(uint16_t address, uint16_t data)
{
  AT24C32_WriteInteger(address, &data, (uint8_t)1U);
}

uint16_t extMem_ReadInteger(uint16_t address)
{
  uint16_t data;

  AT24C32_ReadInteger(address, &data, (uint8_t)1U);

  return data;
}



/*=============================================================================================*/
void extMem_WriteIntegerArray(uint16_t address, uint16_t* data, uint8_t length)
{
  AT24C32_WriteInteger(address, data, length);
}

void extMem_ReadIntegerArray(uint16_t address, uint16_t* data, uint8_t length)
{
  AT24C32_ReadInteger(address, data, length);
}



/*=============================================================================================*/
void extMem_WriteWord(uint16_t address, uint32_t data)
{
  AT24C32_WriteWord(address, &data, (uint8_t)1U);
}

uint32_t extMem_ReadWord(uint16_t address)
{
  uint32_t data;

  AT24C32_ReadWord(address, &data, (uint8_t)1U);

  return data;
}



/*===============================================================================================
*                                       GLOBAL FUNCTIONS
===============================================================================================*/

HAL_StatusTypeDef MemIf_Init(void)
{
  uint16_t index;
  uint32_t magicNum;
  
  /* Store pointer functions */
  extMemIf.writeByte         = &extMem_WriteByte;
  extMemIf.readByte          = &extMem_ReadByte;
  extMemIf.writeByteArray    = &extMem_WriteByteArray;
  extMemIf.readByteArray     = &extMem_ReadByteArray;
  extMemIf.writeInteger      = &extMem_WriteInteger;
  extMemIf.readInteger       = &extMem_ReadInteger;
  extMemIf.writeIntegerArray = &extMem_WriteIntegerArray;
  extMemIf.readIntegerArray  = &extMem_ReadIntegerArray;
  extMemIf.writeWord         = &extMem_WriteWord;
  extMemIf.readWord          = &extMem_ReadWord;

  /* Read magic number */
  magicNum = extMemIf.readWord(EXT_MEM_MAGIC_NUMBER_ADDR);
  
  /* If magic number is not matched, memory is corrupted, need mass erase */
  if (magicNum != EXT_MEM_MAGIC_NUMBER_VAL)
  {
    uint8_t zeroArr[EXT_MEM_BYTE_SECT];
    uint8_t readArr[EXT_MEM_BYTE_SECT];

    memset(zeroArr, (uint8_t)0U, EXT_MEM_BYTE_SECT);

    /* Erase memory to 0 */
    for (index = (uint16_t)0U; index < (EXT_MEM_BYTE_NUM / EXT_MEM_BYTE_SECT); index++)
    {
      extMemIf.writeByteArray((index * EXT_MEM_BYTE_SECT), zeroArr, EXT_MEM_BYTE_SECT);
      extMemIf.readByteArray((index * EXT_MEM_BYTE_SECT), readArr, EXT_MEM_BYTE_SECT);

      if (memcmp(readArr, zeroArr, EXT_MEM_BYTE_SECT) != (int8_t)0U)
      {
        return HAL_ERROR;
      }
    }

    /* Write magic number */
    extMemIf.writeWord(EXT_MEM_MAGIC_NUMBER_ADDR, EXT_MEM_MAGIC_NUMBER_VAL);

    /* Read magic number again to make sure it's correct */
    magicNum = extMemIf.readWord(EXT_MEM_MAGIC_NUMBER_ADDR);

    if (magicNum != EXT_MEM_MAGIC_NUMBER_VAL)
    {
      return HAL_ERROR;
    }
  }

  return HAL_OK;
}



#ifdef  __cplusplus
}
#endif
