//-------------------------------------------------------------------------------------------------
// /nShared/LiteStep.h
// The nModules Project
//
// Confines LSAPI routines to a namespace, and provides extra parsing functions.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "Color.h"
#include "Distance.hpp"

#include "../Utilities/Common.h"

#include <functional>
#include <ShlObj.h>

namespace LiteStep {
  // Wrap the core API in the LiteStep namespace
  #include "../headers/lsapi.h"

  // Fetching of prefixed data types
  bool GetPrefixedRCBool(LPCTSTR prefix, LPCTSTR keyName, bool defaultValue);
  IColorVal* GetPrefixedRCColor(LPCTSTR prefix, LPCTSTR keyName, const IColorVal* defaultValue);
  double GetPrefixedRCDouble(LPCTSTR prefix, LPCTSTR keyName, double defaultValue);
  float GetPrefixedRCFloat(LPCTSTR prefix, LPCTSTR keyName, float defaultValue);
  int GetPrefixedRCInt(LPCTSTR prefix, LPCTSTR keyName, int defaultValue);
  __int64 GetPrefixedRCInt64(LPCTSTR prefix, LPCTSTR keyName, __int64 defaultValue);
  bool GetPrefixedRCLine(LPCTSTR prefix, LPCTSTR keyName, LPTSTR buffer, LPCTSTR defaultValue, size_t cchBuffer);
  UINT GetPrefixedRCMonitor(LPCTSTR prefix, LPCTSTR keyName, UINT defaultValue);
  Distance GetPrefixedRCDistance(LPCTSTR prefix, LPCTSTR keyName, Distance defaultValue);
  bool GetPrefixedRCString(LPCTSTR prefix, LPCTSTR keyName, LPTSTR buffer, LPCTSTR defaultValue, size_t cchBuffer);

  // Utility functions
  void IterateOverLines(LPCTSTR keyName, std::function<void (LPCTSTR line)> callback);
  void IterateOverTokens(LPCTSTR line, std::function<void (LPCTSTR token)> callback);
  void IterateOverLineTokens(LPCTSTR keyName, std::function<void (LPCTSTR token)> callback);
  void IterateOverCommandLineTokens(LPCTSTR prefix, LPCTSTR keyName, std::function<void (LPCTSTR token)> callback);

  // Parsing functions
  bool ParseBool(LPCTSTR boolString);
  IColorVal* ParseColor(LPCTSTR colorString, const IColorVal* defaultValue);
  UINT ParseMonitor(LPCTSTR monitorString, UINT defaultValue);
}
