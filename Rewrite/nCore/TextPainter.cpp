#include "Api.h"
#include "TextPainter.hpp"

#include <assert.h>
#include <dwrite.h>


EXPORT_CDECL(IDiscardablePainter*) CreateTextPainter(
    const ISettingsReader *reader, const StateDefinition *states, BYTE numStates) {
  return new TextPainter(reader, states, numStates);
}

TextPainter::TextPainter(const ISettingsReader *reader, const StateDefinition*, BYTE numStates)
    : mBrush(nullptr)
    , mStateCount(numStates + 1)
    , mResourceRefCount(0) {
  /*std::allocator<State> stateAllocator;
  mStates = stateAllocator.allocate(mStateCount);
  stateAllocator.construct(&mStates[0], initData->settingsReader, nullptr);
  for (int i = 1; i < mStateCount; ++i) {
  StatePainterInitData::State &state = initData->states[i - 1];
  assert(state.base < i);
  ISettingsReader *reader = initData->settingsReader->CreateChild(state.name);
  stateAllocator.construct(&mStates[i], initData->settingsReader, &mStates[state.base]);
  reader->Discard();
  }
  */

  mTextPadding.left = reader->GetLength(L"TextOffsetLeft", NLENGTH(0, 0, 0));
  mTextPadding.top = reader->GetLength(L"TextOffsetTop", NLENGTH(0, 0, 0));
  mTextPadding.right = reader->GetLength(L"TextOffsetRight", NLENGTH(0, 0, 0));
  mTextPadding.bottom = reader->GetLength(L"TextOffsetBottom", NLENGTH(0, 0, 0));
}


TextPainter::~TextPainter() {
  /*std::allocator<State> stateAllocator;
  for (int i = 0; i < mStateCount; ++i) {
  stateAllocator.destroy(&mStates[i]);
  }
  stateAllocator.deallocate(mStates, mStateCount);*/
}


LPVOID TextPainter::AddPane(const IPane *pane) {
  PerPaneData *data = new PerPaneData();
  PositionChanged(pane, data, pane->GetRenderingPosition(), true, true);
  return data;
}


HRESULT TextPainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  if (mResourceRefCount++ == 0) {
    ID2D1SolidColorBrush *brush;
    renderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0, 1.0, 1.0, 1.0), &brush);
    mBrush = brush;
  }
  return S_OK;
}


void TextPainter::Discard() {
  delete this;
}


void TextPainter::DiscardDeviceResources() {
  if (--mResourceRefCount == 0) {
    SAFERELEASE(mBrush);
  }
  assert(mResourceRefCount >= 0);
}


bool TextPainter::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void TextPainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID data, UINT state) const {
  LPCWSTR text = pane->GetRenderingText();
  if (text && *text != L'\0') {
    PerPaneData *paneData = (PerPaneData*)data;

    // TODO(Erik): Hack!
    IDWriteTextFormat *textFormat;

    IDWriteFactory *factory = GetDWriteFactory();

    HRESULT hr = factory->CreateTextFormat(L"Arial", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 24.0f, L"en-US", &textFormat);

    renderTarget->DrawTextW(text, lstrlenW(text), textFormat, paneData->textPosition, mBrush);
    textFormat->Release();
    /*if (paneData->textLayout) {
    paneData->textLayout->Draw(renderTarget, mTextRenderer, 0, 0);
    }*/
  }
}


void TextPainter::PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT, UINT newState, float) const {
  Paint(renderTarget, area, pane, painterData, newState);
}


void TextPainter::PositionChanged(const IPane *pane, LPVOID data, const D2D1_RECT_F &position,
    bool, bool) {
  PerPaneData *paneData = (PerPaneData*)data;
  paneData->textPosition = position;
  paneData->textPosition.left += pane->EvaluateLength(mTextPadding.left, true);
  paneData->textPosition.top += pane->EvaluateLength(mTextPadding.top, false);
  paneData->textPosition.right -= pane->EvaluateLength(mTextPadding.right, true);
  paneData->textPosition.bottom -= pane->EvaluateLength(mTextPadding.bottom, false);
}


void TextPainter::RemovePane(const IPane*, LPVOID data) {
  delete (PerPaneData*)data;
}


void TextPainter::TextChanged(const IPane*, LPVOID data, LPCWSTR text) {}
