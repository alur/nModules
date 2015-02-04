#pragma once

#include "StatePainterData.hpp"
#include "StatePainterState.hpp"

#include "../nCoreApi/IStatePainter.hpp"

#include <dwrite.h>

// TODO(Erik): Make this support rendering to different top-level windows.
class StatePainter : public IStatePainter {
private:
  struct PainterData {
    IDWriteTextLayout *textLayout;
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
    D2D1_RECT_F position, bool isMove, bool isSize) override;
  void APICALL RemovePane(const IPane *pane, LPVOID painterData) override;
  void APICALL TextChanged(const IPane *pane, LPVOID painterData, LPCWSTR text) override;

  // IStatePainter
public:
  void APICALL ActivateState(BYTE state, IPane *pane) override;
  void APICALL ClearState(BYTE state, IPane *pane) override;
  void APICALL Destroy() override;
  void APICALL PaintBackground(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID painterData) const override;
  void APICALL PaintText(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const IPane *pane, LPVOID painterData) const override;

private:
  int mResourceRefCount;

  // TODO(Move these out of here)
private:
  ID2D1Brush *mBrush;
  ID2D1Brush *mTextBrush;
  D2D_COLOR_F mColor;
};
