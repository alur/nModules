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
