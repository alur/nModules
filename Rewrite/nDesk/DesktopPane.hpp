#pragma once

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IMessageHandler.hpp"
#include "../nCoreApi/IPane.hpp"
#include "../nCoreApi/IPainter.hpp"

#include <vector>

class DesktopPane : public IMessageHandler, public IPainter {
public:
  DesktopPane();
  ~DesktopPane();

public:
  void UpdateWallpapers();

  // IMessageHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, NPARAM) override;

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
  HRESULT CreateBitmapBrush(LPCWSTR file, ID2D1RenderTarget*, ID2D1BitmapBrush**, LPUINT widthOut,
    LPUINT heightOut);

private:
  struct Wallpaper {
    D2D1_RECT_F rect;
    ID2D1Brush *brush;
  };

private:
  std::vector<Wallpaper> mWallpapers;
  IEventHandler *mEventHandler;
  IPane *mPane;
  ID2D1RenderTarget *mRenderTarget;
};
