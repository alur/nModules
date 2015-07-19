#pragma once

#include "ApiDefs.h"
#include "IPane.hpp"

#include "../Headers/d2d1.h"

/// <summary>
/// Paints panes.
/// </summary>
class IPainter {
public:
  /// <summary>
  /// Called to initialize the painter for the given pane.
  /// </summary>
  /// <returns>A pointer which will be returned to the painter when this pane is painted.</returns>
  virtual LPVOID APICALL AddPane(const class IPane *pane) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="renderTarget"></param>
  virtual HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) = 0;


  /// <summary>
  /// When this is called, the ID2D1 render target is no longer valid, and the painter should
  /// discard all devices resources.
  /// </summary>
  virtual void APICALL DiscardDeviceResources() = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="renderTarget"></param>
  /// <returns>True if ...</returns>
  virtual bool APICALL DynamicColorChanged(ID2D1RenderTarget *renderTarget) = 0;

  /// <summary>
  /// Paints the given area.
  /// </summary>
  /// <param name="renderTarget">The pane's render target.</param>
  /// <param name="area">The area to paint in.</param>
  /// <param name="pane">The pane being painted.</param>
  /// <param name="painterData">The pointer returned by AddPane.</param>
  /// <param name="state">The current state of the pane.</param>
  virtual void APICALL Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT state) const = 0;

  /// <summary>
  /// Paints the given area during a state transformation.
  /// </summary>
  /// <param name="renderTarget">The pane's render target.</param>
  /// <param name="area">The area to paint in.</param>
  /// <param name="pane">The pane being painted.</param>
  /// <param name="painterData">The pointer returned by AddPane.</param>
  /// <param name="oldState">The state we're transitioning from.</param>
  /// <param name="newState">The state we're transitioning to.</param>
  /// <param name="time">Transition progress, [0, 1].</param>
  virtual void APICALL PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT oldState, UINT newState,
    float time) const = 0;

  /// <summary>
  /// Called when the rendering position of the pane has changed.
  /// </summary>
  /// <param name="position">The new position of the pane.</param>
  /// <param name="isMove">True if the top left corner of the pane has moved.</param>
  /// <param name="isSize">True if this size of the pane has changed.</param>
  virtual void APICALL PositionChanged(const class IPane *pane, LPVOID painterData,
    const D2D1_RECT_F &position, bool isMove, bool isSize) = 0;

  /// <summary>
  /// Called when the pane is no longer going to use this painter. The painter should free up any
  /// resources allocated for that particular pane.
  /// </summary>
  virtual void APICALL RemovePane(const class IPane *pane, LPVOID painterData) = 0;

  /// <summary>
  /// Called when the text of the given pane has changed.
  /// </summary>
  virtual void APICALL TextChanged(const class IPane *pane, LPVOID painterData, LPCWSTR text) = 0;
};
