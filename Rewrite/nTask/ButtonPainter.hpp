#pragma once

#include "../nCoreApi/IPanePainter.hpp"
#include "../nCoreApi/IStatePainter.hpp"

class ButtonPainter : public IPanePainter {
public:
  ButtonPainter(IStatePainter *statePainter, const NRECT &iconPosition);
  ~ButtonPainter();

public:
  ButtonPainter(const ButtonPainter&) = delete;
  ButtonPainter &operator=(const ButtonPainter&) = delete;

  // IPanePainter
public:
  LPVOID APICALL AddPane(const IPane *pane) override;
  HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void APICALL DiscardDeviceResources() override;
  bool APICALL DynamicColorChanged(ID2D1RenderTarget *renderTarget) override;
  void APICALL Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area, const IPane *pane,
    LPVOID painterData) const override;
  void APICALL PositionChanged(const IPane *pane, LPVOID painterData,
    const D2D1_RECT_F &position, bool isMove, bool isSize) override;
  void APICALL RemovePane(const IPane *pane, LPVOID painterData) override;
  void APICALL TextChanged(const IPane *pane, LPVOID painterData, LPCWSTR text) override;

public:
  void SetIcon(HICON icon);

private:
  HRESULT BrushFromIcon(HICON icon, ID2D1RenderTarget *renderTarget, D2D1_SIZE_U *size,
    ID2D1BitmapBrush **brush);

  D2D1_MATRIX_3X2_F GetTransform() const;

private:
  IStatePainter *mStatePainter;
  ID2D1BitmapBrush *mIconBrush;
  ID2D1RenderTarget *mRenderTarget;

  D2D1_SIZE_U mIconSize;

  HICON mIcon;
  const NRECT &mIconPosition;
  D2D1_RECT_F mIconPaintingPosition;
};
