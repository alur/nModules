#include "Api.h"
#include "BackgroundPainter.hpp"

#include <assert.h>


EXPORT_CDECL(IDiscardablePainter*) CreateBackgroundPainter(
    const ISettingsReader *reader, const StateDefinition *states, BYTE numStates) {
  return new BackgroundPainter(reader, states, numStates);
}


BackgroundPainter::BackgroundPainter(const ISettingsReader *reader, const StateDefinition *states,
  BYTE numStates)
    : mStateCount(numStates + 1)
    , mResourceRefCount(0) {
  std::allocator<BackgroundPainterState> stateAllocator;
  mStates = stateAllocator.allocate(mStateCount);
  stateAllocator.construct(&mStates[0], reader, nullptr);
  for (int i = 1; i < mStateCount; ++i) {
    const StateDefinition &state = states[i - 1];
    assert(state.base < i);
    ISettingsReader *stateReader = reader->CreateChild(state.name);
    stateAllocator.construct(&mStates[i], stateReader, &mStates[state.base]);
    stateReader->Discard();
  }
}


BackgroundPainter::~BackgroundPainter() {
  std::allocator<BackgroundPainterState> stateAllocator;
  for (int i = 0; i < mStateCount; ++i) {
    stateAllocator.destroy(&mStates[i]);
  }
  stateAllocator.deallocate(mStates, mStateCount);
}


LPVOID BackgroundPainter::AddPane(const IPane *pane) {
  PerPaneData *data = new PerPaneData();
  data->stateData.resize(mStateCount);
  PositionChanged(pane, data, pane->GetRenderingPosition(), true, true);
  return data;
}


HRESULT BackgroundPainter::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  HRESULT hr = S_OK;
  if (mResourceRefCount++ == 0) {
    for (int i = 0; i < mStateCount && SUCCEEDED(hr); ++i) {
      hr = mStates[i].CreateDeviceResources(renderTarget);
    }
  }
  return hr;
}


void BackgroundPainter::Discard() {
  delete this;
}


void BackgroundPainter::DiscardDeviceResources() {
  if (--mResourceRefCount == 0) {
    for (int i = 0; i < mStateCount; ++i) {
      mStates[i].DiscardDeviceResources();
    }
  }
  assert(mResourceRefCount >= 0);
}


bool BackgroundPainter::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void BackgroundPainter::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID data, UINT state) const {
  PerPaneData *paneData = (PerPaneData*)data;
  mStates[state].Paint(renderTarget, area, paneData->stateData[state], pane);
}


void BackgroundPainter::PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT, UINT newState, float) const {
  Paint(renderTarget, area, pane, painterData, newState);
}


void BackgroundPainter::PositionChanged(const IPane *pane, LPVOID data,
    const D2D1_RECT_F &position, bool isResize, bool isMove) {
  PerPaneData *paneData = (PerPaneData*)data;
  for (int i = 0; i < mStateCount; ++i) {
    mStates[i].PositionChanged(pane, paneData->stateData[i], position, isResize, isMove);
  }
}


void BackgroundPainter::RemovePane(const IPane*, LPVOID data) {
  delete (PerPaneData*)data;
}


void BackgroundPainter::TextChanged(const IPane*, LPVOID, LPCWSTR) {}
