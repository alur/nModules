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


extern LSModule* g_LSModule;


TaskSwitcher::TaskSwitcher() {
    this->settings = new Settings("nTaskSwitch");
    this->window = g_LSModule->CreateDrawableWindow(this->settings, this);

    LoadSettings();
    
    SetParent(this->window->GetWindowHandle(), NULL);
    SetWindowLongPtr(this->window->GetWindowHandle(), GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_COMPOSITED);
    SetWindowPos(this->window->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


TaskSwitcher::~TaskSwitcher() {
    delete this->window;
    delete this->settings;
}


void TaskSwitcher::LoadSettings() {
    DrawableSettings defaults;
    defaults.alwaysOnTop = true;
    defaults.x = 200;
    defaults.y = 200;
    defaults.width = 1020;
    defaults.height = 600;
    defaults.text = _wcsdup(L"Test");

    StateSettings stateDefaults;
    stateDefaults.backgroundBrush.color = 0xDD000000;
    stateDefaults.fontSize = 20;
    StringCchCopy(stateDefaults.fontStyle, sizeof(stateDefaults.fontStyle), "Bold");
    StringCchCopy(stateDefaults.textAlign, sizeof(stateDefaults.textAlign), "Center");
    stateDefaults.textBrush.color = 0xFFFFFFFF;
    stateDefaults.textOffsetTop = 20;

    this->window->Initialize(&defaults, &stateDefaults);
}


LRESULT WINAPI TaskSwitcher::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra) {
    UNREFERENCED_PARAMETER(extra);

    if (message == WM_KEYUP) {
        this->window->Hide();
        ClearWindows();
        return 0;
    }
    else if (message == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE) {
        this->window->Hide();
        ClearWindows();
        return 0;
    }

    return DefWindowProc(window, message, wParam, lParam);
}


void TaskSwitcher::HandleAltTab() {
    if (this->window->IsVisible()) {
        ++this->selectedWindow;
        UpdateActiveWindow();
    }
    else {
        this->selectedWindow = 0;
        this->window->Show();
        SetActiveWindow(this->window->GetWindowHandle());
        SetForegroundWindow(this->window->GetWindowHandle());
        LoadWindows();
        UpdateActiveWindow();
    }
}


void TaskSwitcher::HandleAltShiftTab() {
    if (this->window->IsVisible()) {
        --this->selectedWindow;
        UpdateActiveWindow();
    }
    else {
        this->selectedWindow = 0;
        this->window->Show();
        SetActiveWindow(this->window->GetWindowHandle());
        SetForegroundWindow(this->window->GetWindowHandle());
        LoadWindows();
        UpdateActiveWindow();
    }
}


void TaskSwitcher::ClearWindows() {
    if (this->shownWindows.size() != 0) {
        SetActiveWindow(this->shownWindows[this->selectedWindow].window);
        SetForegroundWindow(this->shownWindows[this->selectedWindow].window);
        for (vector<ShownWindow>::iterator wnd = this->shownWindows.begin(); wnd != this->shownWindows.end(); ++wnd) {
            DwmUnregisterThumbnail(wnd->thumbnail);
        }
        this->shownWindows.clear(); 
    }
}


void TaskSwitcher::AddWindow(HWND window) {
    ShownWindow wnd;
    wnd.window = window;

    int x = this->shownWindows.size() % 5;
    int y = int(this->shownWindows.size() / 5);

    DwmRegisterThumbnail(this->window->GetWindowHandle(), window, &wnd.thumbnail);

    DWM_THUMBNAIL_PROPERTIES properties;
    properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTDESTINATION;
    properties.fSourceClientAreaOnly = FALSE;
    properties.fVisible = TRUE;
    properties.rcDestination.bottom = 60 + 120 + 140 * y;
    properties.rcDestination.top = 60 + 140 * y;
    properties.rcDestination.left = 20 + 200 * x; 
    properties.rcDestination.right = 20 + 180 + 200 * x;

    DwmUpdateThumbnailProperties(wnd.thumbnail, &properties);

    this->shownWindows.push_back(wnd);
}


void TaskSwitcher::LoadWindows() {
    EnumDesktopWindows(NULL, LoadWindowsCallback, (LPARAM)this);
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
    else if (GetWindow(hWnd, GW_OWNER) != NULL) // Windows with owners should not be shown
        return false;
    else if ((exStyle & WS_EX_TOOLWINDOW) == WS_EX_TOOLWINDOW) // Tool windows should not be shown on the taskbar
        return false;

    return true;
}


/// <summary>
/// 
/// </summary>
void TaskSwitcher::UpdateActiveWindow() {
    WCHAR text[MAX_PATH];
    if (this->selectedWindow < 0) {
        this->selectedWindow = int(this->shownWindows.size() - 1);
    }
    else if (this->selectedWindow >= this->shownWindows.size()) {
        this->selectedWindow = 0;
    }
    GetWindowTextW(this->shownWindows[this->selectedWindow].window, text, MAX_PATH);
    this->window->SetText(text);
    this->window->Repaint();
    TRACEW(L"TaskSwitch: %d, %s", this->selectedWindow, text);
}
