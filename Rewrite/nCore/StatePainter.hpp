#pragma once

#include "../nCoreApi/IStatePainter.hpp"

class StatePainter : public IStatePainter {
public:
  StatePainter();

  // IPanePainter
public:
  HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void APICALL DiscardDeviceResources() override;
  void APICALL Paint(ID2D1RenderTarget *renderTarget, D2D1_RECT_F *area, IPane *pane) const override;
  bool APICALL UpdateDWMColor(DWORD newColor, ID2D1RenderTarget *renderTarget) override;
  void APICALL UpdatePosition(D2D1_RECT_F parentPosition) override;

  // IStatePainter
public:
  void APICALL Destroy() override;

private:
  ID2D1Brush *mBrush;
};
