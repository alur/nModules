#pragma once

#include "ApiDefs.h"

#include "../nUtilities/NLength.hpp"
#include "../nUtilities/Windows.h"

/// <summary>
/// Reads settings from the LiteStep RC files, using a prefix.
/// </summary>
class ISettingsReader {
public:
  /// <summary>
  /// Destroys the SettingsReader.
  /// </summary>
  virtual void APICALL Destroy() = 0;

  /// <summary>
  /// Creates a child of this settings reader. If the parent reads settings in the order
  /// SecondaryTaskbar, MainTaskbar, General, the result of CreateChild(Button, ...) will read
  /// settings in the order SecondaryTaskbarButton, MainTaskbarButton, GeneralButton.
  /// </summary>
  /// <param name="name">The name of the child.</param>
  /// <param name="pReader">Pointer which will receieve the settings reader.</param>
  virtual HRESULT APICALL CreateChild(LPCWSTR name, ISettingsReader **pReader) const = 0;

  /// <summary>
  /// Reads a boolean value from the RC files.
  /// </summary>
  /// <returns>The boolean value</returns>
  virtual bool APICALL GetBool(LPCWSTR key, bool defaultValue) const = 0;

  /// <summary>
  /// Reads a boolean value from the RC files.
  /// </summary>
  /// <returns>True if the value was found in the RC files.</returns>
  virtual bool APICALL GetBool(LPCWSTR key, bool *value) const = 0;

  virtual double APICALL GetDouble(LPCWSTR key, double defaultValue) const = 0;
  virtual bool APICALL GetDouble(LPCWSTR key, double *value) const = 0;

  virtual float APICALL GetFloat(LPCWSTR key, float defaultValue) const = 0;
  virtual bool APICALL GetFloat(LPCWSTR key, float *value) const = 0;

  virtual int APICALL GetInt(LPCWSTR key, int defaultValue) const = 0;
  virtual bool APICALL GetInt(LPCWSTR key, int *value) const = 0;

  virtual __int64 APICALL GetInt64(LPCWSTR key, __int64 defaultValue) const = 0;
  virtual bool APICALL GetInt64(LPCWSTR key, __int64 *value) const = 0;

  /// <summary>
  /// Reads a string from the RC files.
  /// </summary>
  /// <returns>True if the value was found in the RC files.</returns>
  virtual bool APICALL GetString(LPCWSTR key, LPWSTR value, size_t cchValue,
    LPWSTR defaultValue) const = 0;

  virtual NLENGTH APICALL GetLength(LPCWSTR key, const NLENGTH &defaultValue) const = 0;
  virtual bool APICALL GetLength(LPCWSTR key, NLENGTH *value) const = 0;
};
