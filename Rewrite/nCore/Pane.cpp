#include "Displays.hpp"
#include "Pane.hpp"

#include "../nShared/LsModule.hpp"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"

#include <assert.h>
#include <dwmapi.h>

extern Displays gDisplays;
extern LsModule gLsModule;
extern HINSTANCE gInstance;



EXPORT_CDECL(IPane*) CreatePane(const PaneInitData *initData) {
  return (IPane*)new Pane(initData, nullptr);
}


EXPORT_CDECL(IPane*) FindPane() {
  return nullptr;
}


LRESULT WINAPI Pane::ExternWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  return ((Pane*)GetWindowLongPtr(window, 0))->HandleMessage(window, message, wParam, lParam, 0);
}


LRESULT WINAPI Pane::InitWindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_CREATE) {
    SetWindowLongPtr(window, 0, (LONG_PTR)((LPCREATESTRUCT)lParam)->lpCreateParams);
    SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)&Pane::ExternWindowProc);
    return ExternWindowProc(window, message, wParam, lParam);
  }
  return DefWindowProc(window, message, wParam, lParam);
}


HRESULT Pane::CreateWindowClasses(HINSTANCE instance) {
  WNDCLASSEX wc;
  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.cbWndExtra = sizeof(Pane*);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.hInstance = instance;
  wc.lpfnWndProc = InitWindowProc;
  wc.style = CS_DBLCLKS;

  wc.lpszClassName = L"LSnPane";
  if (!RegisterClassEx(&wc)) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  wc.lpszClassName = L"DesktopBackgroundClass";
  if (!RegisterClassEx(&wc)) {
    return HRESULT_FROM_WIN32(GetLastError());
  }

  return S_OK;
}


void Pane::DestroyWindowClasses(HINSTANCE instance) {
  UnregisterClass(L"LSnPane", instance);
  UnregisterClass(L"DesktopBackgroundClass", instance);
}


Pane::Pane(const PaneInitData *initData, Pane *parent)
  : mMessageHandler(initData->messageHandler)
  , mPainter(initData->painter)
  , mRenderTarget(nullptr)
  , mWindow(nullptr)
{
  if (initData->settingsReader) {
    mSettings.alwaysOnTop = initData->settingsReader->GetBool(L"AlwaysOnTop", false);
    mSettings.position.top = initData->settingsReader->GetLength(L"Y", NLENGTH(0, 0, 0));
    mSettings.position.left = initData->settingsReader->GetLength(L"X", NLENGTH(0, 0, 0));
    mSettings.position.right = initData->settingsReader->GetLength(L"Width", NLENGTH(0, 0, 200))
      + mSettings.position.left;
    mSettings.position.bottom = initData->settingsReader->GetLength(L"Height", NLENGTH(0, 0, 200))
      + mSettings.position.top;
  } else {
    mSettings.alwaysOnTop = false;
    mSettings.position.left = NLENGTH(0, 0, 0);
    mSettings.position.right = NLENGTH(0, 0, 200);
    mSettings.position.top = NLENGTH(0, 0, 0);
    mSettings.position.bottom = NLENGTH(0, 0, 200);
  }

  float x, y, width, height;
  DWORD style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
  DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_COMPOSITED | WS_EX_NOACTIVATE;
  HWND parentWindow = nullptr;
  HWND insertAfter = HWND_BOTTOM;
  UINT windowPosFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED;
  LPCWSTR windowClass;

  if (CHECKFLAG(initData->flags, PaneInitData::DesktopWindow)) {
    x = (float)GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = (float)GetSystemMetrics(SM_YVIRTUALSCREEN);
    width = (float)GetSystemMetrics(SM_CXVIRTUALSCREEN);
    height = (float)GetSystemMetrics(SM_CYVIRTUALSCREEN);
    parentWindow = GetDesktopWindow();
    style |= WS_CHILD;
    windowPosFlags |= SWP_NOACTIVATE | SWP_NOSENDCHANGING;
    windowClass = L"DesktopBackgroundClass";
  } else {
    x = EvaluateLength(mSettings.position.left);
    y = EvaluateLength(mSettings.position.top);
    width = EvaluateLength(mSettings.position.right) - x;
    height = EvaluateLength(mSettings.position.bottom) - y;
    if (mSettings.alwaysOnTop) {
      insertAfter = HWND_TOPMOST;
    } else {
      windowPosFlags |= SWP_NOZORDER;
    }
    windowClass = L"LSnPane";
  }

  mSize.width = (UINT32)width;
  mSize.height = (UINT32)height;

  mWindow = CreateWindowEx(exStyle, windowClass, initData->name ? initData->name : L"", style,
    (int)x, (int)y, (int)width, (int)height, parentWindow, nullptr, gInstance, this);
  SetWindowLongPtr(mWindow, GWLP_USERDATA, MAGIC_DWORD);
  SetWindowPos(mWindow, insertAfter, (int)x, (int)y, (int)width, (int)height, windowPosFlags);

  MARGINS margins;
  ZeroMemory(&margins, sizeof(MARGINS));
  margins.cyTopHeight = INT_MAX;
  DwmExtendFrameIntoClientArea(mWindow, &margins);

  ShowWindow(mWindow, SW_SHOWNOACTIVATE);
}


Pane::~Pane() {
  DiscardDeviceResources();
  DestroyWindow(mWindow);
}


void Pane::Destroy() {
  delete this;
}


HWND Pane::GetWindow() const {
  return mWindow;
}


IPane *Pane::CreateChild(const PaneInitData *initData) {
  Pane *pane = new Pane(initData, this);

  return pane;
}


HRESULT Pane::ReCreateDeviceResources() {
  HRESULT hr = S_OK;

  if (!mRenderTarget) {
    ID2D1Factory *factory;
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
#ifdef _DEBUG
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, __uuidof(ID2D1Factory), &options,
      (void**)&factory);
    if (SUCCEEDED(hr)) {
      hr = factory->CreateHwndRenderTarget(
        D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
          D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
          96.0f, 96.0f, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT),
        D2D1::HwndRenderTargetProperties(mWindow, D2D1::SizeU(mSize.width, mSize.height)),
        &mRenderTarget);
      if (SUCCEEDED(hr)) {
        mRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
        hr = mPainter->CreateDeviceResources(mRenderTarget);
      }
      factory->Release();
    }
  }

  if (FAILED(hr)) {
    DiscardDeviceResources();
  }

  return hr;
}


void Pane::DiscardDeviceResources() {
  mPainter->DiscardDeviceResources();
  SAFERELEASE(mRenderTarget);
}


float Pane::EvaluateLength(NLENGTH &length) {
  return length.Evaluate(0, 96.0f);
}


LRESULT Pane::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, NPARAM) {
  switch (msg) {
  case WM_ERASEBKGND:
    return 1;

  case WM_NCPAINT:
    return 0;

  case WM_PAINT:
    {
      RECT updateRect;
      if (GetUpdateRect(window, &updateRect, FALSE) != FALSE) {
        if (ReCreateDeviceResources() == S_OK) {
          D2D1_RECT_F d2dUpdateRect = D2D1::RectF((FLOAT)updateRect.left, (FLOAT)updateRect.top,
            (FLOAT)updateRect.right, (FLOAT)updateRect.bottom);

          mRenderTarget->BeginDraw();
          mRenderTarget->PushAxisAlignedClip(&d2dUpdateRect, D2D1_ANTIALIAS_MODE_ALIASED);
          mRenderTarget->Clear();
          mPainter->Paint(mRenderTarget, &d2dUpdateRect, (IPane*)this);
          mRenderTarget->PopAxisAlignedClip();

          HRESULT hr = mRenderTarget->EndDraw();

          if (hr == D2DERR_RECREATE_TARGET) {
            DiscardDeviceResources();
            InvalidateRect(mWindow, nullptr, FALSE);
            UpdateWindow(mWindow);
          } else if (SUCCEEDED(hr)) {
            ValidateRect(window, &updateRect);
          } else {
            // TODO(Erik): Deal with this.
            assert(false);
          }
        }
      }
    }
    return 0;

  case WM_WINDOWPOSCHANGING:
    {
      if (mSettings.alwaysOnTop) {
        LPWINDOWPOS windowPos = LPWINDOWPOS(lParam);
        // TODO(Erik): Handle covered by fullscreen.
        windowPos->hwndInsertAfter = HWND_TOPMOST;
      }
    }
    break;
  }

  return mMessageHandler->HandleMessage(window, msg, wParam, lParam, (NPARAM)(IPane*)this);
}
