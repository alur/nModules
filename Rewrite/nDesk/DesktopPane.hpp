#pragma once

#include "../nCoreApi/IMessageHandler.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IPanePainter.hpp"

#include <vector>

class DesktopPane : public IMessageHandler, public IPanePainter {
public:
  DesktopPane();
  ~DesktopPane();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

  // IPanePainter
public:
  HRESULT APICALL CreateDeviceResources(ID2D1RenderTarget *renderTarget) override;
  void APICALL DiscardDeviceResources() override;
  void APICALL Paint(ID2D1RenderTarget *renderTarget, D2D1_RECT_F *area, IPane *pane)
    const override;
  bool APICALL UpdateDWMColor(DWORD newColor, ID2D1RenderTarget *renderTarget) override;
  void APICALL UpdatePosition(D2D1_RECT_F parentPosition) override;

private:
  HRESULT CreateBitmapBrush(LPCWSTR file, ID2D1RenderTarget*, ID2D1BitmapBrush**, LPUINT widthOut,
    LPUINT heightOut);

private:
  struct Wallpaper {
    D2D1_RECT_F rect;
    ID2D1Brush *brush;
  };

private:
  std::vector<Wallpaper> mWallpapers;
  IPane *mPane;
  ID2D1RenderTarget *mRenderTarget;
};
