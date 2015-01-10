//-------------------------------------------------------------------------------------------------
// /nIcon/SelectionRectangle.hpp
// The nModules Project
//
// Draws the selection rectangle, as a post-processing step.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../Utilities/Common.h"
#include "../nShared/IPainter.hpp"
#include "../nShared/Brush.hpp"

class SelectionRectangle : public IPainter {
  // Constructor/Destructor
public:
  SelectionRectangle();
  ~SelectionRectangle();

  // IPainter
public:
  void Paint(ID2D1RenderTarget *renderTarget) override;
  void DiscardDeviceResources() override;
  HRESULT ReCreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void UpdatePosition(D2D1_RECT_F parentPosition) override;
  bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) override;

  //
public:
  void Init(Settings *parentSettings);
  void SetRect(D2D1_RECT_F rect);
  const D2D1_RECT_F &GetRect() const;
  void Show();
  void Hide();

  //
private:
  Brush mBackBrush;
  BrushSettings mBackBrushSettings;
  Brush::WindowData mBackBrushWindowData;

  Brush mOutlineBrush;
  BrushSettings mOutlineBrushSettings;
  Brush::WindowData mOutlineBrushWindowData;

  float mOutlineWidth;
  D2D1_ROUNDED_RECT mRect;
  bool mHidden;
};
