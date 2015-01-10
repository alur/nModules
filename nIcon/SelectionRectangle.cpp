//-------------------------------------------------------------------------------------------------
// /nIcon/SelectionRectangle.cpp
// The nModules Project
//
// Draws the selection rectangle, as a post-processing step.
//-------------------------------------------------------------------------------------------------
#include "SelectionRectangle.hpp"


SelectionRectangle::SelectionRectangle() {
  mHidden = true;
}


SelectionRectangle::~SelectionRectangle() {}


void SelectionRectangle::Paint(ID2D1RenderTarget *renderTarget) {
  if (mHidden) {
    return;
  }
  if (mBackBrush.IsImageEdgeBrush()) {
    for (Brush::EdgeType type = Brush::EdgeType(0); type != Brush::EdgeType::Count;
        type = Brush::EdgeType(std::underlying_type<Brush::EdgeType>::type(type) + 1)) {
      renderTarget->FillRectangle(
        mBackBrush.GetImageEdgeRectAndScaleBrush(type, &mBackBrushWindowData), mBackBrush.brush);
    }
  } else {
    renderTarget->FillRoundedRectangle(mRect, mBackBrush.brush);
  }
  renderTarget->DrawRoundedRectangle(mRect, mOutlineBrush.brush, mOutlineWidth);
}


void SelectionRectangle::DiscardDeviceResources() {
  mBackBrush.Discard();
  mOutlineBrush.Discard();
}


HRESULT SelectionRectangle::ReCreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  mBackBrush.ReCreate(renderTarget);
  mOutlineBrush.ReCreate(renderTarget);

  return S_OK;
}


/// <summary>
/// IPaintable::UpdatePosition
/// Called when the parent has moved.
/// </summary>
void SelectionRectangle::UpdatePosition(D2D1_RECT_F /* parentPosition */) {
  // This is handled in SetRect instead.
}


/// <summary>
/// IPaintable::UpdateDWMColor
/// Called when the DWM color has changed. Returns true if this Paintable is currently using the DWM color.
/// </summary>
bool SelectionRectangle::UpdateDWMColor(ARGB newColor, ID2D1RenderTarget* renderTarget) {
  return mBackBrush.UpdateDWMColor(newColor, renderTarget);
}


void SelectionRectangle::Init(Settings *parentSettings) {
  Settings *settings = parentSettings->CreateChild(L"SelectionRectangle");
  Settings *outlineSettings = settings->CreateChild(L"Outline");
  mRect.radiusX = settings->GetFloat(L"CornerRadiusX", 0.0f);
  mRect.radiusY = settings->GetFloat(L"CornerRadiusY", 0.0f);
  mOutlineWidth = outlineSettings->GetFloat(L"Width", 0.5f);

  BrushSettings backDefaults;
  backDefaults.color = Color::Create(0x887777EE);
  mBackBrushSettings.Load(settings, &backDefaults);
  mBackBrush.Load(&mBackBrushSettings);

  BrushSettings outlineDefaults;
  outlineDefaults.color = Color::Create(0xDDFFFFFF);
  mOutlineBrushSettings.Load(outlineSettings, &outlineDefaults);
  mOutlineBrush.Load(&mOutlineBrushSettings);

  delete settings;
  delete outlineSettings;
}


void SelectionRectangle::SetRect(D2D1_RECT_F rect) {
  mBackBrush.UpdatePosition(rect, &mBackBrushWindowData);
  mRect.rect = mBackBrushWindowData.position;
  mOutlineBrush.UpdatePosition(mRect.rect, &mOutlineBrushWindowData);
}


const D2D1_RECT_F &SelectionRectangle::GetRect() const {
  return mRect.rect;
}


void SelectionRectangle::Show() {
  mHidden = false;
}


void SelectionRectangle::Hide() {
  mHidden = true;
}
