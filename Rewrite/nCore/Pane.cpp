#include "Api.h"
#include "Displays.hpp"
#include "Pane.hpp"

#include "../nShared/Math.h"

#include "../nUtilities/lsapi.h"
#include "../nUtilities/Macros.h"

#include <assert.h>
#include <dwmapi.h>

extern Displays gDisplays;
extern HINSTANCE gInstance;


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
  , mPainterData(nullptr)
  , mParent(parent)
  , mRenderTarget(nullptr)
  , mText(nullptr)
  , mUpdateLock(0)
  , mVisible(false)
  , mWindow(nullptr)
{
  // Defaults
  mSettings.alwaysOnTop = false;
  mSettings.parent[0] = L'\0';
  mSettings.position.top = NLENGTH(0, 0, 0);
  mSettings.position.left = NLENGTH(0, 0, 0);
  mSettings.position.right = NLENGTH(0, 0, 200);
  mSettings.position.bottom = NLENGTH(0, 0, 200);

  if (initData->settingsReader) {
    const ISettingsReader *reader = initData->settingsReader;
    mSettings.alwaysOnTop = reader->GetBool(L"AlwaysOnTop", mSettings.alwaysOnTop);
    reader->GetString(L"Parent", mSettings.parent, MAX_PREFIX, L"");
    mSettings.position.top = reader->GetLength(L"Y", mSettings.position.top);
    mSettings.position.left = reader->GetLength(L"X", mSettings.position.left);
    mSettings.position.right = reader->GetLength(L"Width", mSettings.position.right)
      + mSettings.position.left;
    mSettings.position.bottom = reader->GetLength(L"Height", mSettings.position.bottom)
      + mSettings.position.top;
  }
  if (CHECKFLAG(initData->flags, PaneInitData::DesktopWindow)) {
    assert(!parent);
    mSettings.position.top = NLENGTH((float)GetSystemMetrics(SM_YVIRTUALSCREEN), 0, 0);
    mSettings.position.left = NLENGTH((float)GetSystemMetrics(SM_XVIRTUALSCREEN), 0, 0);
    mSettings.position.right = NLENGTH((float)GetSystemMetrics(SM_CXVIRTUALSCREEN), 0, 0);
    mSettings.position.bottom = NLENGTH((float)GetSystemMetrics(SM_CYVIRTUALSCREEN), 0, 0);
  }

  if (mParent) {
    mDpi = mParent->mDpi;
    mRenderingPosition = D2D1::RectF(
      EvaluateLengthParent(mSettings.position.left, true) + mParent->mRenderingPosition.left,
      EvaluateLengthParent(mSettings.position.top, false) + mParent->mRenderingPosition.top,
      EvaluateLengthParent(mSettings.position.right, true) + mParent->mRenderingPosition.left,
      EvaluateLengthParent(mSettings.position.bottom, false) + mParent->mRenderingPosition.top);
  } else {
    mDpi = D2D1::Point2F(
      (float)gDisplays.GetDisplay(0).dpi.x,
      (float)gDisplays.GetDisplay(0).dpi.y);
    mRenderingPosition = D2D1::RectF(0, 0,
      EvaluateLengthParent(mSettings.position.right - mSettings.position.left, true),
      EvaluateLengthParent(mSettings.position.bottom - mSettings.position.top, false));
  }
  mSize = D2D1::SizeF(
    mRenderingPosition.right - mRenderingPosition.left,
    mRenderingPosition.bottom - mRenderingPosition.top);

  if (!IsChildPane()) {
    DWORD style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_COMPOSITED | WS_EX_NOACTIVATE;
    HWND parentWindow = nullptr;
    HWND insertAfter = HWND_BOTTOM;
    UINT windowPosFlags = SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED;
    LPCWSTR windowClass;

    if (CHECKFLAG(initData->flags, PaneInitData::DesktopWindow)) {
      parentWindow = GetDesktopWindow();
      style |= WS_CHILD;
      windowPosFlags |= SWP_NOACTIVATE | SWP_NOSENDCHANGING;
      windowClass = L"DesktopBackgroundClass";
    } else {
      if (mSettings.alwaysOnTop) {
        insertAfter = HWND_TOPMOST;
      } else {
        windowPosFlags |= SWP_NOZORDER;
      }
      windowClass = L"LSnPane";
    }

    mWindow = CreateWindowEx(exStyle, windowClass, initData->name ? initData->name : L"", style,
      (int)EvaluateLengthParent(mSettings.position.left, true),
      (int)EvaluateLengthParent(mSettings.position.top, false),
      (int)mSize.width, (int)mSize.height, parentWindow, nullptr, gInstance, this);
    SetWindowLongPtr(mWindow, GWLP_USERDATA, MAGIC_DWORD);
    SetWindowPos(mWindow, insertAfter, 0, 0, 0, 0, windowPosFlags);

    MARGINS margins;
    ZeroMemory(&margins, sizeof(MARGINS));
    margins.cyTopHeight = INT_MAX;
    DwmExtendFrameIntoClientArea(mWindow, &margins);
  }

  mPainterData = mPainter->AddPane(this);
  if (mParent && mParent->mRenderTarget) {
    ReCreateDeviceResources();
  }
}


Pane::~Pane() {
  DiscardDeviceResources();
  mPainter->RemovePane(this, mPainterData);
  if (mWindow) {
    DestroyWindow(mWindow);
  }
  if (mParent) {
    mParent->mChildren.erase(this);
    mParent->Repaint(mRenderingPosition, true);
  }
}


void Pane::DiscardDeviceResources() {
  mPainter->DiscardDeviceResources();
  if (!mParent && mRenderTarget) {
    mRenderTarget->Release();
  }
  mRenderTarget = nullptr;
  for (Pane *child : mChildren) {
    child->DiscardDeviceResources();
  }
}


HRESULT Pane::ReCreateDeviceResources() {
  HRESULT hr = S_OK;

  if (!mRenderTarget) {
    if (!mParent) {
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
          D2D1::HwndRenderTargetProperties(mWindow,
          D2D1::SizeU((UINT32)mSize.width, (UINT32)mSize.height)), &mRenderTarget);
        factory->Release();
        if (SUCCEEDED(hr)) {
          mRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_CLEARTYPE);
          hr = mPainter->CreateDeviceResources(mRenderTarget);
        }
      }
    } else {
      mRenderTarget = mParent->mRenderTarget;
      hr = mPainter->CreateDeviceResources(mRenderTarget);
    }
    for (Pane *child : mChildren) {
      if (FAILED(hr)) {
        break;
      }
      hr = child->ReCreateDeviceResources();
    }
  }

  if (FAILED(hr) && !mParent) {
    DiscardDeviceResources();
  }

  return hr;
}


bool Pane::IsChildPane() const {
  return mParent != nullptr || *mSettings.parent != L'\0';
}


float Pane::EvaluateLengthParent(const NLENGTH &length, bool horizontal) const {
  float parentLength = 0;
  if (mParent) {
    parentLength = horizontal ? mParent->mSize.width : mParent->mSize.height;
  } else if (!IsChildPane()) {
    // TODO(Erik): This should be based on which monitor we belong to.
    parentLength = float(horizontal ?
      gDisplays.GetDisplay(0).width : gDisplays.GetDisplay(0).height);
  }
  return length.Evaluate(parentLength, horizontal ? mDpi.x : mDpi.y);
}


void Pane::Paint(ID2D1RenderTarget *renderTarget, const D2D1_RECT_F *area) const {
  D2D1_RECT_F invalidatedArea;
  if (mVisible && RectIntersection(area, &mRenderingPosition, &invalidatedArea)) {
    renderTarget->PushAxisAlignedClip(invalidatedArea, D2D1_ANTIALIAS_MODE_ALIASED);
    mPainter->Paint(renderTarget, &invalidatedArea, (IPane*)this, mPainterData);
    renderTarget->PopAxisAlignedClip();
  }
}


void Pane::Repaint(bool update) {
  Repaint(mRenderingPosition, update);
}


void Pane::Repaint(const D2D1_RECT_F &area, bool update) {
  if (mVisible) {
    if (IsChildPane() && mParent) {
      mParent->Repaint(area, update && mUpdateLock == 0);
    } else {
      RECT r = { (LONG)area.left, (LONG)area.top, (LONG)area.right, (LONG)area.bottom };
      InvalidateRect(mWindow, &r, FALSE);
      if (update && mUpdateLock == 0) {
        UpdateWindow(mWindow);
      }
    }
  }
}
