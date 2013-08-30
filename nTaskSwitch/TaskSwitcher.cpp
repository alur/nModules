/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskSwitcher.cpp
 *  The nModules Project
 *
 *  Manages the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include "TaskSwitcher.hpp"
#include "../nShared/Window.hpp"
#include "../nShared/LSModule.hpp"
#include <Shlwapi.h>
#include "../nShared/DWMColorVal.hpp"


static UINT (WINAPI *DwmpActivateLivePreview)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown) = nullptr;
static HWND desktopWindow = nullptr;


TaskSwitcher::TaskSwitcher() : Drawable(L"nTaskSwitch")
{
    if (DwmpActivateLivePreview == nullptr)
    {
        DwmpActivateLivePreview = (UINT (WINAPI *)(UINT, HWND, HWND, UINT))GetProcAddress(GetModuleHandleW(L"DWMAPI.DLL"), (LPCSTR)0x71);
    }
    if (desktopWindow == nullptr)
    {
        desktopWindow = FindWindowW(L"DesktopBackgroundClass", nullptr);
    }

    this->hoveredThumbnail = nullptr;
    this->peekTimer = 0;

    LoadSettings();
    
    SetParent(mWindow->GetWindowHandle(), nullptr);
    SetWindowLongPtrW(mWindow->GetWindowHandle(), GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_COMPOSITED);
    SetWindowPos(mWindow->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


TaskSwitcher::~TaskSwitcher()
{
    Hide();
}


void TaskSwitcher::LoadSettings()
{
    MonitorInfo::Monitor primaryMonitor;
    primaryMonitor = mWindow->GetMonitorInformation()->m_monitors[0];

    mThumbnailSettings.Load(mSettings);

    StateRender<State>::InitData initData;
    initData[State::Base].defaults.brushSettings[::State::BrushType::Background].color = std::unique_ptr<IColorVal>(new DWMColorVal());
    initData[State::Base].defaults.fontSize = 16;
    initData[State::Base].defaults.fontWeight = DWRITE_FONT_WEIGHT_LIGHT;
    initData[State::Base].defaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
    initData[State::Base].defaults.brushSettings[::State::BrushType::Text].color = Color::Create(0xFFFFFFFF);
    initData[State::Base].defaults.textOffsetTop = 10;
    mStateRender.Load(initData, mSettings);

    WindowSettings defaults, windowSettings;
    defaults.alwaysOnTop = true;

    // Center in the workarea of the primary monitor.
    defaults.x = int(primaryMonitor.workArea.left + primaryMonitor.workAreaWidth*0.2);
    defaults.y = 200;
    defaults.width = int(primaryMonitor.workAreaWidth*0.6);
    defaults.height = 600;
    defaults.text = _wcsdup(L"Test");
    windowSettings.Load(mSettings, &defaults);

    mWindow->Initialize(windowSettings, &mStateRender);

    this->windowsPerRow = mSettings->GetInt(L"WindowsPerRow", 7);
    // LivePreview_ms doesn't seem to have any effect on explorer in Windows 8.
    this->peekDelay = mSettings->GetInt(L"PeekDelay", SHRegGetIntW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AltTab\\LivePreview_ms", 1000));

    LayoutSettings layoutDefaults;
    layoutDefaults.mPadding.left = 20;
    layoutDefaults.mPadding.right = 20;
    layoutDefaults.mPadding.top = 40;
    layoutDefaults.mPadding.bottom = 40;
    layoutDefaults.mRowSpacing = 0;
    layoutDefaults.mColumnSpacing = 0;
    this->layoutSettings.Load(mSettings, &layoutDefaults);

    this->taskWidth = (mWindow->GetSize().width - this->layoutSettings.mPadding.left - this->layoutSettings.mPadding.right - (this->windowsPerRow - 1) * this->layoutSettings.mColumnSpacing)/this->windowsPerRow;
    this->taskHeight = int(this->taskWidth/1.518f);
}


LRESULT WINAPI TaskSwitcher::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra)
{
    UNREFERENCED_PARAMETER(extra);

    if (message == WM_KEYUP || message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE)
    {
        Hide();
        return 0;
    }
    else if (message == WM_TIMER && wParam == this->peekTimer && mWindow->IsVisible())
    {
        this->peeking = true;
        Preview(this->shownWindows[this->selectedWindow]->mTargetWindow);
        mWindow->ClearCallbackTimer(this->peekTimer);
        this->peekTimer = 0;
        return 0;
    }
    else if (message == WM_MOUSEMOVE || message == WM_MOUSELEAVE)
    {
        HoveringOverTask(nullptr);
    }

    return DefWindowProcW(window, message, wParam, lParam);
}


void TaskSwitcher::HandleAltTab()
{
    if (mWindow->IsVisible())
    {
        UpdateActiveWindow(1);
    }
    else
    {
        Show(1);
    }
}


void TaskSwitcher::HandleAltShiftTab()
{
    if (mWindow->IsVisible())
    {
        UpdateActiveWindow(-1);
    }
    else
    {
        Show(-1);
    }
}


void TaskSwitcher::Hide()
{
    if (this->peekTimer != 0)
    {
        mWindow->ClearCallbackTimer(this->peekTimer);
        this->peekTimer = 0;
    }
    this->peeking = false;
    mWindow->Hide();

    if (this->shownWindows.size() != 0)
    {
        (this->hoveredThumbnail ? this->hoveredThumbnail : this->shownWindows[this->selectedWindow])->Activate();

        for (vector<TaskThumbnail*>::iterator wnd = this->shownWindows.begin(); wnd != this->shownWindows.end(); ++wnd)
        {
            delete *wnd;
        }
        this->shownWindows.clear(); 
    }

    DwmpActivateLivePreview(0, nullptr, nullptr, 1);
}


void TaskSwitcher::AddWindow(HWND window)
{
    this->shownWindows.push_back(new TaskThumbnail(
        this,
        window,
        this->layoutSettings.mPadding.left + this->shownWindows.size() % this->windowsPerRow * (this->taskWidth + this->layoutSettings.mColumnSpacing),
        this->layoutSettings.mPadding.top + (int)this->shownWindows.size() / this->windowsPerRow * (this->taskHeight + this->layoutSettings.mRowSpacing),
        this->taskWidth,
        this->taskHeight,
        mThumbnailSettings
    ));
}


void TaskSwitcher::Show(int delta)
{
    Window::UpdateLock updateLock(mWindow);

    this->hoveredThumbnail = nullptr;
    this->selectedWindow = 0;
    this->peeking = false;

    SetActiveWindow(mWindow->GetWindowHandle());
    SetForegroundWindow(mWindow->GetWindowHandle());
    EnumDesktopWindows(nullptr, LoadWindowsCallback, (LPARAM)this);

    if (desktopWindow)
    {
        AddWindow(desktopWindow);
    }

    int height = this->layoutSettings.mPadding.top + this->layoutSettings.mPadding.bottom + ((int)this->shownWindows.size() - 1) / this->windowsPerRow * (this->taskHeight + this->layoutSettings.mRowSpacing) + this->taskHeight;

    mWindow->SetPosition(
        mWindow->GetDrawingSettings()->x,
        mWindow->GetMonitorInformation()->m_monitors[0].workAreaHeight/2 - height/2 + mWindow->GetMonitorInformation()->m_monitors[0].workArea.top,
        mWindow->GetDrawingSettings()->width, 
        height);

    mWindow->Show(SW_SHOWNORMAL);
    
    for (auto &thumbnail : this->shownWindows)
    {
        thumbnail->UpdateIconPosition();
    }

    this->peekTimer = mWindow->SetCallbackTimer(this->peekDelay, this);
    
    UpdateActiveWindow(delta);
}


/// <summary>
/// Worker used by AddExisting.
/// </summary>
BOOL CALLBACK TaskSwitcher::LoadWindowsCallback(HWND window, LPARAM taskSwitcher)
{
    if (IsTaskbarWindow(window))
    {
        ((TaskSwitcher*)taskSwitcher)->AddWindow(window);
    }
    return TRUE;
}


/// <summary>
/// Determines if a window should be shown on the taskbar.
/// </summary>
bool TaskSwitcher::IsTaskbarWindow(HWND hWnd)
{
    // Make sure it's actually a window.
    if (!IsWindow(hWnd))
        return false;

    // And that it's visible
    if (!IsWindowVisible(hWnd))
        return false;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);

    // Windows with the WS_EX_APPWINDOW style should always be shown
    if ((exStyle & WS_EX_APPWINDOW) == WS_EX_APPWINDOW)
        return true;
    else if (GetParent(hWnd) != nullptr) // Windows with parents should not be shown
        return false;
    else if (::GetWindow(hWnd, GW_OWNER) != nullptr) // Windows with owners should not be shown
        return false;
    else if ((exStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW) // Tool windows should not be shown on the taskbar
        return false;

    return true;
}


/// <summary>
/// 
/// </summary>
void TaskSwitcher::UpdateActiveWindow(int delta)
{
    if (this->shownWindows.size() > 0)
    {
        if (delta != 0)
        {
            this->shownWindows[this->selectedWindow]->Deselect();

            this->selectedWindow += delta;

            if (this->selectedWindow < 0)
            {
                this->selectedWindow = int(this->shownWindows.size() - 1);
            }
            else if (this->selectedWindow >= int(this->shownWindows.size()))
            {
                this->selectedWindow = 0;
            }

            this->shownWindows[this->selectedWindow]->Select();
            this->hoveredThumbnail = nullptr;
        }

        HWND targetWindow = this->hoveredThumbnail ? this->hoveredThumbnail->mTargetWindow : this->shownWindows[this->selectedWindow]->mTargetWindow;

        if (targetWindow == desktopWindow)
        {
            mWindow->SetText(L"Desktop");
        }
        else
        {
            WCHAR text[MAX_PATH];
            GetWindowTextW(targetWindow, text, MAX_PATH);
            mWindow->SetText(text);
        }
        mWindow->Repaint();

        Preview(targetWindow);
    }
}


void TaskSwitcher::Preview(HWND window)
{
    if (this->peeking)
    {
        if (window == desktopWindow)
        {
            DwmpActivateLivePreview(1, GetDesktopWindow(), mWindow->GetWindowHandle(), 1);
        }
        else
        {
            DwmpActivateLivePreview(1, window, mWindow->GetWindowHandle(), 1);
        }
    }
    else if (this->peekTimer != 0)
    {
        SetTimer(mWindow->GetWindowHandle(), this->peekTimer, this->peekDelay, nullptr);
    }
}


void TaskSwitcher::HoveringOverTask(TaskThumbnail* task)
{
    if (task != this->hoveredThumbnail)
    {
        this->hoveredThumbnail = task;
        UpdateActiveWindow(0);
    }
}
