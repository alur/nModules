//-------------------------------------------------------------------------------------------------
// /nClock/ClockHand.hpp
// The nModules Project
//
// A hand on a clock.
//-------------------------------------------------------------------------------------------------
#pragma once

#include "../nShared/Brush.hpp"
#include "../nShared/IPainter.hpp"

#include "../Utilities/Common.h"

class ClockHand : public IPainter {
public:
  ClockHand();

    // IPainter
public:
  void Paint(ID2D1RenderTarget *renderTarget) override;
  void DiscardDeviceResources() override;
  HRESULT ReCreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void UpdatePosition(D2D1_RECT_F parentPosition) override;
  bool UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) override;

public:
  void Initialize(Settings *clockSettings, LPCTSTR prefix, float maxValue);
  void SetValue(float value);

    // User settings
private:
  D2D1_RECT_F mHandRect;
  bool mSmoothMovement;

private:
  float mMaxValue;

    // Computed member variables
private:
  D2D1_SIZE_F mCenterPoint;
  float mRotation;

  Brush mBrush;
  BrushSettings mBrushSettings;
  Brush::WindowData mBrushWindowData;
};
