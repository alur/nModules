#pragma once

#include "../nCoreApi/IDiscardablePainter.hpp"

class TextPainter : public IDiscardablePainter {
private:
  struct PerPaneData {
    D2D1_RECT_F textPosition;
  };

public:
  explicit TextPainter(const ISettingsReader*, const StateDefinition*, BYTE numStates);
  ~TextPainter();

  // IDiscardable
public:
  void APICALL Discard() override;

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

private:
  int mResourceRefCount;
  int mStateCount;
  //State *mStates;

  // TODO(Erik): Move these out of here
private:
  ID2D1Brush *mBrush;
  NRECT mTextPadding;
};
