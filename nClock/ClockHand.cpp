//-------------------------------------------------------------------------------------------------
// /nClock/ClockHand.cpp
// The nModules Project
//
// A hand on a clock.
//-------------------------------------------------------------------------------------------------
#include "ClockHand.hpp"

static const BrushSettings sBrushDefaults([] (BrushSettings &defaults) {
  defaults.color = Color::Create(0xFF77FFEE);
});


/// <summary>
/// Constructor
/// </summary>
ClockHand::ClockHand() {
  mCenterPoint = D2D1::SizeF(0, 0);
}


/// <summary>
/// IPaintable::Paint
/// Paints the hand to the render target.
/// </summary>
void ClockHand::Paint(ID2D1RenderTarget *renderTarget) {
  using D2D1::Matrix3x2F;

  D2D1_MATRIX_3X2_F currentTransform;
  renderTarget->GetTransform(&currentTransform);
  renderTarget->SetTransform(
    currentTransform * Matrix3x2F::Rotation(mRotation) * Matrix3x2F::Translation(mCenterPoint));

  mBrush.brush->SetTransform(mBrushWindowData.brushTransform);
  renderTarget->FillRectangle(mHandRect, mBrush.brush);

  renderTarget->SetTransform(currentTransform);
}


/// <summary>
/// IPaintable::DiscardDeviceResources
/// Releases all D2D resources.
/// </summary>
void ClockHand::DiscardDeviceResources() {
  mBrush.Discard();
}


/// <summary>
/// IPaintable::ReCreateDeviceResources
/// (Re)Creates all D2D resources.
/// </summary>
HRESULT ClockHand::ReCreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  HRESULT hr = mBrush.ReCreate(renderTarget);
  if (SUCCEEDED(hr)) {
    mBrush.UpdatePosition(mHandRect, &mBrushWindowData);
  }
  return hr;
}


/// <summary>
/// IPaintable::UpdatePosition
/// Called when the parent has moved.
/// </summary>
void ClockHand::UpdatePosition(D2D1_RECT_F parentPosition) {
  mCenterPoint = D2D1::SizeF(
      (parentPosition.left + parentPosition.right) / 2.0f,
      (parentPosition.top + parentPosition.bottom) / 2.0f);
}


/// <summary>
/// IPaintable::UpdateDWMColor
/// Called when the DWM color has changed. Returns true if this Paintable is currently using the
/// DWM color.
/// </summary>
bool ClockHand::UpdateDWMColor(ARGB newColor, ID2D1RenderTarget *renderTarget) {
  return mBrush.UpdateDWMColor(newColor, renderTarget);
}


/// <summary>
/// Initializes this clock hand.
/// </summary>
void ClockHand::Initialize(Settings *clockSettings, LPCTSTR prefix, float maxValue) {
  Settings *settings = clockSettings->CreateChild(prefix);

  mSmoothMovement = settings->GetBool(L"SmoothMovement", false);
  mMaxValue = maxValue;

  mBrushSettings.Load(settings, &sBrushDefaults);
  mBrush.Load(&mBrushSettings);

  float length = settings->GetFloat(L"Length", 50);
  float thickness = settings->GetFloat(L"Thickness", 3);
  float offset = settings->GetFloat(L"Offset", 0);

  mHandRect.left = offset;
  mHandRect.top = -thickness / 2.0f;
  mHandRect.bottom = mHandRect.top + thickness;
  mHandRect.right = mHandRect.left + length;

  delete settings;
}


/// <summary>
/// Sets the rotation of the clock hand.
/// </summary>
void ClockHand::SetValue(float value) {
  if (value > mMaxValue) {
    value -= mMaxValue;
  }

  if (!mSmoothMovement) {
    value = floorf(value);
  }

  mRotation = value * 360.0f / mMaxValue - 90.0f;
}
