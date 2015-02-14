#pragma once

#include "StatePainterData.hpp"
#include "State.hpp"

#include "../nCoreApi/IStatePainter.hpp"

#include <dwrite.h>

// TODO(Erik): Make this support rendering to different top-level windows.
class StatePainter : public IStatePainter {
private:
  struct PainterData {
    IDWriteTextLayout *textLayout;
    D2D1_RECT_F textPosition;
  };

public:
  explicit StatePainter(const StatePainterInitData *initData);
  ~StatePainter();

  // IPanePainter
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

  int mStateCount;
  State *mStates;

  // TODO(Erik): Move these out of here
private:
  ID2D1Brush *mBrush;
  ID2D1Brush *mTextBrush;
  D2D_COLOR_F mColor;

  NRECT mTextPadding;
};
