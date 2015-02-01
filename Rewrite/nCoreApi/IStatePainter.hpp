#pragma once

#include "IPanePainter.hpp"
#include "ISettingsReader.hpp"

#include <stdint.h>

/// <summary>
/// The de-facto painter for the core nModules.
/// </summary>
class IStatePainter : public IPanePainter {
public:
  /// <summary>
  /// Destroys the pane.
  /// </summary>
  virtual void APICALL Destroy() = 0;
};

/// <summary>
///
/// </summary>
struct StatePainterInitData {
  size_t cbSize;
  ISettingsReader *settingsReader;
  struct State {
    LPCWSTR name;
    BYTE base;
  } *states;
  BYTE numStates;
};
