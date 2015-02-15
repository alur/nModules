#include "Api.h"
#include "BackgroundPainter.hpp"

#include <assert.h>


EXPORT_CDECL(IDiscardablePainter*) CreateBackgroundPainter(
    const ISettingsReader *reader, const StateDefinition *states, BYTE numStates) {
  return new BackgroundPainter(reader, states, numStates);
}


BackgroundPainter::BackgroundPainter(const ISettingsReader *reader, const StateDefinition*,
  BYTE numStates)
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

  mTextPadding.left = initData->settingsReader->GetLength(L"TextOffsetLeft", NLENGTH(0, 0, 0));
  mTextPadding.top = initData->settingsReader->GetLength(L"TextOffsetTop", NLENGTH(0, 0, 0));
  mTextPadding.right = initData->settingsReader->GetLength(L"TextOffsetRight", NLENGTH(0, 0, 0));
  mTextPadding.bottom = initData->settingsReader->GetLength(L"TextOffsetBottom", NLENGTH(0, 0, 0));
  */

  DWORD color = (DWORD)reader->GetInt64(L"Color", 0x55C0448F);
  mColor.a = (color >> 24) / 255.0f;
  mColor.r = (color >> 16 & 0xFF) / 255.0f;
  mColor.g = (color >> 8 & 0xFF) / 255.0f;
  mColor.b = (color & 0xFF) / 255.0f;
}


BackgroundPainter::~BackgroundPainter() {
  /*std::allocator<State> stateAllocator;
  for (int i = 0; i < mStateCount; ++i) {
    stateAllocator.destroy(&mStates[i]);
  }
  stateAllocator.deallocate(mStates, mStateCount);*/
}


LPVOID BackgroundPainter::AddPane(const IPane *pane) {
  return nullptr;
}


HRESULT BackgroundPainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  if (mResourceRefCount++ == 0) {
    ID2D1SolidColorBrush *brush;
    renderTarget->CreateSolidColorBrush(mColor, &brush);
    mBrush = brush;
  }
  return S_OK;
}


void BackgroundPainter::Discard() {
  delete this;
}


void BackgroundPainter::DiscardDeviceResources() {
  if (--mResourceRefCount == 0) {
    SAFERELEASE(mBrush);
  }
  assert(mResourceRefCount >= 0);
}


bool BackgroundPainter::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void BackgroundPainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID data, UINT state) const {
  renderTarget->FillRectangle(area, mBrush);
}


void BackgroundPainter::PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT, UINT newState, float) const {
  Paint(renderTarget, area, pane, painterData, newState);
}


void BackgroundPainter::PositionChanged(const IPane*, LPVOID, const D2D1_RECT_F&,
    bool, bool) {
}


void BackgroundPainter::RemovePane(const IPane*, LPVOID data) {}


void BackgroundPainter::TextChanged(const IPane*, LPVOID data, LPCWSTR text) {}
