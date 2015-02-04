#pragma once

#include "ApiDefs.h"
#include "IMessageHandler.hpp"
#include "IPanePainter.hpp"
#include "ISettingsReader.hpp"

#include "../nUtilities/NRect.hpp"
#include "../nUtilities/Windows.h"

/// <summary>
///
/// </summary>
struct PaneInitData {
  size_t cbSize;
  LPCWSTR name;
  DWORD64 flags;
  IMessageHandler *messageHandler;
  class IPanePainter *painter;
  // The settings reader to use. This may be null, if you would like the pane not to read
  // configuration settings.
  const ISettingsReader *settingsReader;

  enum Flag : DWORD64 {
    // Set for the desktop window.
    DesktopWindow = 1,
  };
};

/// <summary>
/// A part of a window.
/// </summary>
class IPane : public IMessageHandler {
public:
  /// <summary>
  /// Creates a child of this pane.
  /// </summary>
  virtual IPane *APICALL CreateChild(const PaneInitData*) = 0;

  /// <summary>
  /// Destroys the pane.
  /// </summary>
  virtual void APICALL Destroy() = 0;

  /// <summary>
  /// Returns the current value of the given length.
  /// </summary>
  /// <param name="horizontal">
  /// Set this to true if this is a horizontal length, false for vertical.
  /// </param>
  virtual float APICALL EvaluateLength(const NLENGTH &length, bool horizontal) const = 0;

  /// <summary>
  /// Returns the painter data for this pane.
  /// </summary>
  virtual LPVOID APICALL GetPainterData() const = 0;

  /// <summary>
  /// Retrieves the rendering position of the pane. This is the absolute position, in pixels, of
  /// the pane relative to its parent window.
  /// </summary>
  virtual const D2D1_RECT_F *APICALL GetRenderingPosition() const = 0;

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
  /// Paints any child panes.
  /// </summary>
  /// <param name="area">The area to repaint.</param>
  virtual void APICALL PaintChildren(ID2D1RenderTarget*, const D2D1_RECT_F *area) const = 0;

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
  /// Reverts a Lock() call. Updates the window contents if neccesary.
  /// </summary>
  virtual void APICALL Unlock() = 0;
};
