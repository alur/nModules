//-------------------------------------------------------------------------------------------------
// /Utilities/Hashing.h
// The nModules Project
//
// Hashing functions.
//-------------------------------------------------------------------------------------------------
#pragma once

#include <stdint.h>

namespace Hashing {
  uint32_t Crc32(const void *data, size_t length, uint32_t previous = 0);
  uint64_t Crc64(const void *data, size_t length, uint64_t previous = 0);
}
