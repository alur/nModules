/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CRC.h
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

unsigned __int32 crc32(const void* data, size_t length, unsigned __int32 previousCrc32 = 0);
unsigned __int64 crc64(const void* data, size_t length, unsigned __int64 previousCrc64 = 0);
