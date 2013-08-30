/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskThumbnail.cpp
 *  The nModules Project
 *
 *  A single task in the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include "ThumbnailSettings.hpp"
#include "TaskThumbnail.hpp"
#include "../nShared/LSModule.hpp"
#include <strsafe.h>
#include <Shldisp.h>
#include "TaskSwitcher.hpp"
#include "../Utilities/StopWatch.hpp"
#include "../Utilities/Math.h"
#include <shellapi.h>

static HWND desktopWindow = nullptr;
static UINT (*DwmpActivateLivePreview)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown) = nullptr;

extern LSModule gLSModule;


TaskThumbnail::TaskThumbnail(
    Drawable* parent,
    HWND targetWindow,
    int x,
    int y,
    int width,
    int height,
    ThumbnailSettings& thumbnailSettings
)
    : Drawable(parent, L"Task")
    , mThumbnailSettings(thumbnailSettings)
    , mTargetWindow(targetWindow)
{
    if (DwmpActivateLivePreview == nullptr)
    {
        DwmpActivateLivePreview = (UINT (*)(UINT, HWND, HWND, UINT))GetProcAddress(GetModuleHandleW(L"DWMAPI.DLL"), (LPCSTR)0x71);
    }
    if (desktopWindow == nullptr)
    {
        desktopWindow = FindWindowW(L"DesktopBackgroundClass", nullptr);
    }

    mWindow->Initialize(mThumbnailSettings.mWindowSettings, &mThumbnailSettings.mStateRender);
    mWindow->SetPosition((float)x, (float)y, (float)width, (float)height);
    mWindow->Show();

    DwmRegisterThumbnail(mWindow->GetWindowHandle(), targetWindow, &mThumbnail);
    
    // 
    DWM_THUMBNAIL_PROPERTIES properties;
    properties.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY;
    properties.fSourceClientAreaOnly = FALSE;
    
    if (targetWindow == desktopWindow)
    {
        properties.dwFlags |= DWM_TNP_RECTSOURCE;
        MonitorInfo* monInfo = mWindow->GetMonitorInformation();
        properties.rcSource = monInfo->m_monitors[0].rect;
        properties.rcSource.bottom -= monInfo->m_virtualDesktop.rect.top;
        properties.rcSource.top -= monInfo->m_virtualDesktop.rect.top;
        properties.rcSource.left -= monInfo->m_virtualDesktop.rect.left;
        properties.rcSource.right -= monInfo->m_virtualDesktop.rect.left;
    }
    
    DwmUpdateThumbnailProperties(mThumbnail, &properties);

    //
    SIZE sourceSize;
    if (targetWindow == desktopWindow)
    {
        MonitorInfo* monInfo = mWindow->GetMonitorInformation();
        sourceSize.cx = monInfo->m_monitors[0].width;
        sourceSize.cy = monInfo->m_monitors[0].height;
    }
    else
    {
        DwmQueryThumbnailSourceSize(mThumbnail, &sourceSize);
    }

    //
    double scale = Math::min(
        (width - mThumbnailSettings.mThumbnailMargins.left - mThumbnailSettings.mThumbnailMargins.right)/(double)sourceSize.cx,
        (height - mThumbnailSettings.mThumbnailMargins.bottom - mThumbnailSettings.mThumbnailMargins.top)/(double)sourceSize.cy);
    sourceSize.cx = long(sourceSize.cx*scale);
    sourceSize.cy = long(sourceSize.cy*scale);

    int horizontalOffset = (width - mThumbnailSettings.mThumbnailMargins.left - mThumbnailSettings.mThumbnailMargins.right - sourceSize.cx)/2;
    int verticalOffset = (height - mThumbnailSettings.mThumbnailMargins.top - mThumbnailSettings.mThumbnailMargins.bottom - sourceSize.cy)/2;

    properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION;
    properties.fVisible = TRUE;
    properties.rcDestination.bottom = y + height - mThumbnailSettings.mThumbnailMargins.bottom - verticalOffset;
    properties.rcDestination.top = y + mThumbnailSettings.mThumbnailMargins.top + verticalOffset;
    properties.rcDestination.left = x + mThumbnailSettings.mThumbnailMargins.left + horizontalOffset;
    properties.rcDestination.right = x + width - mThumbnailSettings.mThumbnailMargins.right - horizontalOffset;

    DwmUpdateThumbnailProperties(mThumbnail, &properties);

    //
    this->iconOverlayWindow = gLSModule.CreateDrawableWindow(mSettings, this);

    StateRender<IconState>::InitData iconInitData;
    iconInitData[IconState::Base].defaults.brushSettings[::State::BrushType::Background].color = Color::Create(0x00000000);
    mIconStateRender.Load(iconInitData, mSettings);
    
    RECT r;
    mWindow->GetScreenRect(&r);
    this->iconOverlayWindow->Initialize(mThumbnailSettings.mIconWindowSettings, &mIconStateRender);
    this->iconOverlayWindow->SetPosition(float(r.right - 32 - horizontalOffset - mThumbnailSettings.mThumbnailMargins.right + 4),
        float(r.bottom - 32 - verticalOffset - mThumbnailSettings.mThumbnailMargins.bottom + 4), 32, 32);

    SetWindowPos(this->iconOverlayWindow->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetWindowPos(this->iconOverlayWindow->GetWindowHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    UpdateIcon();
}


void TaskThumbnail::ActivateState(State state)
{
    mThumbnailSettings.mStateRender.ActivateState(state, mWindow);
}


void TaskThumbnail::ClearState(State state)
{
    mThumbnailSettings.mStateRender.ClearState(state, mWindow);
}


void TaskThumbnail::UpdateIconPosition()
{
    RECT r;
    mWindow->GetScreenRect(&r);
    this->iconOverlayWindow->SetPosition((float)r.right - 32, (float)r.bottom - 32, 32, 32);

    SetWindowPos(this->iconOverlayWindow->GetWindowHandle(), HWND_TOP,
        0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}


TaskThumbnail::~TaskThumbnail()
{
    delete this->iconOverlayWindow;
    DwmUnregisterThumbnail(mThumbnail);
}


void TaskThumbnail::Activate()
{
    if (mTargetWindow == desktopWindow)
    {
        LiteStep::LSExecute(nullptr, L"!MinimizeWindows", SW_NORMAL);
    }
    else
    {
        WINDOWPLACEMENT wp;

        // Work out the window RECT
        ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
        GetWindowPlacement(mTargetWindow, &wp);
        if (wp.showCmd == SW_SHOWMINIMIZED)
        {
            ShowWindow(mTargetWindow, SW_SHOWNORMAL);
        }

        SetActiveWindow(mTargetWindow);
        SetForegroundWindow(mTargetWindow);
    }
}


void TaskThumbnail::Select()
{
    ActivateState(State::Selected);
}


void TaskThumbnail::Deselect()
{
    ClearState(State::Selected);
}


LRESULT WINAPI TaskThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra)
{
    UNREFERENCED_PARAMETER(extra);

    switch (message)
    {
    case WM_MOUSEMOVE:
        {
            ActivateState(State::Hover);
            ((TaskSwitcher*)mParent)->HoveringOverTask(this);
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            ClearState(State::Hover);
        }
        return 0;

    case WM_LBUTTONDOWN:
        {
            ((TaskSwitcher*)mParent)->Hide();
        }
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}


void TaskThumbnail::SetIcon(HICON icon)
{
    D2D1_RECT_F pos;
    pos.right = 32;
    pos.bottom = 32;
    pos.left = 0;
    pos.top = 0;

    this->iconOverlayWindow->AddOverlay(pos, icon);
    this->iconOverlayWindow->Show();
}


/// <summary>
/// Updates the icon
/// </summary>
void TaskThumbnail::UpdateIcon()
{
    if (mTargetWindow != desktopWindow)
    {
        this->requestedIcon = ICON_BIG;
        SendMessageCallbackW(mTargetWindow, WM_GETICON, ICON_BIG, NULL, UpdateIconCallback, (ULONG_PTR)this);
    }
    else
    {
        HICON icon = ExtractIconW(gLSModule.GetInstance(), L"shell32.dll", 34);

        if (icon)
        {
            SetIcon(icon);
            DestroyIcon(icon);
        }
    }
}


/// <summary>
/// Updates the icon
/// </summary>
void CALLBACK TaskThumbnail::UpdateIconCallback(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult)
{
    // We really only expect WM_GETICON messages.
    if (uMsg == WM_GETICON)
    {
        TaskThumbnail* taskThumbnail = (TaskThumbnail*)dwData;

        // If we got an icon back, use it.
        if (lResult != 0)
        {
            taskThumbnail->SetIcon((HICON)lResult);
        }
        else switch (taskThumbnail->requestedIcon)
        {
        case ICON_BIG:
            {
                taskThumbnail->requestedIcon = ICON_SMALL;
                SendMessageCallbackW(hWnd, WM_GETICON, ICON_SMALL, 0, UpdateIconCallback, dwData);
            }
            break;

        case ICON_SMALL:
            {
                taskThumbnail->requestedIcon = ICON_SMALL2;
                SendMessageCallbackW(hWnd, WM_GETICON, ICON_SMALL2, 0, UpdateIconCallback, dwData);
            }
            break;

        case ICON_SMALL2:
            {
                HICON hIcon;
                hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
                if (!hIcon) {
                    hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
                }
                taskThumbnail->SetIcon(hIcon);
            }
            break;
        }
    }
}
