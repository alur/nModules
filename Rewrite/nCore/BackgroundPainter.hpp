#pragma once

#include "BackgroundPainterState.hpp"

#include "../nCoreApi/IDiscardablePainter.hpp"
#include "../nCoreApi/IPane.hpp"

#include <vector>

// TODO(Erik): Make this support rendering to different top-level windows.
class BackgroundPainter : public IDiscardablePainter {
private:
  struct PerPaneData {
    D2D1_ROUNDED_RECT position;
    std::vector<BackgroundPainterState::PerPaneData> stateData;
  };

public:
  explicit BackgroundPainter(const ISettingsReader*, const StateDefinition*, BYTE numStates);
  ~BackgroundPainter();

  // IDiscardable
public:
  void APICALL Discard() override;

  // IPainter
public:
  LPVOID APICALL AddPane(const IPane *pane) override;
  HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void APICALL DiscardDeviceResources() override;
  bool APICALL DynamicColorChanged(ID2D1RenderTarget *renderTarget) override;
  void APICALL Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area, const IPane *pane,
    LPVOID painterData, UINT state) const override;
  void APICALL PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT oldState, UINT newState, float time) const
    override;
  void APICALL PositionChanged(const IPane *pane, LPVOID painterData,
    const D2D1_RECT_F &position, bool isMove, bool isSize) override;
  void APICALL RemovePane(const IPane *pane, LPVOID painterData) override;
  void APICALL TextChanged(const IPane *pane, LPVOID painterData, LPCWSTR text) override;

private:
  int mResourceRefCount;
  int mStateCount;
  BackgroundPainterState *mStates;
};
