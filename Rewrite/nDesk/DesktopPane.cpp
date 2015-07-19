#include "DesktopPane.hpp"

#include "../nCoreApi/Core.h"

#include "../nShared/Math.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

#include <algorithm>
#include <ShlObj.h>
#include <wincodec.h>


DesktopPane::DesktopPane()
  : mPane(nullptr)
  , mRenderTarget(nullptr)
{
  PaneInitData initData;
  ZeroMemory(&initData, sizeof(PaneInitData));
  initData.cbSize = sizeof(PaneInitData);
  initData.name = nullptr;
  initData.messageHandler = (IMessageHandler*)this;
  IPainter *painter = (IPainter*)this;
  initData.painters = &painter;
  initData.numPainters = 1;
  initData.flags = PaneInitData::DesktopWindow;

  ISettingsReader *reader = nCore::CreateSettingsReader(L"nDesk", nullptr);
  mEventHandler = nCore::CreateEventHandler(reader);
  reader->Discard();

  mPane = nCore::CreatePane(&initData);
  mPane->Show();
}


DesktopPane::~DesktopPane() {
  if (mPane != nullptr) {
    mPane->Discard();
  }
  mEventHandler->Discard();
}


LRESULT DesktopPane::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (msg) {
  case WM_WINDOWPOSCHANGING:
    {
      // Keep the hWnd at the bottom of the window stack
      LPWINDOWPOS c = LPWINDOWPOS(lParam);
      c->hwnd = window;
      c->hwndInsertAfter = HWND_BOTTOM;
      c->flags &= ~SWP_HIDEWINDOW;
      c->flags |= SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOMOVE | SWP_SHOWWINDOW;
      const Display &desktop = nCore::GetDisplays()->GetDesktop();
      c->x = desktop.rect.left;
      c->y = desktop.rect.top;
      c->cx = desktop.width;
      c->cy = desktop.height;
    }
    return 0;
  }

  return mEventHandler->HandleMessage(window, msg, wParam, lParam, nullptr);
}


LPVOID DesktopPane::AddPane(const IPane*) {
  return nullptr;
}


HRESULT DesktopPane::CreateDeviceResources(ID2D1RenderTarget *renderTarget) {
  HRESULT hr = S_OK;
  mRenderTarget = renderTarget;
  IDesktopWallpaper *desktopWallpaper;
  hr = LSCoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL, IID_IDesktopWallpaper,
    (void**)&desktopWallpaper);
  if (SUCCEEDED(hr)) {
    const Display &desktop = nCore::GetDisplays()->GetDesktop();

    DESKTOP_WALLPAPER_POSITION position;
    desktopWallpaper->GetPosition(&position);

    UINT numMonitors;
    hr = desktopWallpaper->GetMonitorDevicePathCount(&numMonitors);
    if (SUCCEEDED(hr)) {
      mWallpapers.resize(numMonitors);
      for (UINT i = 0; i < numMonitors; ++i) {
        Wallpaper &wallpaper = mWallpapers[i];

        LPWSTR id;
        hr = desktopWallpaper->GetMonitorDevicePathAt(i, &id);
        if (SUCCEEDED(hr)) {
          RECT rect;
          hr = desktopWallpaper->GetMonitorRECT(id, &rect);
          if (SUCCEEDED(hr)) {
            wallpaper.rect.top = (float)(rect.top - desktop.rect.top);
            wallpaper.rect.left = (float)(rect.left - desktop.rect.left);
            wallpaper.rect.right = (float)(rect.right - desktop.rect.left);
            wallpaper.rect.bottom = (float)(rect.bottom - desktop.rect.top);

            LPWSTR file;
            hr = desktopWallpaper->GetWallpaper(id, &file);
            if (SUCCEEDED(hr)) {
              UINT width, height;
              wchar_t expandedPath[MAX_PATH];
              ID2D1BitmapBrush *bitmapBrush;
              ExpandEnvironmentStrings(file, expandedPath, MAX_PATH);
              hr = CreateBitmapBrush(expandedPath, renderTarget, &bitmapBrush, &width, &height);
              if (SUCCEEDED(hr)) {
                D2D1_SIZE_F translation = D2D1::SizeF(wallpaper.rect.left, wallpaper.rect.top);
                D2D1_SIZE_F scale = D2D1::SizeF(1, 1);

                switch (position) {
                default:
                case DWPOS_FILL:
                  scale.height = scale.width = std::max(
                    float(rect.right - rect.left) / width,
                    float(rect.bottom - rect.top) / height);
                  translation.width += (rect.right - rect.left - width * scale.width) / 2.0f;
                  translation.height += (rect.bottom - rect.top - height * scale.height) / 2.0f;
                  break;

                case DWPOS_FIT:
                  scale.height = scale.width = std::min(
                    float(rect.right - rect.left) / width,
                    float(rect.bottom - rect.top) / height);
                  translation.width += (rect.right - rect.left - width * scale.width) / 2.0f;
                  translation.height += (rect.bottom - rect.top - height * scale.height) / 2.0f;
                  break;

                case DWPOS_STRETCH:
                  scale = D2D1::SizeF(
                    float(rect.right - rect.left) / width,
                    float(rect.bottom - rect.top) / height);
                  break;

                case DWPOS_CENTER:
                  translation.width += (rect.right - rect.left - width) / 2.0f;
                  translation.height += (rect.bottom - rect.top - height) / 2.0f;
                  break;

                case DWPOS_SPAN:
                  break;

                case DWPOS_TILE:
                  break;
                }

                bitmapBrush->SetTransform(
                  D2D1::Matrix3x2F::Scale(scale) * D2D1::Matrix3x2F::Translation(translation));
                wallpaper.brush = (ID2D1Brush*)bitmapBrush;
              }
              CoTaskMemFree(file);
            }
          }
          CoTaskMemFree(id);
        }
        if (FAILED(hr)) {
          break;
        }
      }
    }
    desktopWallpaper->Release();
  }
  return hr;
}


void DesktopPane::DiscardDeviceResources() {
  for (Wallpaper &wallpaper : mWallpapers) {
    SAFERELEASE(wallpaper.brush);
  }
  mRenderTarget = nullptr;
}


bool DesktopPane::DynamicColorChanged(ID2D1RenderTarget*) {
  return false;
}


void DesktopPane::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area, const IPane*,
    LPVOID, UINT) const {
  for (const Wallpaper &wallpaper : mWallpapers) {
    D2D1_RECT_F invalidatedArea;
    if (RectIntersection(area, &wallpaper.rect, &invalidatedArea)) {
      renderTarget->FillRectangle(invalidatedArea, wallpaper.brush);
    }
  }
}


void DesktopPane::PaintTransform(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area,
    const class IPane *pane, LPVOID painterData, UINT, UINT newState, float) const {
  Paint(renderTarget, area, pane, painterData, newState);
}


void DesktopPane::PositionChanged(const IPane*, LPVOID, const D2D1_RECT_F&, bool, bool) {
}


void DesktopPane::RemovePane(const IPane*, LPVOID) {
}


void DesktopPane::TextChanged(const IPane*, LPVOID, LPCWSTR) {
}


void DesktopPane::UpdateWallpapers() {
  // TODO(Erik):
  if (mRenderTarget) {
    ID2D1RenderTarget *renderTarget = mRenderTarget;
    DiscardDeviceResources();
    CreateDeviceResources(renderTarget);
    mPane->Repaint(nullptr);
  }
}


HRESULT DesktopPane::CreateBitmapBrush(LPCWSTR file, ID2D1RenderTarget *renderTarget,
    ID2D1BitmapBrush **brush, LPUINT widthOut, LPUINT heightOut) {
  HRESULT hr = S_OK;
  IWICImagingFactory *factory = nCore::GetWICFactory();
  IWICBitmapDecoder *decoder;
  hr = factory->CreateDecoderFromFilename(file, nullptr, GENERIC_READ,
    WICDecodeMetadataCacheOnDemand, &decoder);
  if (SUCCEEDED(hr)) {
    IWICBitmapFrameDecode *frame;
    hr = decoder->GetFrame(0, &frame);
    if (SUCCEEDED(hr)) {
      hr = frame->GetSize(widthOut, heightOut);
      if (SUCCEEDED(hr)) {
        IWICFormatConverter *converter;
        hr = factory->CreateFormatConverter(&converter);
        if (SUCCEEDED(hr)) {
          hr = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone,
            nullptr, 0.0, WICBitmapPaletteTypeMedianCut);
          if (SUCCEEDED(hr)) {
            ID2D1Bitmap *bitmap;
            hr = renderTarget->CreateBitmapFromWicBitmap(converter, &bitmap);
            if (SUCCEEDED(hr)) {
              hr = renderTarget->CreateBitmapBrush(bitmap, brush);
              bitmap->Release();
            }
          }
          converter->Release();
        }
      }
      frame->Release();
    }
    decoder->Release();
  }
  return hr;
}
