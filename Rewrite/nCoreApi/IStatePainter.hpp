#pragma once

#include "IPane.hpp"
#include "IPanePainter.hpp"
#include "ISettingsReader.hpp"

#include <stdint.h>

/// <summary>
/// The de-facto painter for the core nModules.
/// </summary>
class IStatePainter : public IPanePainter {
public:
  /// <summary>
  /// Activates a state for a pane.
  /// </summary>
  virtual void APICALL ActivateState(BYTE state, IPane *pane) = 0;

  /// <summary>
  /// Clears a state for a pane.
  /// </summary>
  virtual void APICALL ClearState(BYTE state, IPane *pane) = 0;

  /// <summary>
  /// Destroys the pane.
  /// </summary>
  virtual void APICALL Destroy() = 0;

  /// <summary>
  /// Paints the pane background.
  /// </summary>
  virtual void APICALL PaintBackground(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID painterData) const = 0;

  /// <summary>
  /// Paints the pane's text.
  /// </summary>
  virtual void APICALL PaintText(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID painterData) const = 0;
};

/// <summary>
///
/// </summary>
struct StatePainterInitData {
  size_t cbSize;
  ISettingsReader *settingsReader;
  BYTE numStates;

  // The states for this painter, in order of precedence.
  struct State {
    LPCWSTR name;
    // Bitfield of depedencies. A state with dependencies set is automatically activated when all
    // of its dependencies are active, and cleared when they are not.
    ULONGLONG dependencies;
    // The base for this state. If a setting isn't specified in this state, the one from the base
    // state will be used instead.
    BYTE base;
  } *states;
};
