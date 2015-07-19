#pragma once

#include "ApiDefs.h"

#include "../Headers/Windows.h"

#include <stdint.h>

enum class DataType : uint8_t {
  Error,
  Float,
  Integer,
  String,
  Boolean
};

class IDataProvider {
public:
  /// <summary>
  /// Get the boolean value, called for types where
  /// </summary>
  virtual HRESULT APICALL Get(bool *dest, uint8_t argc, DataType *argt, void *argv) = 0;

  /// <summary>
  /// Called for Float types.
  /// </summary>
  virtual HRESULT APICALL Get(float *dest, uint8_t argc, DataType *argt, void *argv) = 0;

  /// <summary>
  /// Called for Float types.
  /// </summary>
  virtual HRESULT APICALL Get(int *dest, uint8_t argc, DataType *argt, void *argv) = 0;

  /// <summary>
  /// Called for String types.
  /// </summary>
  virtual HRESULT APICALL Get(wchar_t *dest, size_t cchDest,
    uint8_t argc, DataType *argt, void *argv) = 0;

  /// <summary>
  /// Gets the return type of a call with the given parameters. If the argument types are not
  /// valid, return Error.
  /// </summary>
  virtual DataType APICALL GetReturnType(uint8_t argc, DataType *argt) const = 0;

  /// <summary>
  /// Checks that a given set of values are valid input for this data provider.
  /// </summary>
  virtual HRESULT APICALL ValidateValues(uint8_t argc, DataType *argt, void *argv) const = 0;
};
