#pragma once

#include "../nCoreApi/IPanePainter.hpp"
#include "../nCoreApi/IStatePainter.hpp"

class ButtonPainter : public IPanePainter {
public:
  ButtonPainter(IStatePainter *statePainter);
  ~ButtonPainter();

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

public:
  void SetIcon(HICON icon);

private:
  HRESULT BrushFromIcon(HICON icon, ID2D1RenderTarget *renderTarget, ID2D1BitmapBrush **brush);

private:
  IStatePainter *mStatePainter;
  D2D1_RECT_F mIconPaintingPosition;
  ID2D1BitmapBrush *mIconBrush;
  ID2D1RenderTarget *mRenderTarget;
  HICON mIcon;
};
