#pragma once

#include "ApiDefs.h"
#include "IPane.hpp"

#include "../nUtilities/d2d1.h"

/// <summary>
///
/// </summary>
class IPanePainter {
public:
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
  /// <param name="area"></param>
  virtual void APICALL Paint(ID2D1RenderTarget *renderTarget, D2D1_RECT_F *area, class IPane *pane)
    const = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="newColor"></param>
  /// <param name="renderTarget"></param>
  virtual bool APICALL UpdateDWMColor(DWORD newColor, ID2D1RenderTarget *renderTarget) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="parentPosition"></param>
  virtual void APICALL UpdatePosition(D2D1_RECT_F parentPosition) = 0;
};
