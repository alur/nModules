//-------------------------------------------------------------------------------------------------
// /Utilities/CRC64.cpp
// The nModules Project
//
// Function for computing the CRC64 of some data.
//-------------------------------------------------------------------------------------------------
#include "Hashing.h"


/// <summary>
/// Computes the CRC64 of the data.
/// </summary>
/// <param name="data">The data to compute the CRC for.</param>
/// <param name="length">The number of bytes of data.</param>
/// <param name="previous">
/// If the CRC is being calculated piecewise, the CRC of the previous pieces.
/// </param>
uint64_t Hashing::Crc64(const void *data, size_t length, uint64_t crc) {
  crc = ~crc;

  if (length > 4) {
  }

  return ~crc;
}
