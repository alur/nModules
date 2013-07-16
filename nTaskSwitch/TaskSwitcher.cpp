/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskSwitcher.cpp
 *  The nModules Project
 *
 *  Manages the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "TaskSwitcher.hpp"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/Debugging.h"
#include <strsafe.h>
#include <Shlwapi.h>


static UINT (* DwmpActivateLivePreview)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown) = nullptr;
static HWND desktopWindow = nullptr;


TaskSwitcher::TaskSwitcher() : Drawable("nTaskSwitch") {
    if (DwmpActivateLivePreview == nullptr) {
        DwmpActivateLivePreview = (UINT (*)(UINT, HWND, HWND, UINT))GetProcAddress(GetModuleHandle("DWMAPI.DLL"), (LPCSTR)0x71);
    }
    if (desktopWindow == nullptr) {
        desktopWindow = FindWindowW(L"DesktopBackgroundClass", nullptr);
    }

    this->hoveredThumbnail = nullptr;
    this->peekTimer = 0;

    LoadSettings();
    
    SetParent(this->window->GetWindowHandle(), nullptr);
    SetWindowLongPtr(this->window->GetWindowHandle(), GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_COMPOSITED);
    SetWindowPos(this->window->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


TaskSwitcher::~TaskSwitcher() {
    Hide();
}


void TaskSwitcher::LoadSettings() {
    MonitorInfo::Monitor primaryMonitor;
    primaryMonitor = this->window->GetMonitorInformation()->m_monitors[0];

    DrawableSettings defaults;
    defaults.alwaysOnTop = true;

    // Center in the workarea of the primary monitor.
    defaults.x = int(primaryMonitor.workArea.left + primaryMonitor.workAreaWidth*0.2);
    defaults.y = 200;
    defaults.width = int(primaryMonitor.workAreaWidth*0.6);
    defaults.height = 600;
    defaults.text = _wcsdup(L"Test");

    StateSettings stateDefaults;
    stateDefaults.backgroundBrush.color = 0xDD000000;
    stateDefaults.fontSize = 16;
    StringCchCopy(stateDefaults.fontWeight, sizeof(stateDefaults.fontWeight), "Light");
    StringCchCopy(stateDefaults.textAlign, sizeof(stateDefaults.textAlign), "Center");
    stateDefaults.textBrush.color = 0xFFFFFFFF;
    stateDefaults.textOffsetTop = 10;
    BOOL b;
    DwmGetColorizationColor(&stateDefaults.backgroundBrush.color, &b);

    this->window->Initialize(&defaults, &stateDefaults);

    this->windowsPerRow = this->settings->GetInt("WindowsPerRow", 7);
    // LivePreview_ms doesn't seem to have any effect on explorer in windows 8.
    this->peekDelay = this->settings->GetInt("PeekDelay", SHRegGetIntW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\AltTab\\LivePreview_ms", 1000));

    LayoutSettings layoutDefaults;
    layoutDefaults.padding.left = 20;
    layoutDefaults.padding.right = 20;
    layoutDefaults.padding.top = 40;
    layoutDefaults.padding.bottom = 40;
    layoutDefaults.rowSpacing = 0;
    layoutDefaults.columnSpacing = 0;

    this->layoutSettings.Load(this->settings, &layoutDefaults);

    this->taskWidth = (this->window->GetDrawingSettings()->width - this->layoutSettings.padding.left - this->layoutSettings.padding.right - (this->windowsPerRow - 1) * this->layoutSettings.columnSpacing)/this->windowsPerRow;
    this->taskHeight = int(this->taskWidth/1.518f);
}


LRESULT WINAPI TaskSwitcher::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra) {
    UNREFERENCED_PARAMETER(extra);

    if (message == WM_KEYUP || message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE) {
        Hide();
        return 0;
    }
    else if (message == WM_TIMER && wParam == this->peekTimer && this->window->IsVisible()) {
        this->peeking = true;
        Preview(this->shownWindows[this->selectedWindow]->targetWindow);
        this->window->ClearCallbackTimer(this->peekTimer);
        this->peekTimer = 0;
        return 0;
    }
    else if (message == WM_MOUSEMOVE || message == WM_MOUSELEAVE) {
        HoveringOverTask(NULL);
    }

    return DefWindowProc(window, message, wParam, lParam);
}


void TaskSwitcher::HandleAltTab() {
    if (this->window->IsVisible()) {
        UpdateActiveWindow(1);
    }
    else {
        Show(1);
    }
}


void TaskSwitcher::HandleAltShiftTab() {
    if (this->window->IsVisible()) {
        UpdateActiveWindow(-1);
    }
    else {
        Show(-1);
    }
}


void TaskSwitcher::Hide() {
    if (this->peekTimer != 0) {
        this->window->ClearCallbackTimer(this->peekTimer);
        this->peekTimer = 0;
    }
    this->peeking = false;
    this->window->Hide();

    if (this->shownWindows.size() != 0) {
        (this->hoveredThumbnail ? this->hoveredThumbnail : this->shownWindows[this->selectedWindow])->Activate();

        for (vector<TaskThumbnail*>::iterator wnd = this->shownWindows.begin(); wnd != this->shownWindows.end(); ++wnd) {
            delete *wnd;
        }
        this->shownWindows.clear(); 
    }

    DwmpActivateLivePreview(0, NULL, NULL, 1);
}


void TaskSwitcher::AddWindow(HWND window) {
    this->shownWindows.push_back(new TaskThumbnail(
        this,
        window,
        this->layoutSettings.padding.left + this->shownWindows.size() % this->windowsPerRow * (this->taskWidth + this->layoutSettings.columnSpacing),
        this->layoutSettings.padding.top + (int)this->shownWindows.size() / this->windowsPerRow * (this->taskHeight + this->layoutSettings.rowSpacing),
        this->taskWidth,
        this->taskHeight
    ));
}


void TaskSwitcher::Show(int delta) {
    this->hoveredThumbnail = nullptr;
    this->selectedWindow = 0;
    this->peeking = false;

    SetActiveWindow(this->window->GetWindowHandle());
    SetForegroundWindow(this->window->GetWindowHandle());
    EnumDesktopWindows(NULL, LoadWindowsCallback, (LPARAM)this);

    if (desktopWindow) {
        AddWindow(desktopWindow);
    }

    int height = this->layoutSettings.padding.top + this->layoutSettings.padding.bottom + ((int)this->shownWindows.size() - 1) / this->windowsPerRow * (this->taskHeight + this->layoutSettings.rowSpacing) + this->taskHeight;

    this->window->SetPosition(
        this->window->GetDrawingSettings()->x,
        this->window->GetMonitorInformation()->m_monitors[0].workAreaHeight/2 - height/2 + this->window->GetMonitorInformation()->m_monitors[0].workArea.top,
        this->window->GetDrawingSettings()->width, 
        height);

    this->window->Show(SW_SHOWNORMAL);
    
    for (auto &thumbnail : this->shownWindows) {
        thumbnail->UpdateIconPosition();
    }

    this->peekTimer = this->window->SetCallbackTimer(this->peekDelay, this);
    
    UpdateActiveWindow(delta);
}


/// <summary>
/// Worker used by AddExisting.
/// </summary>
BOOL CALLBACK TaskSwitcher::LoadWindowsCallback(HWND window, LPARAM taskSwitcher) {
    if (IsTaskbarWindow(window)) {
        ((TaskSwitcher*)taskSwitcher)->AddWindow(window);
    }
    return TRUE;
}


/// <summary>
/// Determines if a window should be shown on the taskbar.
/// </summary>
bool TaskSwitcher::IsTaskbarWindow(HWND hWnd) {
    // Make sure it's actually a window.
    if (!IsWindow(hWnd))
        return false;

    // And that it's visible
    if (!IsWindowVisible(hWnd))
        return false;

    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);

    // Windows with the WS_EX_APPWINDOW style should always be shown
    if ((exStyle & WS_EX_APPWINDOW) == WS_EX_APPWINDOW)
        return true;
    else if (GetParent(hWnd) != NULL) // Windows with parents should not be shown
        return false;
    else if (::GetWindow(hWnd, GW_OWNER) != NULL) // Windows with owners should not be shown
        return false;
    else if ((exStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW) // Tool windows should not be shown on the taskbar
        return false;

    return true;
}


/// <summary>
/// 
/// </summary>
void TaskSwitcher::UpdateActiveWindow(int delta) {
    if (this->shownWindows.size() > 0) {
        if (delta != 0) {
            this->shownWindows[this->selectedWindow]->Deselect();

            this->selectedWindow += delta;

            if (this->selectedWindow < 0) {
                this->selectedWindow = int(this->shownWindows.size() - 1);
            }
            else if (this->selectedWindow >= int(this->shownWindows.size())) {
                this->selectedWindow = 0;
            }

            this->shownWindows[this->selectedWindow]->Select();
            this->hoveredThumbnail = NULL;
        }

        HWND targetWindow = this->hoveredThumbnail ? this->hoveredThumbnail->targetWindow : this->shownWindows[this->selectedWindow]->targetWindow;

        if (targetWindow == desktopWindow) {
            this->window->SetText(L"Desktop");
        }
        else {
            WCHAR text[MAX_PATH];
            GetWindowTextW(targetWindow, text, MAX_PATH);
            this->window->SetText(text);
        }
        this->window->Repaint();

        Preview(targetWindow);
    }
}


void TaskSwitcher::Preview(HWND window) {
    if (this->peeking) {
        if (window == desktopWindow) {
            DwmpActivateLivePreview(1, GetDesktopWindow(), this->window->GetWindowHandle(), 1);
        }
        else {
            DwmpActivateLivePreview(1, window, this->window->GetWindowHandle(), 1);
        }
    }
    else if (this->peekTimer != 0) {
        SetTimer(this->window->GetWindowHandle(), this->peekTimer, this->peekDelay, NULL);
    }
}


void TaskSwitcher::HoveringOverTask(TaskThumbnail* task) {
    if (task != this->hoveredThumbnail) {
        this->hoveredThumbnail = task;
        UpdateActiveWindow(0);
    }
}
