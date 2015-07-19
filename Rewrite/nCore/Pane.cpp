#include "Api.h"
#include "Displays.hpp"
#include "Pane.hpp"

#include "../nShared/Math.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

#include <assert.h>
#include <dwmapi.h>
#include <strsafe.h>
#include <unordered_set>
#include <unordered_map>

extern Displays gDisplays;
extern HINSTANCE gInstance;

// Top-level always on top panes.
static std::unordered_set<Pane*> sAlwaysOnTopPanes;

// The panes which are, or want to be, children of the pane with the given name.
static std::unordered_map<std::wstring, std::unordered_set<Pane*>> sChildren;

// All named panes.
static std::unordered_map<std::wstring, Pane*> sNamedPanes;


void Pane::FullscreenActivated(HMONITOR monitor, HWND fullscreenWindow) {
  for (Pane *pane : sAlwaysOnTopPanes) {
    pane->OnFullscreenActivated(monitor, fullscreenWindow);
  }
}


void Pane::FullscreenDeactivated(HMONITOR monitor) {
  for (Pane *pane : sAlwaysOnTopPanes) {
    pane->OnFullscreenDeactivated(monitor);
  }
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
  , mCurrentState(0)
  , mActiveStates(initData->numStates + 1, false)
  , mParent(parent)
  , mRenderTarget(nullptr)
  , mText(nullptr)
  , mUpdateLock(0)
  , mVisible(false)
  , mWindow(nullptr)
  , mActiveChild(nullptr)
  , mIsTrackingMouse(false)
  , mStateDependencies(initData->numStates + 1)
  , mStateDependents(initData->numStates + 1)
{
  mName[0] = L'\0';
  if (initData->name) {
    StringCchCopy(mName, MAX_PREFIX, initData->name);
  }

  for (int i = 0; i < initData->numStates; ++i) {
    ULONGLONG dependencies = initData->states[i].dependencies;
    for (int j = 1; dependencies; ++j, dependencies >>= 1) {
      if (dependencies & 1) {
        mStateDependencies[i+1].push_back((BYTE)j);
        mStateDependents[j].push_back((BYTE)i+1);
      }
    }
  }

  // Defaults
  mSettings.alwaysOnTop = false;
  mSettings.clickThrough = false;
  mSettings.parent[0] = L'\0';
  mSettings.position.top = NLENGTH(0, 0, 0);
  mSettings.position.left = NLENGTH(0, 0, 0);
  mSettings.position.right = NLENGTH(0, 0, 200);
  mSettings.position.bottom = NLENGTH(0, 0, 200);

  if (initData->settingsReader) {
    const ISettingsReader *reader = initData->settingsReader;
    mSettings.alwaysOnTop = reader->GetBool(L"AlwaysOnTop", mSettings.alwaysOnTop);
    mSettings.clickThrough = reader->GetBool(L"ClickThrough", mSettings.clickThrough);
    if (!mParent) {
      reader->GetString(L"Parent", mSettings.parent, MAX_PREFIX, L"");
    }
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

  if (*mSettings.parent) {
    sChildren[mSettings.parent].insert(this);
    auto parent = sNamedPanes.find(mSettings.parent);
    if (parent != sNamedPanes.end()) {
      mParent = parent->second;
      mParent->mChildren.insert(this);
    }
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
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_COMPOSITED;
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
        sAlwaysOnTopPanes.insert(this);
      }
      windowClass = L"LSnPane";
    }

    mWindow = CreateWindowEx(exStyle, windowClass, initData->name ? initData->name : L"", style,
      (int)EvaluateLengthParent(mSettings.position.left, true),
      (int)EvaluateLengthParent(mSettings.position.top, false),
      (int)mSize.width, (int)mSize.height, parentWindow, nullptr, gInstance, this);
    SetWindowLongPtr(mWindow, GWLP_USERDATA, MAGIC_DWORD);
    SetWindowPos(mWindow, insertAfter, 0, 0, 0, 0, windowPosFlags);
    ShowWindow(mWindow, SW_HIDE);

    ZeroMemory(&mTrackMouseEvent, sizeof(TRACKMOUSEEVENT));
    mTrackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    mTrackMouseEvent.hwndTrack = mWindow;
    mTrackMouseEvent.dwFlags = TME_LEAVE;
    mTrackMouseEvent.dwHoverTime = 200;

    MARGINS margins;
    ZeroMemory(&margins, sizeof(MARGINS));
    margins.cyTopHeight = INT_MAX;
    DwmExtendFrameIntoClientArea(mWindow, &margins);
  }

  mPainterData.resize(initData->numPainters);
  mPainters.resize(initData->numPainters);
  for (int i = 0; i < initData->numPainters; ++i) {
    mPainters[i] = initData->painters[i];
    mPainterData[i] = mPainters[i]->AddPane(this);
  }

  if (mParent && mParent->mRenderTarget) {
    ReCreateDeviceResources();
  }

  if (*mName) {
    assert(sNamedPanes.count(mName) == 0);
    sNamedPanes[mName] = this;

    for (Pane *child : sChildren[mName]) {
      mChildren.insert(child);
      child->mParent = this;
      child->ParentPositionChanged();
      child->ReCreateDeviceResources();
    }
  }
}


Pane::~Pane() {
  if (*mName) {
    sNamedPanes.erase(mName);
  }
  if (mSettings.parent) {
    sChildren[mSettings.parent].erase(this);
  }
  if (mSettings.alwaysOnTop) {
    sAlwaysOnTopPanes.erase(this);
  }
  DiscardDeviceResources();
  for (int i = 0; i < mPainters.size(); ++i) {
    mPainters[i]->RemovePane(this, mPainterData[i]);
  }
  if (mWindow) {
    DestroyWindow(mWindow);
  }
  if (mParent) {
    mParent->mChildren.erase(this);
    mParent->Repaint(mRenderingPosition, true);
    if (mParent->mActiveChild == this) {
      mParent->mActiveChild = nullptr;
    }
  }
  for (Pane *child : mChildren) {
    child->mParent = nullptr;
  }
}


void Pane::DiscardDeviceResources() {
  if (mRenderTarget) {
    for (IPainter *painter : mPainters) {
      painter->DiscardDeviceResources();
    }
    for (Pane *child : mChildren) {
      child->DiscardDeviceResources();
    }
    if (!mParent && mRenderTarget) {
      mRenderTarget->Release();
    }
    mRenderTarget = nullptr;
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
          for (int i = 0; i < mPainters.size() && SUCCEEDED(hr); ++i) {
            hr = mPainters[i]->CreateDeviceResources(mRenderTarget);
          }
        }
      }
    } else if (mParent->mRenderTarget) {
      mRenderTarget = mParent->mRenderTarget;
      for (int i = 0; i < mPainters.size() && SUCCEEDED(hr); ++i) {
        hr = mPainters[i]->CreateDeviceResources(mRenderTarget);
      }
    }
    if (mRenderTarget) {
      for (Pane *child : mChildren) {
        if (FAILED(hr)) {
          break;
        }
        hr = child->ReCreateDeviceResources();
      }
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
    for (int i = 0; i < mPainters.size(); ++i) {
      mPainters[i]->Paint(renderTarget, &invalidatedArea, (IPane*)this, mPainterData[i],
        mCurrentState);
    }
    renderTarget->PopAxisAlignedClip();
  }
}


void Pane::ParentPositionChanged() {
  D2D1_RECT_F newPosition = D2D1::RectF(
    EvaluateLengthParent(mSettings.position.left, true) + mParent->mRenderingPosition.left,
    EvaluateLengthParent(mSettings.position.top, false) + mParent->mRenderingPosition.top,
    EvaluateLengthParent(mSettings.position.right, true) + mParent->mRenderingPosition.left,
    EvaluateLengthParent(mSettings.position.bottom, false) + mParent->mRenderingPosition.top);

  D2D1_SIZE_F newSize = D2D1::SizeF(
    newPosition.right - newPosition.left,
    newPosition.bottom - newPosition.top);

  bool isMove = newPosition.left != mRenderingPosition.left
    || newPosition.top != mRenderingPosition.top;
  bool isSize = newSize.width != mSize.width || newSize.height != mSize.height;

  if (!isMove && !isSize) {
    return;
  }

  mRenderingPosition = newPosition;
  mSize = newSize;
  for (int i = 0; i < mPainters.size(); ++i) {
    mPainters[i]->PositionChanged(this, mPainterData[i], mRenderingPosition, isMove, isSize);
  }
}


void Pane::Repaint(bool update) {
  Repaint(mRenderingPosition, update);
}


void Pane::Repaint(const D2D1_RECT_F &area, bool update) {
  if (mVisible) {
    if (!IsChildPane()) {
      RECT r = { (LONG)area.left, (LONG)area.top, (LONG)area.right, (LONG)area.bottom };
      InvalidateRect(mWindow, &r, FALSE);
      if (update && mUpdateLock == 0) {
        UpdateWindow(mWindow);
      }
    } else if (mParent) {
      mParent->Repaint(area, update && mUpdateLock == 0);
    }
  }
}


void Pane::RepaintInvalidated() const {
  if (mUpdateLock == 0) {
    if (!IsChildPane() && mWindow) {
      UpdateWindow(mWindow);
    } else if (mParent) {
      mParent->RepaintInvalidated();
    }
  }
}


void Pane::OnFullscreenActivated(HMONITOR monitor, HWND fullscreenWindow) {
  if (MonitorFromWindow(mWindow, MONITOR_DEFAULTTONULL) == monitor) {
    mCoveredByFullscreenWindow = true;
    SetWindowPos(mWindow, fullscreenWindow, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE |
      SWP_NOREDRAW);
  }
}


void Pane::OnFullscreenDeactivated(HMONITOR monitor) {
  if (MonitorFromWindow(mWindow, MONITOR_DEFAULTTONULL) == monitor) {
    mCoveredByFullscreenWindow = false;
    SetWindowPos(mWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
  }
}
