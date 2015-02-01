#pragma once

#include "../nCoreApi/IStatePainter.hpp"

// TODO(Erik): Make this support rendering to different top-level windows.
class StatePainter : public IStatePainter {
private:
  struct PainterData {

  };

public:
  StatePainter(const StatePainterInitData *initData);

  // IPanePainter
public:
  LPVOID APICALL AddPane(const IPane *pane) override;
  HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void APICALL DiscardDeviceResources() override;
  bool APICALL DynamicColorChanged(ID2D1RenderTarget *renderTarget) override;
  void APICALL Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area, const IPane *pane,
    LPVOID painterData) const override;
  void APICALL PositionChanged(const IPane *pane, LPVOID painterData,
    D2D1_RECT_F position) override;
  void APICALL RemovePane(const IPane *pane, LPVOID painterData) override;
  void APICALL TextChanged(const IPane *pane, LPVOID painterData, LPCWSTR text) override;

  // IStatePainter
public:
  void APICALL Destroy() override;

private:
  ID2D1Brush *mBrush;
  D2D_COLOR_F mColor;
  int mResourceRefCount;
};
