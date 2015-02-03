#include "Api.h"
#include "StatePainter.hpp"

#include "../nUtilities/Macros.h"

#include <assert.h>


EXPORT_CDECL(IStatePainter*) CreateStatePainter(const StatePainterInitData *initData) {
  return new StatePainter(initData);
}


StatePainter::StatePainter(const StatePainterInitData *initData)
    : mBrush(nullptr)
    , mTextBrush(nullptr)
    , mResourceRefCount(0)
{
  DWORD color = (DWORD)initData->settingsReader->GetInt64(L"Color", 0x55C0448F);
  mColor.a = (color >> 24) / 255.0f;
  mColor.r = (color >> 16 & 0xFF) / 255.0f;
  mColor.g = (color >> 8 & 0xFF) / 255.0f;
  mColor.b = (color & 0xFF) / 255.0f;
}


LPVOID StatePainter::AddPane(const IPane*) {
  PainterData *data = new PainterData();
  data->textLayout = nullptr;
  return data;
}


HRESULT StatePainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  if (mResourceRefCount++ == 0) {
    ID2D1SolidColorBrush *brush;
    renderTarget->CreateSolidColorBrush(mColor, &brush);
    mBrush = brush;
    renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0, 1.0, 1.0), &brush);
    mTextBrush = brush;
  }
  return S_OK;
}


void StatePainter::ActivateState(BYTE state, IPane *pane) {
}


void StatePainter::ClearState(BYTE state, IPane *pane) {
}


void StatePainter::Destroy() {
  delete this;
}


void StatePainter::DiscardDeviceResources() {
  if (--mResourceRefCount == 0) {
    SAFERELEASE(mBrush);
    SAFERELEASE(mTextBrush);
  }
  assert(mResourceRefCount >= 0);
}


bool StatePainter::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void StatePainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID data) const {
  PaintBackground(renderTarget, area, pane, data);
  PaintText(renderTarget, area, pane, data);
  pane->PaintChildren(renderTarget, area);
}


void StatePainter::PaintBackground(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID data) const {
  renderTarget->FillRectangle(area, mBrush);
}


void StatePainter::PaintText(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID data) const {
  LPCWSTR text = pane->GetRenderingText();
  if (text && *text != L'\0') {
    PainterData *paneData = (PainterData*)data;

    // TODO(Erik): Hack!
    IDWriteTextFormat *textFormat;

    IDWriteFactory *factory = GetDWriteFactory();

    HRESULT hr = factory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-US", &textFormat);

    renderTarget->DrawTextW(text, lstrlenW(text), textFormat, area, mTextBrush);
    textFormat->Release();
    /*if (paneData->textLayout) {
    paneData->textLayout->Draw(renderTarget, mTextRenderer, 0, 0);
    }*/
  }
}


void StatePainter::PositionChanged(const IPane*, LPVOID, D2D1_RECT_F, bool, bool) {
}


void StatePainter::RemovePane(const IPane*, LPVOID data) {
  delete (PainterData*)data;
}


void StatePainter::TextChanged(const IPane*, LPVOID data, LPCWSTR text) {
}
