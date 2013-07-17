/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskThumbnail.cpp
 *  The nModules Project
 *
 *  A single task in the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "TaskThumbnail.hpp"
#include "../nShared/LSModule.hpp"
#include "../nShared/Debugging.h"
#include <strsafe.h>
#include <Shldisp.h>
#include "TaskSwitcher.hpp"
#include "../Utilities/StopWatch.hpp"

static HWND desktopWindow = nullptr;
static UINT (*DwmpActivateLivePreview)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown) = nullptr;

extern LSModule gLSModule;


TaskThumbnail::TaskThumbnail(Drawable* parent, HWND targetWindow, int x, int y, int width, int height) : Drawable(parent, "Task") {
    if (DwmpActivateLivePreview == nullptr) {
        DwmpActivateLivePreview = (UINT (*)(UINT, HWND, HWND, UINT))GetProcAddress(GetModuleHandleW(L"DWMAPI.DLL"), (LPCSTR)0x71);
    }
    if (desktopWindow == nullptr) {
        desktopWindow = FindWindowW(L"DesktopBackgroundClass", nullptr);
    }

    DrawableSettings defaults;
    defaults.x = x;
    defaults.y = y;
    defaults.width = width;
    defaults.height = height;

    StateSettings stateDefaults;
    stateDefaults.backgroundBrush.color = 0x00000000;

    this->window->Initialize(&defaults, &stateDefaults);
    this->window->Show();

    this->targetWindow = targetWindow;
    DwmRegisterThumbnail(this->window->GetWindowHandle(), targetWindow, &this->thumbnail);
    
    // 
    DWM_THUMBNAIL_PROPERTIES properties;
    properties.dwFlags = DWM_TNP_SOURCECLIENTAREAONLY;
    properties.fSourceClientAreaOnly = FALSE;
    
    if (targetWindow == desktopWindow) {
        properties.dwFlags |= DWM_TNP_RECTSOURCE;
        MonitorInfo* monInfo = this->window->GetMonitorInformation();
        properties.rcSource = monInfo->m_monitors[0].rect;
        properties.rcSource.bottom -= monInfo->m_virtualDesktop.rect.top;
        properties.rcSource.top -= monInfo->m_virtualDesktop.rect.top;
        properties.rcSource.left -= monInfo->m_virtualDesktop.rect.left;
        properties.rcSource.right -= monInfo->m_virtualDesktop.rect.left;
    }
    
    DwmUpdateThumbnailProperties(this->thumbnail, &properties);

    //
    SIZE sourceSize;
    if (targetWindow == desktopWindow) {
        MonitorInfo* monInfo = this->window->GetMonitorInformation();
        sourceSize.cx = monInfo->m_monitors[0].width;
        sourceSize.cy = monInfo->m_monitors[0].height;
    }
    else {
        DwmQueryThumbnailSourceSize(this->thumbnail, &sourceSize);
    }
    this->settings->GetOffsetRect("MarginLeft", "MarginTop", "MarginRight", "MarginBottom", &this->thumbnailMargins, 10, 10, 10, 10);

    double scale = min(
        (width - this->thumbnailMargins.left - this->thumbnailMargins.right)/(double)sourceSize.cx,
        (height - this->thumbnailMargins.bottom - this->thumbnailMargins.top)/(double)sourceSize.cy);
    sourceSize.cx = long(sourceSize.cx*scale);
    sourceSize.cy = long(sourceSize.cy*scale);

    int horizontalOffset = (width - this->thumbnailMargins.left - this->thumbnailMargins.right - sourceSize.cx)/2;
    int verticalOffset = (height - this->thumbnailMargins.top - this->thumbnailMargins.bottom - sourceSize.cy)/2;

    properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_RECTDESTINATION;
    properties.fVisible = TRUE;
    properties.rcDestination.bottom = y + height - this->thumbnailMargins.bottom - verticalOffset;
    properties.rcDestination.top = y + this->thumbnailMargins.top + verticalOffset;
    properties.rcDestination.left = x + this->thumbnailMargins.left + horizontalOffset;
    properties.rcDestination.right = x + width - this->thumbnailMargins.right - horizontalOffset;

    DwmUpdateThumbnailProperties(this->thumbnail, &properties);

    //
    StateSettings hoverDefaults(stateDefaults);
    hoverDefaults.backgroundBrush.color = 0xCC888888;
    this->stateHover = this->window->AddState("Hover", 100, &hoverDefaults);

    //
    StateSettings selectedDefaults(stateDefaults);
    selectedDefaults.outlineBrush.color = 0xFFFFFFFF;
    selectedDefaults.outlineWidth = 5;
    this->stateSelected = this->window->AddState("Selected", 150, &selectedDefaults);

    //
    StateSettings selectedHoverDefaults(hoverDefaults);
    selectedHoverDefaults.outlineBrush.color = 0xFFFFFFFF;
    selectedHoverDefaults.outlineWidth = 5;
    this->stateSelectedHover = this->window->AddState("SelectedHover", 200, &selectedHoverDefaults);

    //
    this->iconOverlayWindow = gLSModule.CreateDrawableWindow(this->settings, this);

    DrawableSettings iconDefaults;
    iconDefaults.alwaysOnTop = true;

    StateSettings iconStateDefaults;
    iconStateDefaults.backgroundBrush.color = 0;
    
    RECT r;
    this->window->GetScreenRect(&r);
    this->iconOverlayWindow->Initialize(&iconDefaults, &iconStateDefaults);
    this->iconOverlayWindow->SetPosition(r.right - 32 - horizontalOffset - this->thumbnailMargins.right + 4,
        r.bottom - 32 - verticalOffset - this->thumbnailMargins.bottom + 4, 32, 32);

    SetWindowPos(this->iconOverlayWindow->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    UpdateIcon();
}


void TaskThumbnail::UpdateIconPosition() {
    RECT r;
    this->window->GetScreenRect(&r);
    this->iconOverlayWindow->SetPosition(r.right - 32, r.bottom - 32, 32, 32);

    SetWindowPos(this->iconOverlayWindow->GetWindowHandle(), HWND_TOP,
        0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}


TaskThumbnail::~TaskThumbnail() {
    delete this->iconOverlayWindow;
    DwmUnregisterThumbnail(this->thumbnail);
}


void TaskThumbnail::Activate() {
    if (this->targetWindow == desktopWindow) {
        LiteStep::LSExecute(nullptr, "!MinimizeWindows", SW_NORMAL);
    }
    else {
        WINDOWPLACEMENT wp;

        // Work out the window RECT
        ZeroMemory(&wp, sizeof(WINDOWPLACEMENT));
        GetWindowPlacement(this->targetWindow, &wp);
        if (wp.showCmd == SW_SHOWMINIMIZED) {
            ShowWindow(this->targetWindow, SW_SHOWNORMAL);
        }

        SetActiveWindow(this->targetWindow);
        SetForegroundWindow(this->targetWindow);
    }
}


void TaskThumbnail::Select() {
    if ((*this->stateHover)->active) {
        this->window->ActivateState(this->stateSelectedHover);
    }
    this->window->ActivateState(this->stateSelected);
}


void TaskThumbnail::Deselect() {
    this->window->ClearState(this->stateSelected);
    this->window->ClearState(this->stateSelectedHover);
}


LRESULT WINAPI TaskThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra) {
    UNREFERENCED_PARAMETER(extra);

    switch (message) {
    case WM_MOUSEMOVE:
        {
            if ((*this->stateSelected)->active) {
                this->window->ActivateState(this->stateSelectedHover);
            }
            this->window->ActivateState(this->stateHover);
            ((TaskSwitcher*)this->parent)->HoveringOverTask(this);
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->window->ClearState(this->stateHover);
            this->window->ClearState(this->stateSelectedHover);
        }
        return 0;

    case WM_LBUTTONDOWN:
        {
            ((TaskSwitcher*)this->parent)->Hide();
        }
        return 0;
    }

    return DefWindowProcW(window, message, wParam, lParam);
}


void TaskThumbnail::SetIcon(HICON icon) {
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
void TaskThumbnail::UpdateIcon() {
    if (this->targetWindow != desktopWindow) {
        this->requestedIcon = ICON_BIG;
        SendMessageCallbackW(this->targetWindow, WM_GETICON, ICON_BIG, NULL, UpdateIconCallback, (ULONG_PTR)this);
    }
    else {
        HICON icon = ExtractIconW(gLSModule.GetInstance(), L"shell32.dll", 34);

        if (icon) {
            SetIcon(icon);
            DestroyIcon(icon);
        }
    }
}


/// <summary>
/// Updates the icon
/// </summary>
void CALLBACK TaskThumbnail::UpdateIconCallback(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult) {
    // We really only expect WM_GETICON messages.
    if (uMsg == WM_GETICON) {
        TaskThumbnail* taskThumbnail = (TaskThumbnail*)dwData;

        // If we got an icon back, use it.
        if (lResult != 0) {
            taskThumbnail->SetIcon((HICON)lResult);
        }
        else switch (taskThumbnail->requestedIcon) {
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
