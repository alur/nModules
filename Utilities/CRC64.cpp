/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CRC64.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "CRC.h"


unsigned __int64 crc64(const void* data, size_t length, unsigned __int64 crc)
{
    crc = ~crc;

    if (length > 4)
    {

    }

    return ~crc;
}
