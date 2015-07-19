#pragma once

#include "ApiDefs.h"
#include "IDiscardable.hpp"
#include "Lengths.h"

#include "../Headers/Windows.h"

/// <summary>
/// Reads settings from the LiteStep RC files, using a prefix.
/// </summary>
class ISettingsReader : public IDiscardable {
public:
  /// <summary>
  /// Creates a child of this settings reader. If the parent reads settings in the order
  /// SecondaryTaskbar, MainTaskbar, General, the result of CreateChild(Button, ...) will read
  /// settings in the order SecondaryTaskbarButton, MainTaskbarButton, GeneralButton.
  /// </summary>
  /// <param name="name">The name of the child.</param>
  virtual ISettingsReader* APICALL CreateChild(LPCWSTR name) const = 0;

  /// <summary>
  /// Enumerates all command lines with the given key.
  /// </summary>
  virtual void APICALL EnumCommandLines(LPCWSTR key,
    void (APICALL *callback)(LPCWSTR line, LPARAM), LPARAM) const = 0;

  /// <summary>
  /// Enumerates all lines with the given key.
  /// </summary>
  virtual void APICALL EnumLines(LPCWSTR key, void (APICALL *callback)(LPCWSTR line, LPARAM),
    LPARAM) const = 0;

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

  virtual UINT APICALL GetMonitor(LPCWSTR key, const UINT defaultValue) const = 0;
  virtual bool APICALL GetMonitor(LPCWSTR key, const LPUINT value) const = 0;

  /// <summary>
  /// Reads a rectangle specified by X Y Width and Height from the RC files. If one or more keys
  /// aren't found the RC files, the values from defaultValue will be used.
  /// </summary>
  virtual NRECT APICALL GetXYWHRect(LPCWSTR key, const NRECT &defaultValue) const = 0;

  /// <summary>
  /// Reads a rectangle specified by X Y Width and Height from the RC files. If only someparts are
  /// missing, those parts will be set to 0.
  /// </summary>
  /// <returns>True if any of the specified keys was found in the RC files.</returns>
  virtual bool APICALL GetXYWHRect(LPCWSTR key, NRECT *value) const = 0;

  /// <summary>
  /// Reads a rectangle specified by Left, Top, Right, and Bottom from the RC files. If one or more
  /// keys aren't found the RC files, the values from defaultValue will be used.
  /// </summary>
  virtual NRECT APICALL GetLTRBRect(LPCWSTR key, const NRECT &defaultValue) const = 0;

  /// <summary>
  /// Reads a rectangle specified by Left, Top, Right, and Bottom from the RC files. If only some
  /// parts are missing, those parts will be set to 0.
  /// </summary>
  /// <returns>True if any of the specified keys was found in the RC files.</returns>
  virtual bool APICALL GetLTRBRect(LPCWSTR key, NRECT *value) const = 0;
};
