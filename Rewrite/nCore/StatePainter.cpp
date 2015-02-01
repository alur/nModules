#include "StatePainter.hpp"

#include "../nUtilities/Macros.h"

#include <assert.h>


EXPORT_CDECL(IStatePainter*) CreateStatePainter(const StatePainterInitData *initData) {
  return new StatePainter(initData);
}


StatePainter::StatePainter(const StatePainterInitData *initData)
    : mBrush(nullptr)
    , mResourceRefCount(0)
{
  DWORD color = (DWORD)initData->settingsReader->GetInt64(L"Color", 0x55C0448F);
  mColor.a = (color >> 24) / 255.0f;
  mColor.r = (color >> 16 & 0xFF) / 255.0f;
  mColor.g = (color >> 8 & 0xFF) / 255.0f;
  mColor.b = (color & 0xFF) / 255.0f;
}


LPVOID StatePainter::AddPane(const IPane*) {
  return nullptr;
}


HRESULT StatePainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  if (mResourceRefCount++ == 0) {
    ID2D1SolidColorBrush *brush;
    renderTarget->CreateSolidColorBrush(mColor, &brush);
    mBrush = brush;
  }
  return S_OK;
}


void StatePainter::Destroy() {
  delete this;
}


void StatePainter::DiscardDeviceResources() {
  if (--mResourceRefCount == 0) {
    SAFERELEASE(mBrush);
  }
  assert(mResourceRefCount >= 0);
}


bool StatePainter::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void StatePainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID painterData) const {
  renderTarget->FillRectangle(area, mBrush);

  LPCWSTR text = pane->GetRenderingText();
  if (text && *text != L'\0') {
  }

  pane->PaintChildren(renderTarget, area);
}


void StatePainter::PositionChanged(const IPane *pane, LPVOID painterData, D2D1_RECT_F position) {
}


void StatePainter::RemovePane(const IPane*, LPVOID) {
}


void StatePainter::TextChanged(const IPane*, LPVOID, LPCWSTR) {
}
