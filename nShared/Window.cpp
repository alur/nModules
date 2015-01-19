/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Window.cpp
 *  The nModules Project
 *
 *  A generic drawable window.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "Color.h"
#include "ErrorHandler.h"
#include "Factories.h"
#include "LiteStep.h"
#include "MessageHandler.hpp"
#include "Window.hpp"
#include "WindowSettings.hpp"

#include "../nCoreCom/Core.h"

#include "../Utilities/CommonD2D.h"
#include "../Utilities/Math.h"
#include "../Utilities/StringUtils.h"

#include <algorithm>
#include <dwmapi.h>
#include <dwrite.h>
#include <strsafe.h>
#include <Wincodec.h>
#include <windowsx.h>


using std::map;


/// <summary>
/// Constructor used to create a DrawableWindow for a pre-existing window. Used by nDesk.
/// </summary>
/// <param name="drawable">Pointer to the drawable which should be updated.</param>
void Window::TextChangeHandler(LPVOID drawable) {
  ((Window*)drawable)->UpdateText();
}


/// <summary>
/// Common constructor. Called by the other constructors to initalize common settings.
/// </summary>
/// <param name="settings">The settings to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
Window::Window(Settings* settings, MessageHandler* msgHandler)
    : activeChild(nullptr)
    , mAnimating(false)
    , initialized(false)
    , isTrackingMouse(false)
    , msgHandler(msgHandler)
    , parsedText(nullptr)
    , mParent(nullptr)
    , mRenderTarget(nullptr)
    , mSettings(settings)
    , text(nullptr)
    , visible(nullptr)
    , mDontForwardMouse(false)
    , mCaptureHandler(nullptr)
    , mIsChild(false)
    , mNeedsUpdate(false)
    , timerIDs(nullptr)
    , userMsgIDs(nullptr)
    , window(nullptr)
    , mCoveredByFullscreen(false)
    , mWindowData(nullptr)
    , mStateRender(nullptr)
{
    ZeroMemory(&this->drawingArea, sizeof(this->drawingArea));
}


/// <summary>
/// Constructor used to create a DrawableWindow for a pre-existing window. Used by nDesk.
/// </summary>
/// <param name="window">The window to draw to.</param>
/// <param name="prefix">The settings prefix to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
Window::Window(HWND window, LPCTSTR prefix, MessageHandler *msgHandler)
    : Window(new Settings(prefix), msgHandler)
{
    this->timerIDs = new UIDGenerator<UINT_PTR>(1);
    this->userMsgIDs = new UIDGenerator<UINT>(WM_FIRSTREGISTERED);
    this->window = window;

    this->initialized = true;
    this->visible = true;

    // Configure the mouse tracking struct
    ZeroMemory(&this->trackMouseStruct, sizeof(TRACKMOUSEEVENT));
    this->trackMouseStruct.cbSize = sizeof(TRACKMOUSEEVENT);
    this->trackMouseStruct.hwndTrack = this->window;
    this->trackMouseStruct.dwFlags = TME_LEAVE;
    this->trackMouseStruct.dwHoverTime = 200;
}


/// <summary>
/// Constructor used to create a window with a "Parent" setting.
/// </summary>
/// <param name="parent">The name of the parent's window.</param>
/// <param name="settings">The settings to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
Window::Window(LPCTSTR parent, Settings *settings, MessageHandler *msgHandler)
    : Window(new Settings(settings), msgHandler)
{
    StringCchCopy(mParentName, _countof(mParentName), parent);
    mParent = nCore::System::FindRegisteredWindow(mParentName);
    mIsChild = true;
    if (mParent)
    {
        mParent->children.push_back(this);
        this->window = mParent->window;
    }
    else
    {
        nCore::System::AddWindowRegistrationListener(mParentName, this);
    }
}


/// <summary>
/// Constructor used by LSModule to create a top-level window.
/// </summary>
/// <param name="parent">The window to use as a parent for this top-level window.</param>
/// <param name="windowClass">The windowclass to use for the top-level window.</param>
/// <param name="instance">Used for creating the window.</param>
/// <param name="settings">The settings to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
Window::Window(HWND /* parent */, LPCTSTR windowClass, HINSTANCE instance, Settings* settings, MessageHandler* msgHandler)
    : Window(new Settings(settings), msgHandler)
{
    this->timerIDs = new UIDGenerator<UINT_PTR>(1);
    this->userMsgIDs = new UIDGenerator<UINT>(WM_FIRSTREGISTERED);

    // Create the window
    this->window = MessageHandler::CreateMessageWindowEx(WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW | WS_EX_COMPOSITED,
        windowClass, settings->GetPrefix(), WS_POPUP, 0, 0, 0, 0, NULL, NULL, instance, this);
    SetWindowPos(this->window, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    SetWindowLongPtr(this->window, GWLP_USERDATA, MAGIC_DWORD);

    // Configure the mouse tracking struct
    ZeroMemory(&this->trackMouseStruct, sizeof(TRACKMOUSEEVENT));
    this->trackMouseStruct.cbSize = sizeof(TRACKMOUSEEVENT);
    this->trackMouseStruct.hwndTrack = this->window;
    this->trackMouseStruct.dwFlags = TME_LEAVE;
    this->trackMouseStruct.dwHoverTime = 200;

    // All frame :p
    MARGINS m;
    ZeroMemory(&m, sizeof(m));
    m.cyTopHeight = INT_MAX;
    DwmExtendFrameIntoClientArea(this->window, &m);

    // It is possible that we have a parent set. This will either be "VirtualDesktop", or "Monitor[0-9]+"
    TCHAR parent[MAX_RCCOMMAND];
    mSettings->GetString(L"Parent", parent, _countof(parent), L"");
    {
        if (_wcsnicmp(parent, L"Monitor", _countof(L"Monitor") - 1) == 0)
        {
        }
        else // VirtualDesktop
        {
        }
    }
}


/// <summary>
/// Constructor used by CreateChild to create a child window.
/// </summary>
/// <param name="parent">The parent of this window.</param>
/// <param name="settings">The settings to use.</param>
/// <param name="msgHandler">The default message handler for this window.</param>
Window::Window(Window* parent, Settings* settings, MessageHandler* msgHandler)
    : Window(new Settings(settings), msgHandler)
{
    mParent = parent;
    mIsChild = true;
    this->window = parent->window;
}


/// <summary>
/// Destroys all children and frees allocated resources.
/// </summary>
Window::~Window() {
  this->initialized = false;
  if (mParent) {
    mParent->RemoveChild(this);
  } else if (mIsChild) {
    nCore::System::RemoveWindowRegistrationListener(mParentName, this);
  }

  // Clear all update locks, to prevent crashes
  for (UpdateLock *lock : mActiveLocks) {
    lock->mLocked = false;
  }

  // Register with the core
  if (mWindowSettings.registerWithCore) {
    nCore::System::UnRegisterWindow(mSettings->GetPrefix());
  }

  if (!mIsChild && this->window) {
    DestroyWindow(this->window);
  }

  SAFERELEASE(this->parsedText);

  DiscardDeviceResources();

  // Delete all overlays
  ClearOverlays();

  // Let the children know that we are vanishing
  for (Window *child : this->children) {
    child->ParentLeft();
  }

  if (!mIsChild) {
    SAFERELEASE(mRenderTarget);
    SAFEDELETE(this->timerIDs);
    SAFEDELETE(this->userMsgIDs);
  }

  SAFEDELETE(mWindowData);
  SAFEDELETE(mSettings);
  free((LPVOID)this->text);
}


/// <summary>
/// Adds a brush owner.
/// </summary>
void Window::AddBrushOwner(IBrushOwner *owner, LPCTSTR name) {
  mBrushOwners[name] = owner;
}


/// <summary>
/// Adds an overlay icon.
/// </summary>
/// <param name="position">Where to place the overlay, relative to the parent.</param>
/// <param name="icon">The icon to use as an overlay.</param>
/// <returns>An object which can be used to modify/remove this overlay.</returns>
Window::OVERLAY Window::AddOverlay(D2D1_RECT_F position, HICON icon, int zOrder) {
  IWICBitmap *source = nullptr;
  IWICImagingFactory *factory = nullptr;

  Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));

  // Generate a WIC bitmap and call the overloaded AddOverlay function
  factory->CreateBitmapFromHICON(icon, &source);
  return AddOverlay(position, source, zOrder);
}


/// <summary>
/// Adds an overlay image.
/// </summary>
/// <param name="position">Where to place the overlay, relative to the parent.</param>
/// <param name="bitmap">The bitmap to use as an overlay.</param>
/// <returns>An object which can be used to modify/remove this overlay.</returns>
Window::OVERLAY Window::AddOverlay(D2D1_RECT_F position, HBITMAP bitmap, int zOrder) {
  IWICBitmap *source = nullptr;
  IWICImagingFactory *factory = nullptr;

  Factories::GetWICFactory(reinterpret_cast<LPVOID*>(&factory));

  // Generate a WIC bitmap and call the overloaded AddOverlay function
  factory->CreateBitmapFromHBITMAP(bitmap, nullptr, WICBitmapUseAlpha, &source);
  return AddOverlay(position, source, zOrder);
}


/// <summary>
/// Adds an overlay image.
/// </summary>
/// <param name="position">Where to place the overlay, relative to the parent.</param>
/// <param name="source">The bitmap to use as an overlay.</param>
/// <returns>An object which can be used to modify/remove this overlay.</returns>
Window::OVERLAY Window::AddOverlay(D2D1_RECT_F position, IWICBitmapSource *source, int zOrder) {
  Overlay *overlay = new Overlay(position, this->drawingArea, source, zOrder);
  overlay->ReCreateDeviceResources(mRenderTarget);

  OVERLAY iter;
  for (iter = this->overlays.begin(); iter != this->overlays.end() && iter->GetZOrder() < overlay->GetZOrder(); ++iter);
  return this->overlays.insert(iter.mIter, overlay);
}


/// <summary>
/// Adds a custom painter which is called after children and overlays.
/// </summary>
/// <param name="painter">The painter.</param>
/// <returns>An object which can be used to modify/remove this painter.</returns>
Window::PAINTER Window::AddPostPainter(IPainter* painter) {
  PAINTER ret = this->postPainters.insert(this->postPainters.end(), painter);
  ret->ReCreateDeviceResources(mRenderTarget);
  ret->UpdatePosition(this->drawingArea);

  return ret;
}


/// <summary>
/// Adds a custom painter which is called before children and overlays.
/// </summary>
/// <param name="painter">The painter.</param>
/// <returns>An object which can be used to modify/remove this painter.</returns>
Window::PAINTER Window::AddPrePainter(IPainter* painter)
{
    PAINTER ret = this->prePainters.insert(this->prePainters.end(), painter);
    ret->ReCreateDeviceResources(mRenderTarget);
    ret->UpdatePosition(this->drawingArea);

    return ret;
}


/// <summary>
/// Performs an animation step.
/// </summary>
void Window::Animate() {
  float progress = Easing::Transform(Clamp(0.0f, mAnimationClock.GetTime() / mAnimationDuration, 1.0f), mAnimationEasing);

  if (progress >= 1.0f) {
    mAnimating = false;
  }

  Rect step;
  step.left = mAnimationStart.left + (mAnimationTarget.left - mAnimationStart.left)*progress;
  step.top = mAnimationStart.top + (mAnimationTarget.top - mAnimationStart.top)*progress;
  step.right = mAnimationStart.right + (mAnimationTarget.right - mAnimationStart.right)*progress;
  step.bottom = mAnimationStart.bottom + (mAnimationTarget.bottom - mAnimationStart.bottom)*progress;

  SetPosition(step.left, step.top, step.right - step.left, step.bottom - step.top);
}


/// <summary>
/// Clears the specified callback timer, stoping the timer and unregistering the timer ID from the handler that owns it.
/// </summary>
/// <param name="timer">The ID of the timer to clear.</param>
void Window::ClearCallbackTimer(UINT_PTR timer)
{
    if (!mIsChild)
    {
        KillTimer(this->window, timer);
        this->timers.erase(timer);
        this->timerIDs->ReleaseID(timer);
    }
    else if(mParent)
    {
        mParent->ClearCallbackTimer(timer);
    }
}


/// <summary>
/// Removes all overlays from the window.
/// </summary>
void Window::ClearOverlays()
{
    for (Overlay *overlay : this->overlays)
    {
        delete overlay;
    }
    this->overlays.clear();
}


/// <summary>
/// Creates a child window.
/// </summary>
/// <param name="childSettings">The settings the child window should use.</param>
/// <param name="msgHandler">The default message handler for the child window.</param>
/// <returns>The child window.</returns>
Window *Window::CreateChild(Settings* childSettings, MessageHandler* msgHandler)
{
    Window* child = new Window(this, childSettings, msgHandler);
    children.push_back(child);
    return child;
}


/// <summary>
/// Discards all device dependent resources.
/// </summary>
void Window::DiscardDeviceResources()
{
    if (!mIsChild)
    {
        SAFERELEASE(mRenderTarget);
    }
    else
    {
        mRenderTarget = nullptr;
    }

    for (IPainter *painter : this->prePainters)
    {
        painter->DiscardDeviceResources();
    }
    for (Overlay *overlay : this->overlays)
    {
        overlay->DiscardDeviceResources();
    }
    mStateRender->DiscardDeviceResources();
    for (IPainter *painter : this->postPainters)
    {
        painter->DiscardDeviceResources();
    }

    // Discard resources for all children as well.
    for (Window *child : this->children)
    {
        child->DiscardDeviceResources();
    }
}


/// <summary>
/// Disables forwarding of mouse events to children.
/// </summary>
void Window::DisableMouseForwarding()
{
    mDontForwardMouse = true;
}


/// <summary>
/// Enables forwarding of mouse events to children.
/// </summary>
void Window::EnableMouseForwarding()
{
    mDontForwardMouse = false;
}


/// <summary>
/// Should be called when a fullscreen window has
/// </summary>
void Window::FullscreenActivated(HMONITOR monitor, HWND fullscreenWindow)
{
    if (!mIsChild && IsVisible())
    {
        if (MonitorFromWindow(this->window, MONITOR_DEFAULTTONULL) == monitor)
        {
            mCoveredByFullscreen = true;
            if (mWindowSettings.alwaysOnTop && visible)
            {
                SetWindowPos(this->window, fullscreenWindow, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
            }
        }
    }
}


/// <summary>
/// Enables forwarding of mouse events to children.
/// </summary>
void Window::FullscreenDeactivated(HMONITOR monitor)
{
    if (!mIsChild && mCoveredByFullscreen)
    {
        if (MonitorFromWindow(this->window, MONITOR_DEFAULTTONULL) == monitor)
        {
            mCoveredByFullscreen = false;
            if (mWindowSettings.alwaysOnTop)
            {
                SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
            }
        }
    }
}


IBrushOwner *Window::GetBrushOwner(LPCTSTR name)
{
    auto owner = mBrushOwners.find(name);
    if (owner != mBrushOwners.end())
    {
        return owner->second;
    }
    return GetState(name);
}


/// <summary>
/// Gets the "Desired" size of the window, given the specified constraints.
/// </summary>
/// <param name="maxWidth">Out. The maximum width to return.</param>
/// <param name="maxHeight">Out. The maximum height to return.</param>
/// <param name="size">Out. The desired size will be placed in this SIZE.</param>
void Window::GetDesiredSize(int maxWidth, int maxHeight, LPSIZE size)
{
    mStateRender->GetDesiredSize(maxWidth, maxHeight, size, this);
}


/// <summary>
/// Returns the drawing settings for the default state.
/// </summary>
/// <returns>The drawing settings for the default state.</returns>
WindowSettings* Window::GetDrawingSettings()
{
    return &mWindowSettings;
}


/// <summary>
///
/// </summary>
D2D1_RECT_F Window::GetDrawingRect()
{
    return this->drawingArea;
}


ID2D1RenderTarget *Window::GetRenderTarget()
{
    return mRenderTarget;
}


D2D1_POINT_2F const &Window::GetPosition() const
{
    return mPosition;
}


D2D1_SIZE_F const &Window::GetSize() const
{
    return mSize;
}


/// <summary>
/// Gets the screen position of the window.
/// </summary>
/// <param name="rect">Out. The screen position of this window will be placed in this rect.</param>
void Window::GetScreenRect(LPRECT rect)
{
    RECT r;
    GetWindowRect(this->window, &r);
    rect->left = r.left + (LONG)this->drawingArea.left;
    rect->top = r.top + (LONG)this->drawingArea.top;
    rect->right = r.left + (LONG)this->drawingArea.right;
    rect->bottom = r.top + (LONG)this->drawingArea.bottom;
}


/// <summary>
/// Returns the current text of this window.
/// </summary>
/// <returns>The current parsed text.</returns>
LPCWSTR Window::GetText()
{
    return this->text;
}


/// <summary>
/// Returns the window handle of the top-level window this window belongs to.
/// </summary>
/// <returns>the window handle.</returns>
HWND Window::GetWindowHandle()
{
    return this->window;
}


/// <summary>
/// Returns the specified state, if it exists.
/// </summary>
State *Window::GetState(LPCTSTR stateName)
{
    return mStateRender->GetState(stateName);
}


/// <summary>
/// Returns the data states use to render to this window.
/// </summary>
IStateWindowData *Window::GetWindowData()
{
    return mWindowData;
}


/// <summary>
/// Handles window messages for this drawablewindow. Any messages forwarded from here will have the extra parameter set to this.
/// </summary>
/// <param name="window">The handle of the window this message was sent to.</param>
/// <param name="msg">The message.</param>
/// <param name="wParam">Message data.</param>
/// <param name="lParam">Message data</param>
/// <param name="extra">Message data. Not used.</param>
/// <returns>Something</returns>
LRESULT WINAPI Window::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID extra)
{
    UNREFERENCED_PARAMETER(extra);

    // Forward mouse messages to the lowest level child window which the mouse is over.
    if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST && !mDontForwardMouse)
    {
        UpdateLock updateLock(this);

        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);
        MessageHandler *handler = nullptr;

        if (mCaptureHandler == nullptr)
        {
            for (Window *child : this->children)
            {
                if (!child->mWindowSettings.clickThrough)
                {
                    D2D1_RECT_F pos = child->drawingArea;
                    if (xPos >= pos.left && xPos <= pos.right && yPos >= pos.top && yPos <= pos.bottom)
                    {
                        handler = child;
                        break;
                    }
                }
            }

            if (msg == WM_MOUSEMOVE)
            {
                if (!mIsChild && !isTrackingMouse)
                {
                    isTrackingMouse = true;
                    TrackMouseEvent(&this->trackMouseStruct);
                }
                if (handler != activeChild)
                {
                    if (activeChild != nullptr)
                    {
                        activeChild->HandleMessage(window, WM_MOUSELEAVE, 0, 0, this);
                    }
                    else
                    {
                        this->msgHandler->HandleMessage(window, WM_MOUSEMOVE, wParam, lParam, this);
                    }
                    activeChild = (Window*)handler;
                }
            }
        }
        else
        {
            handler = mCaptureHandler;
        }

        if (handler == nullptr)
        {
            handler = this->msgHandler;
        }

        // Let our messagehandler deal with it.
        return handler->HandleMessage(window, msg, wParam, lParam, this);
    }

    // Forward keyboard messages to the active child
    if (msg >= WM_KEYFIRST && msg <= WM_KEYLAST)
    {
        UpdateLock updateLock(this);
        if (activeChild != nullptr)
        {
            activeChild->HandleMessage(window, msg, wParam, lParam, this);
        }
    }

    // Handle DrawableWindow messages.
    switch (msg)
    {
    case WM_MOUSELEAVE:
        {
            UpdateLock updateLock(this);
            isTrackingMouse = false;
            if (activeChild != nullptr)
            {
                activeChild->HandleMessage(window, WM_MOUSELEAVE, 0, 0, this);
                activeChild = nullptr;
            }
        }
        break;

    case WM_ERASEBKGND:
        {
        }
        return 1;

    case WM_PAINT:
        {
            bool inAnimation = false;
            RECT updateRect;

            UpdateLock lock(this);

            if (GetUpdateRect(window, &updateRect, FALSE) != FALSE)
            {
                if (ReCreateDeviceResources() == S_OK)
                {
                    D2D1_RECT_F d2dUpdateRect = D2D1::RectF(
                        (FLOAT)updateRect.left, (FLOAT)updateRect.top, (FLOAT)updateRect.right, (FLOAT)updateRect.bottom);

                    mRenderTarget->BeginDraw();
                    mRenderTarget->PushAxisAlignedClip(&d2dUpdateRect, D2D1_ANTIALIAS_MODE_ALIASED);
                    mRenderTarget->Clear();

                    Paint(inAnimation, &d2dUpdateRect);

                    mRenderTarget->PopAxisAlignedClip();

                    // If EndDraw fails we need to recreate all device-dependent resources
                    if (mRenderTarget->EndDraw() == D2DERR_RECREATE_TARGET)
                    {
                        DiscardDeviceResources();
                    }
                    else
                    {
                        ValidateRect(this->window, nullptr);
                    }
                }

                // Paint actual owned/child windows.
                //EnumChildWindows(this->window, [] (HWND hwnd, LPARAM) -> BOOL
                //{
                //    SendMessage(hwnd, WM_PAINT, 0, 0);
                //    return TRUE;
                //}, 0);

                // Handled specially here for top-level windows, as we must NOT resize
                // the top-level window between BeginPaint and EndPaint.
                if (mAnimating)
                {
                    Animate();
                }
            }

            // We just painted, don't update
            mNeedsUpdate = false;

            if (inAnimation)
            {
                PostMessage(window, WM_ANIMATIONPAINT, 0, 0);
            }
        }
        return 0;

    case WM_TIMER:
        {
            UpdateLock updateLock(this);
            map<UINT_PTR, MessageHandler*>::const_iterator iter = timers.find(wParam);
            if (iter != timers.end())
            {
                return iter->second->HandleMessage(window, msg, wParam, lParam, this);
            }
        }
        return 0;

    case WM_DWMCOLORIZATIONCOLORCHANGED:
        {
            UpdateLock updateLock(this);

            // When the intensity is really high, the alpha drops to 0 :/
            if (wParam >> 24 == 0 && wParam != 0)
            {
                wParam |= 0xFF000000;
            }

            if (UpdateDWMColor(ARGB(wParam)))
            {
                UpdateWindow(this->window);
            }
        }
        return 0;

    case WM_WINDOWPOSCHANGING:
        {
            if (mWindowSettings.alwaysOnTop)
            {
                LPWINDOWPOS windowPos = LPWINDOWPOS(lParam);
                if (!mCoveredByFullscreen)
                {
                    windowPos->hwndInsertAfter = HWND_TOPMOST;
                }
            }
        }
        return 0;

    case WM_ANIMATIONPAINT:
        {
            //PostMessage(window, WM_PAINT, 0, 0);
        }
        return 0;
    }

    // Forward registered user messages.
    if (msg >= WM_FIRSTREGISTERED)
    {
        UpdateLock updateLock(this);
        map<UINT,MessageHandler*>::const_iterator handler = this->userMessages.find(msg);
        if (handler != this->userMessages.end())
        {
            return handler->second->HandleMessage(window, msg, wParam, lParam, this);
        }
    }

    // Let the default messagehandler deal with anything else, if it is initialized.
    if (this->msgHandler && this->msgHandler->mInitialized)
    {
        return this->msgHandler->HandleMessage(window, msg, wParam, lParam, this);
    }
    else
    {
        return DefWindowProc(window, msg, wParam, lParam);
    }
}


/// <summary>
/// Hides the window.
/// </summary>
void Window::Hide()
{
    this->visible = false;
    if (!mIsChild)
    {
        ShowWindow(this->window, SW_HIDE);
        PostMessage(this->window, WM_HIDDEN, 0, 0);
    }
    else
    {
        RECT r = { (LONG)drawingArea.left, (LONG)drawingArea.top, (LONG)drawingArea.right, (LONG)drawingArea.bottom };
        mParent->Repaint(&r);
    }
}


/// <summary>
/// Initializes this window.
/// </summary>
void Window::Initialize(WindowSettings &windowSettings, IStateRender *stateRender)
{
    mWindowSettings = windowSettings;

    // Load the base state
    mStateRender = stateRender;
    mWindowData = mStateRender->CreateWindowData(this);

    // Register with the core.
    if (mWindowSettings.registerWithCore)
    {
        nCore::System::RegisterWindow(mSettings->GetPrefix(), this);
    }

    // Put the window in its correct position.
    SetPosition(mWindowSettings.x, mWindowSettings.y,
        mWindowSettings.width, mWindowSettings.height);

    // Create D2D resources.
    ReCreateDeviceResources();

    // AlwaysOnTop
    if (!mIsChild && mWindowSettings.alwaysOnTop)
    {
        ::SetParent(this->window, nullptr);
        SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
    }

    // Set the text.
    SetText(mWindowSettings.text);

    this->initialized = true;
}


/// <summary>
/// Check if this window is currently visible.
/// </summary>
/// <returns>True if this window and all its ancestors are visible.</returns>
bool Window::IsChild()
{
    return mIsChild;
}


/// <summary>
/// Check if this window is currently visible.
/// </summary>
/// <returns>True if this window and all its ancestors are visible.</returns>
bool Window::IsVisible()
{
    if (mParent)
    {
        return this->visible && mParent->IsVisible();
    }
    return this->visible;
}


/// <summary>
/// Moves the window.
/// </summary>
/// <param name="x">The x coordinate to move the window to. Relative to the parent.</param>
/// <param name="y">The y coordinate to move the window to. Relative to the parent.</param>
void Window::Move(float x, float y)
{
    SetPosition(x, y, mSize.width, mSize.height);
}


/// <summary>
/// Moves the window.
/// </summary>
/// <param name="x">The x coordinate to move the window to. Relative to the parent.</param>
/// <param name="y">The y coordinate to move the window to. Relative to the parent.</param>
void Window::Move(Distance x, Distance y)
{
    SetPosition(x, y, mWindowSettings.width, mWindowSettings.height);
}


/// <summary>
/// Removes the specified child.
/// </summary>
void Window::Paint(bool &inAnimation, D2D1_RECT_F *updateRect)
{
    UpdateLock lock(this);
    if (this->visible && RectIntersectArea(updateRect, &this->drawingArea) > 0)
    {
        mRenderTarget->PushAxisAlignedClip(this->drawingArea, D2D1_ANTIALIAS_MODE_ALIASED);

        // Paint the active state's background.
        mStateRender->Paint(mRenderTarget, mWindowData);

        // Pre painters.
        for (IPainter *painter : this->prePainters)
        {
            painter->Paint(mRenderTarget);
        }

        // Paint the active state's text.
        mStateRender->PaintText(mRenderTarget, mWindowData);

        // Paint all overlays.
        PaintOverlays(updateRect);

        // Paint all children.
        PaintChildren(inAnimation, updateRect);

        // Post painters.
        for (IPainter *painter : this->postPainters)
        {
            painter->Paint(mRenderTarget);
        }

        inAnimation |= mAnimating;
        if (mAnimating && mIsChild)
        {
            Animate();
        }

        mRenderTarget->PopAxisAlignedClip();
    }
}


/// <summary>
/// Paints all child windows.
/// </summary>
void Window::PaintChildren(bool &inAnimation, D2D1_RECT_F *updateRect)
{
    for (Window *child : this->children)
    {
        child->Paint(inAnimation, updateRect);
    }
}


/// <summary>
/// Paints all overlays.
/// </summary>
void Window::PaintOverlays(D2D1_RECT_F *updateRect)
{
    UNREFERENCED_PARAMETER(updateRect);

    for (Overlay *overlay : this->overlays)
    {
        overlay->Paint(mRenderTarget);
    }
}


/// <summary>
/// Called by the parent when it is passing away.
/// </summary>
void Window::ParentLeft()
{
    mParent = nullptr;
    UpdateParentVariables();
    SendToAll(nullptr, WM_TOPPARENTLOST, 0, 0, this);

    if (*mParentName != '\0')
    {
        nCore::System::AddWindowRegistrationListener(mParentName, this);
    }
}


/// <summary>
/// Registers an user message (>= WM_USER) which will be forwarded to the specified handler.
/// </summary>
/// <param name="msgHandler">The handler which will receive the message.</param>
/// <returns>The assigned message ID.</returns>
UINT Window::RegisterUserMessage(MessageHandler* msgHandler)
{
    if (!mIsChild)
    {
        UINT ret = this->userMsgIDs->GetNewID();
        this->userMessages.insert(std::pair<UINT, MessageHandler*>(ret, msgHandler));
        return ret;
    }
    else if (mParent)
    {
        return mParent->RegisterUserMessage(msgHandler);
    }
    else
    {
        TRACE("RegisterUserMessage failed!");
        return 0;
    }
}


/// <summary>
/// Releases a user message. It will no longer be forwarded to the specified handler if received.
/// </summary>
/// <param name="message">The ID of the message to release.</param>
void Window::ReleaseUserMessage(UINT message)
{
    if (!mIsChild)
    {
        this->userMessages.erase(message);
        this->userMsgIDs->ReleaseID(message);
    }
    else if (mParent)
    {
        mParent->ReleaseUserMessage(message);
    }
}


/// <summary>
/// Resize the window.
/// </summary>
/// <param name="width">The width to resize the window to.</param>
/// <param name="height">The height to resize the window to.</param>
void Window::Resize(float width, float height)
{
    SetPosition(mPosition.x, mPosition.y, width, height);
}


/// <summary>
/// Resize the window.
/// </summary>
/// <param name="width">The width to resize the window to.</param>
/// <param name="height">The height to resize the window to.</param>
void Window::Resize(Distance width, Distance height)
{
    SetPosition(mWindowSettings.x, mWindowSettings.y, width, height);
}


/// <summary>
/// (Re)Creates all device-dependent resources.
/// </summary>
/// <returns>S_OK if successful, an error code otherwise.</returns>
HRESULT Window::ReCreateDeviceResources()
{
    HRESULT hr = S_OK;

    if (!mRenderTarget)
    {
        if (!mIsChild)
        {
            ID2D1Factory *pD2DFactory = nullptr;
            hr = Factories::GetD2DFactory(reinterpret_cast<LPVOID*>(&pD2DFactory));

            // Create the render target
            if (SUCCEEDED(hr))
            {
                hr = pD2DFactory->CreateHwndRenderTarget(
                    D2D1::RenderTargetProperties(
                        D2D1_RENDER_TARGET_TYPE_DEFAULT,
                        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                        96.0f,
                        96.0f
                    ),
                    D2D1::HwndRenderTargetProperties(this->window, D2D1::SizeU((UINT32)mSize.width, (UINT32)mSize.height)),
                    &mRenderTarget
                );
                if (SUCCEEDED(hr))
                {
                    mRenderTarget->SetTextAntialiasMode(mWindowSettings.textAntiAliasMode);
                }
            }
        }
        else
        {
            if (!mParent) // We are a child with a missing parent
            {
                return S_FALSE;
            }
            if (!mParent->mRenderTarget) // We are a child of a parent who either failed to create the render target, or is missing its parent.
            {
                return S_FALSE;
            }
            mRenderTarget = mParent->mRenderTarget;
        }

        if (SUCCEEDED(hr))
        {
            for (IPainter *painter : this->prePainters)
            {
                RETURNONFAIL(hr, painter->ReCreateDeviceResources(mRenderTarget));
            }

            RETURNONFAIL(hr, mStateRender->ReCreateDeviceResources(mRenderTarget));
            mStateRender->UpdatePosition(this->drawingArea, mWindowData);

            for (Overlay *overlay : this->overlays)
            {
                RETURNONFAIL(hr, overlay->ReCreateDeviceResources(mRenderTarget));
            }

            for (IPainter *painter : this->postPainters)
            {
                RETURNONFAIL(hr, painter->ReCreateDeviceResources(mRenderTarget));
            }

            // Recreate resources for all children as well.
            for (Window *child : this->children)
            {
                RETURNONFAIL(hr, child->ReCreateDeviceResources());
            }
        }
    }

    return hr;
}


/// <summary>
/// Releases a SetMouseCapture
/// </summary>
void Window::PopUpdateLock(UpdateLock *lock)
{
    if (!mIsChild)
    {
        mActiveLocks.erase(lock);
        if (mActiveLocks.empty() && mNeedsUpdate)
        {
            mNeedsUpdate = false;
            UpdateWindow(GetWindowHandle());
        }
    }
    else if (mParent)
    {
        mParent->PopUpdateLock(lock);
    }
}


/// <summary>
/// Releases a SetMouseCapture
/// </summary>
void Window::PushUpdateLock(UpdateLock *lock)
{
    if (!mIsChild)
    {
        mActiveLocks.insert(lock);
        lock->mWindow = this;
    }
    else if (mParent)
    {
        mParent->PushUpdateLock(lock);
    }
}


/// <summary>
/// Releases a SetMouseCapture
/// </summary>
void Window::ReleaseMouseCapture()
{
    if (!mIsChild)
    {
        ReleaseCapture();
        this->mCaptureHandler = nullptr;
    }
    else if(mParent)
    {
        mParent->ReleaseMouseCapture();
    }
}


/// <summary>
/// Removes the specified child.
/// </summary>
/// <param name="child">The child to remove.</param>
void Window::RemoveChild(Window *child)
{
    this->children.remove(child);
    if (child == this->activeChild)
    {
        this->activeChild = nullptr;
    }
}


/// <summary>
/// Removes the specified overlay.
/// </summary>
/// <param name="overlay">The overlay to remove.</param>
void Window::RemoveOverlay(OVERLAY overlay)
{
    if (overlay.mValid)
    {
        delete *overlay.mIter;
        this->overlays.erase(overlay.mIter);
    }
}


/// <summary>
/// Repaints the window.
/// </summary>
/// <param name="region">The area of the window to repaint. If NULL, the whole window is repainted.</param>
void Window::Repaint(LPCRECT region)
{
    if (this->initialized && this->visible)
    {
        if (mIsChild)
        {
            if (mParent != nullptr)
            {
                if (region != nullptr)
                {
                    mParent->Repaint(region);
                }
                else
                {
                    RECT r = { (LONG)drawingArea.left, (LONG)drawingArea.top, (LONG)drawingArea.right, (LONG)drawingArea.bottom };
                    mParent->Repaint(&r);
                }
            }
        }
        else {
            InvalidateRect(this->window, region, TRUE);
            if (mActiveLocks.empty())
            {
                UpdateWindow(this->window);
            }
            else
            {
                mNeedsUpdate = true;
            }
        }
    }
}


/// <summary>
/// Repaints the window.
/// </summary>
/// <param name="region">The area of the window to repaint. If NULL, the whole window is repainted.</param>
void Window::Repaint(const D2D1_RECT_F *region)
{
    if (this->initialized && this->visible)
    {
        RECT r;
        if (region == nullptr)
        {
            region = &this->drawingArea;
        }

        r.left = (LONG)(region->left - 1.5f);
        r.top = (LONG)(region->top - 1.5f);
        r.bottom = (LONG)(region->bottom + 1.5f);
        r.right = (LONG)(region->right + 1.5f);

        Repaint(&r);
    }
}


/// <summary>
/// Modifies the AlwaysOnTop setting
/// </summary>
void Window::SetAlwaysOnTop(bool value)
{
    bool oldValue = mWindowSettings.alwaysOnTop;
    mWindowSettings.alwaysOnTop = value;
    if (!mIsChild && !mCoveredByFullscreen)
    {
        if (value)
        {
            SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
        }
        else if (oldValue)
        {
            SetWindowPos(this->window, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
        }
    }
}


/// <summary>
/// Starts a new animation, or updates the parameters of the current one.
/// </summary>
/// <param name="x">The x coordinate to animate to.</param>
/// <param name="y">The y coordinate to animate to.</param>
/// <param name="width">The width to animate to.</param>
/// <param name="height">The height to animate to.</param>
/// <param name="duration">The number of milliseconds to complete the animation in.</param>
/// <param name="easing">The easing to use.</param>
void Window::SetAnimation(Distance x, Distance y, Distance width, Distance height, int duration, Easing::Type easing)
{
    mAnimationTarget = Rect(x, y, x + width, y + height);
    mAnimationStart = Rect(mWindowSettings.x, mWindowSettings.y, mWindowSettings.x + mWindowSettings.width, mWindowSettings.y + mWindowSettings.height);
    mAnimationEasing = easing;
    mAnimationClock.Clock();
    mAnimationDuration = duration / 1000.0f;
    mAnimating = true;

    Repaint();
}


/// <summary>
/// Creates a new timer which is forwarded to the specified handler.
/// </summary>
/// <param name="elapse">The uElapse parameter of SetTimer.</param>
/// <param name="msgHandler">The handler WM_TIMER messags with this ID are sent to.</param>
/// <returns>The assigned timer ID.</returns>
UINT_PTR Window::SetCallbackTimer(UINT elapse, MessageHandler* msgHandler)
{
    if (!mIsChild)
    {
        UINT_PTR ret = SetTimer(this->window, this->timerIDs->GetNewID(), elapse, NULL);
        this->timers.insert(std::pair<UINT_PTR, MessageHandler*>(ret, msgHandler));
        return ret;
    }
    else if (mParent)
    {
        return mParent->SetCallbackTimer(elapse, msgHandler);
    }
    else
    {
        TRACE("SetCallbackTimer failed!");
        return 0;
    }
}


/// <summary>
/// Modifies the ClickThrough setting
/// </summary>
void Window::SetClickThrough(bool value)
{
    mWindowSettings.clickThrough = value;
}


/// <summary>
/// Redirects input to the selected message handler, regardless of where the mouse is.
/// </summary>
void Window::SetMouseCapture(MessageHandler *captureHandler)
{
    if (!mIsChild)
    {
        SetCapture(this->window);
        this->mCaptureHandler = captureHandler;
    }
    else if (mParent) { // Just ignore this request if we are a child without a parent.
        mParent->SetMouseCapture(captureHandler == nullptr ? this : captureHandler);
    }
}


/// <summary>
/// Moves and resizes the window.
/// </summary>
/// <param name="rect">The new position of the window.</param>
void Window::SetPosition(RECT rect)
{
    SetPosition((float)rect.left, (float)rect.top, (float)(rect.right - rect.left), (float)(rect.bottom - rect.top));
}


/// <summary>
/// Moves and resizes the window.
/// </summary>
/// <param name="rect">The new position of the window.</param>
void Window::SetPosition(D2D1_RECT_F rect)
{
    SetPosition(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}


/// <summary>
/// Send the specified message to all children, all the way down the tree.
/// </summary>
void Window::SendToAll(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID data)
{
    this->msgHandler->HandleMessage(window, msg, wParam, lParam, data);
    for (Window *child : this->children)
    {
        child->SendToAll(window, msg, wParam, lParam, data);
    }
}

// Called when the DWM color has changed. Windows should invalidate
// if appropriate, but not update, in response to this.
bool Window::UpdateDWMColor(ARGB newColor)
{
    bool ret = false;

    // It's important that ret is on the right hand side, to prevent short-circuiting
    for (IPainter *painter : this->prePainters)
    {
        ret = painter->UpdateDWMColor(newColor, mRenderTarget) || ret;
    }

    ret = mStateRender->UpdateDWMColor(newColor, mRenderTarget) || ret;

    for (IPainter *painter : this->postPainters)
    {
        ret = painter->UpdateDWMColor(newColor, mRenderTarget) || ret;
    }

    if (ret)
    {
        RECT r = { (LONG)drawingArea.left, (LONG)drawingArea.top, (LONG)drawingArea.right, (LONG)drawingArea.bottom };
        InvalidateRect(this->window, &r, TRUE);
    }

    for (Window *child : this->children)
    {
        ret = child->UpdateDWMColor(newColor) || ret;
    }

    return ret;
}


/// <summary>
/// Updates variables which are dependent on the parent window.
/// </summary>
void Window::UpdateParentVariables()
{
    if (mParent)
    {
        this->window = mParent->window;
    }
    else
    {
        this->window = nullptr;
    }

    for (Window *child : this->children)
    {
        child->UpdateParentVariables();
    }
}


/// <summary>
/// Specifies a new parent for this child.
/// </summary>
void Window::SetParent(Window *newParent)
{
    ASSERT(mParent == nullptr);

    mParent = newParent;
    mParent->children.push_back(this);

    UpdateParentVariables();
    SendToAll(this->window, WM_NEWTOPPARENT, 0, 0, this);

    SetPosition(mWindowSettings.x, mWindowSettings.y,
        mWindowSettings.width, mWindowSettings.height);
    ReCreateDeviceResources();
    Repaint();
}


/// <summary>
/// Moves and resizes the window.
/// </summary>
/// <param name="x">The x coordinate to move the window to. Relative to the parent.</param>
/// <param name="y">The y coordinate to move the window to. Relative to the parent.</param>
/// <param name="width">The width to resize the window to.</param>
/// <param name="height">The height to resize the window to.</param>
void Window::SetPosition(float x, float y, float width, float height, LPARAM extra) {
  SetPosition(Distance(x), Distance(y), Distance(width), Distance(height), extra);
}


/// <summary>
/// Moves and resizes the window.
/// </summary>
/// <param name="x">The x coordinate to move the window to. Relative to the parent.</param>
/// <param name="y">The y coordinate to move the window to. Relative to the parent.</param>
/// <param name="width">The width to resize the window to.</param>
/// <param name="height">The height to resize the window to.</param>
void Window::SetPosition(Distance x, Distance y, Distance width, Distance height, LPARAM extra) {
  UpdateLock lock(this);

  // Invalidate the current area.
  Repaint();

  // Update the drawing settings.
  mWindowSettings.x = x;
  mWindowSettings.y = y;
  mWindowSettings.width = width;
  mWindowSettings.height = height;

  // Determine the width and height of the parent
  D2D1_SIZE_F parentSize = D2D1::SizeF(0, 0);
  if (!mIsChild) {
    // TODO(Erik): This should be based on the size of the monitor the window is on.
    MonitorInfo &monitorInfo = nCore::FetchMonitorInfo();
    const MonitorInfo::Monitor &primaryMonitor = monitorInfo.GetMonitor(0);
    parentSize = D2D1::SizeF((FLOAT)primaryMonitor.width, (FLOAT)primaryMonitor.height);
  } else if (mParent) {
    parentSize = mParent->mSize;
  }

  D2D1_SIZE_F newSize = D2D1::SizeF(width.Evaluate(parentSize.width), height.Evaluate(parentSize.height));
  D2D1_POINT_2F newPosition = D2D1::Point2F(x.Evaluate(parentSize.width), y.Evaluate(parentSize.height));

  //
  bool isResize = newSize.height != mSize.height || newSize.width != mSize.width;
  bool isMove = newPosition.x != mPosition.x || newPosition.y != mPosition.y;

  // Update mSize and mPosition
  mSize = newSize;
  mPosition = newPosition;

  // Position the window and/or set the backarea.
  if (!mIsChild) {
    SetWindowPos(this->window, 0, int(mPosition.x + 0.5f), int(mPosition.y + 0.5f), int(mSize.width + 0.5f), int(mSize.height + 0.5f), SWP_NOZORDER | SWP_NOACTIVATE);
    this->drawingArea = D2D1::RectF(0, 0, mSize.width, mSize.height);
    if (mRenderTarget) {
      VERIFY_HR(mRenderTarget->Resize(D2D1::SizeU(UINT32(mSize.width + 0.5f), UINT32(mSize.height + 0.5f))));
    }
  } else if(mParent) {
    this->drawingArea = D2D1::RectF(
      mParent->drawingArea.left + mPosition.x,
      mParent->drawingArea.top + mPosition.y,
      mParent->drawingArea.left + mPosition.x + mSize.width,
      mParent->drawingArea.top + mPosition.y + mSize.height
    );
  }

  // Update all paintables.
  mStateRender->UpdatePosition(this->drawingArea, mWindowData);
  for (Overlay *overlay : this->overlays) {
    overlay->UpdatePosition(this->drawingArea);
  }
  for (IPainter *painter : this->prePainters) {
    painter->UpdatePosition(this->drawingArea);
  }
  for (IPainter *painter : this->postPainters) {
    painter->UpdatePosition(this->drawingArea);
  }
  if (isResize || mIsChild) {
    for (Window *child : this->children) {
      child->Move(child->mWindowSettings.x, child->mWindowSettings.y);
    }
  }

  // Invalidate the new area.
  Repaint();

  //
  if (isResize) {
    this->msgHandler->HandleMessage(GetWindowHandle(), WM_SIZECHANGE, MAKEWPARAM(mSize.width, mSize.height), extra, this);
  }

  //
  if (isMove) {
    this->msgHandler->HandleMessage(GetWindowHandle(), WM_POSITIONCHANGE, MAKEWPARAM(mPosition.x, mPosition.y), extra, this);
  }
}


/// <summary>
/// Shows the window.
/// </summary>
void Window::Show(int nCmdShow)
{
    if (!mIsChild)
    {
        ShowWindow(this->window, nCmdShow);
        if (mWindowSettings.alwaysOnTop)
        {
            SetWindowPos(this->window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
        }
    }
    this->visible = true;
}


/// <summary>
/// Sizes the window to fit its current text.
/// </summary>
/// <param name="maxWidth">The maximum width to size the window to.</param>
/// <param name="maxHeight">The maximum height to size the window to.</param>
/// <param name="minWidth">The minimum width to size the window to.</param>
/// <param name="minHeight">The minimum height to size the window to.</param>
void Window::SizeToText(int maxWidth, int maxHeight, int minWidth, int minHeight)
{
    SIZE s;
    GetDesiredSize(maxWidth, maxHeight, &s);
    s.cx = std::max(s.cx, (long)minWidth);
    s.cy = std::max(s.cy, (long)minHeight);
    this->SetPosition(mPosition.x, mPosition.y, (float)s.cx, (float)s.cy);
}


/// <summary>
/// Sets the text for this window.
/// </summary>
/// <param name="text">The text for this window.</param>
void Window::SetText(LPCWSTR text)
{
    if (mWindowSettings.evaluateText)
    {
        SAFEDELETE(this->parsedText);
        this->parsedText = (IParsedText*)nCore::System::ParseText(text);
        this->parsedText->SetChangeHandler(TextChangeHandler, this);
        UpdateText();
    }
    else
    {
        this->text = StringUtils::ReallocOverwrite(const_cast<LPWSTR>(this->text), text);
        mStateRender->UpdateText(mWindowData);
    }
}


/// <summary>
/// Sets the text offsets for all states.
/// </summary>
/// <param name="left">The text offset from the left.</param>
/// <param name="top">The text offset from the top.</param>
/// <param name="right">The text offset from the right.</param>
/// <param name="bottom">The text offset from the bottom.</param>
void Window::SetTextOffsets(float left, float top, float right, float bottom)
{
    mStateRender->SetTextOffsets(left, top, right, bottom);
}


/// <summary>
/// Forcibly updates the text.
/// </summary>
void Window::UpdateText()
{
    if (mWindowSettings.evaluateText)
    {
        WCHAR buf[4096];
        this->parsedText->Evaluate(buf, 4096);
        this->text = StringUtils::ReallocOverwrite(const_cast<LPWSTR>(this->text), buf);
    }
    else
    {
        this->text = StringUtils::ReallocOverwrite(const_cast<LPWSTR>(this->text), mWindowSettings.text);
    }
    mStateRender->UpdateText(mWindowData);
    Repaint();
}
