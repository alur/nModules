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
    , mStateCount(initData->numStates + 1)
    , mResourceRefCount(0)
{
  std::allocator<State> stateAllocator;
  mStates = stateAllocator.allocate(mStateCount);
  stateAllocator.construct(&mStates[0], initData->settingsReader, nullptr);
  for (int i = 1; i < mStateCount; ++i) {
    StatePainterInitData::State &state = initData->states[i - 1];
    assert(state.base < i);
    ISettingsReader *reader;
    initData->settingsReader->CreateChild(state.name, &reader);
    stateAllocator.construct(&mStates[i], initData->settingsReader, &mStates[state.base]);
    reader->Destroy();
  }

  DWORD color = (DWORD)initData->settingsReader->GetInt64(L"Color", 0x55C0448F);
  mTextPadding.left = initData->settingsReader->GetLength(L"TextOffsetLeft", NLENGTH(0, 0, 0));
  mTextPadding.top = initData->settingsReader->GetLength(L"TextOffsetTop", NLENGTH(0, 0, 0));
  mTextPadding.right = initData->settingsReader->GetLength(L"TextOffsetRight", NLENGTH(0, 0, 0));
  mTextPadding.bottom = initData->settingsReader->GetLength(L"TextOffsetBottom", NLENGTH(0, 0, 0));

  mColor.a = (color >> 24) / 255.0f;
  mColor.r = (color >> 16 & 0xFF) / 255.0f;
  mColor.g = (color >> 8 & 0xFF) / 255.0f;
  mColor.b = (color & 0xFF) / 255.0f;
}


StatePainter::~StatePainter() {
  std::allocator<State> stateAllocator;
  for (int i = 0; i < mStateCount; ++i) {
    stateAllocator.destroy(&mStates[i]);
  }
  stateAllocator.deallocate(mStates, mStateCount);
}


LPVOID StatePainter::AddPane(const IPane *pane) {
  PainterData *data = new PainterData();
  data->textLayout = nullptr;
  PositionChanged(pane, data, pane->GetRenderingPosition(), true, true);
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

    renderTarget->DrawTextW(text, lstrlenW(text), textFormat, paneData->textPosition, mTextBrush);
    textFormat->Release();
    /*if (paneData->textLayout) {
    paneData->textLayout->Draw(renderTarget, mTextRenderer, 0, 0);
    }*/
  }
}


void StatePainter::PositionChanged(const IPane *pane, LPVOID data, const D2D1_RECT_F &position,
    bool, bool) {
  PainterData *paneData = (PainterData*)data;
  paneData->textPosition = position;
  paneData->textPosition.left += pane->EvaluateLength(mTextPadding.left, true);
  paneData->textPosition.top += pane->EvaluateLength(mTextPadding.top, false);
  paneData->textPosition.right -= pane->EvaluateLength(mTextPadding.right, true);
  paneData->textPosition.bottom -= pane->EvaluateLength(mTextPadding.bottom, false);
}


void StatePainter::RemovePane(const IPane*, LPVOID data) {
  delete (PainterData*)data;
}


void StatePainter::TextChanged(const IPane*, LPVOID data, LPCWSTR text) {
}
