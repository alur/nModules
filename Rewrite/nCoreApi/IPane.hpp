#pragma once

#include "ApiDefs.h"
#include "IDiscardable.hpp"
#include "IMessageHandler.hpp"
#include "IPainter.hpp"
#include "ISettingsReader.hpp"
#include "Lengths.h"

#include "../Headers/d2d1.h"

struct StateDefinition {
  LPCWSTR name;
  // Bitfield of depedencies. A state with dependencies set is automatically activated when all
  // of its dependencies are active, and cleared when they are not.
  ULONGLONG dependencies;
  // The base for this state. If a setting isn't specified in this state, the one from the base
  // state will be used instead.
  BYTE base;
};

struct PaneInitData {
  size_t cbSize;
  LPCWSTR name;
  DWORD64 flags;
  IMessageHandler *messageHandler;
  class IPainter **painters;
  size_t numPainters;
  // The settings reader to use. This may be null, if you would like the pane not to read
  // configuration settings.
  const ISettingsReader *settingsReader;
  BYTE numStates;
  const StateDefinition *states;

  enum Flag : DWORD64 {
    // Set for the desktop window.
    DesktopWindow = 1,
    // True if we should try to parse the text.
    DynamicText = 2
  };
};

/// <summary>
/// A part of a window.
/// </summary>
class IPane : public IMessageHandler, public IDiscardable {
public:
  /// <summary>
  /// Activates the given state.
  /// </summary>
  virtual void APICALL ActivateState(BYTE state) = 0;

  /// <summary>
  /// Clears the given state.
  /// </summary>
  virtual void APICALL ClearState(BYTE state) = 0;

  /// <summary>
  /// Creates a child of this pane.
  /// </summary>
  virtual IPane *APICALL CreateChild(const PaneInitData*) = 0;

  /// <summary>
  /// Returns the current value of the given length.
  /// </summary>
  /// <param name="horizontal">
  /// Set this to true if this is a horizontal length, false for vertical.
  /// </param>
  virtual float APICALL EvaluateLength(const NLENGTH &length, bool horizontal) const = 0;

  /// <summary>
  /// Returns the current value of the given rectangle.
  /// </summary>
  virtual D2D1_RECT_F APICALL EvaluateRect(const NRECT &rect) const = 0;

  /// <summary>
  /// Returns the painter data for this pane. Do not use this when painting, as it needs to search
  /// for the proper painter.
  /// </summary>
  virtual LPVOID APICALL GetPainterData(const IPainter*) const = 0;

  /// <summary>
  /// Retrieves the rendering position of the pane. This is the absolute position, in pixels, of
  /// the pane relative to its parent window.
  /// </summary>
  virtual const D2D1_RECT_F &APICALL GetRenderingPosition() const = 0;

  /// <summary>
  /// Retrieves the size in pixels of the pane.
  /// </summary>
  virtual const D2D1_SIZE_F &APICALL GetRenderingSize() const = 0;

  /// <summary>
  /// Retrieves the text that should be rendered for this pane. Note that this may not be the same
  /// as what was passed to SetText. Might be null.
  /// </summary>
  virtual LPCWSTR APICALL GetRenderingText() const = 0;

  /// <summary>
  /// Retrieves the position of the pane on the screen.
  /// </summary>
  /// <returns>False if the pane isn't on the screen.</returns>
  virtual bool APICALL GetScreenPosition(D2D1_RECT_F *rect) const = 0;

  /// <summary>
  /// Gets the current HWND of the window containing the pane. Note that this may change.
  /// </summary>
  virtual HWND APICALL GetWindow() const = 0;

  /// <summary>
  /// Hides this pane.
  /// </summary>
  virtual void APICALL Hide() = 0;

  /// <summary>
  /// Prevents the pane from updating its contents. Use this to repaint many windows changes with
  /// a single pass, rather than a pass per change. You must call Unlock() once for each call to
  /// Lock().
  /// </summary>
  virtual void APICALL Lock() = 0;

  /// <summary>
  /// Moves the top-left corner of the pane to the given position.
  /// </summary>
  virtual void APICALL Move(const NPOINT&) = 0;

  /// <summary>
  /// Repositions the pane.
  /// </summary>
  virtual void APICALL Position(LPCNRECT) = 0;

  /// <summary>
  /// Repaints a given area of the pane.
  /// </summary>
  virtual void APICALL Repaint(LPCNRECT) = 0;

  /// <summary>
  /// Sets the text of the pane.
  /// </summary>
  /// <param name="text">The new text.</param>
  virtual void APICALL SetText(LPCWSTR text) = 0;

  /// <summary>
  /// Shows this pane.
  /// </summary>
  virtual void APICALL Show() = 0;

  /// <summary>
  /// Toggles the given state.
  /// </summary>
  virtual void APICALL ToggleState(BYTE state) = 0;

  /// <summary>
  /// Reverts a Lock() call. Updates the window contents if neccesary.
  /// </summary>
  virtual void APICALL Unlock() = 0;
};
