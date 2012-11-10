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


static HWND desktopWindow = NULL;
static UINT (* DwmpActivateLivePreview)(UINT onOff, HWND hWnd, HWND topMost, UINT unknown) = NULL;


TaskThumbnail::TaskThumbnail(Drawable* parent, HWND targetWindow, int x, int y, int width, int height) : Drawable(parent, "Task") {
    if (DwmpActivateLivePreview == NULL) {
        DwmpActivateLivePreview = (UINT (*)(UINT, HWND, HWND, UINT))GetProcAddress(GetModuleHandle("DWMAPI.DLL"), (LPCSTR)0x71);
    }
    if (desktopWindow == NULL) {
        desktopWindow = FindWindowW(L"DesktopBackgroundClass", NULL);
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
    properties.fSourceClientAreaOnly = TRUE;
    
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
    this->settings->GetOffsetRect("MarginLeft", "MarginTop", "MarginRight", "MarginBottom", &this->thumbnailMargins, 5, 5, 5, 5);

    double scale = min(
        (width - this->thumbnailMargins.left - this->thumbnailMargins.right)/(double)sourceSize.cx,
        (height - this->thumbnailMargins.bottom - this->thumbnailMargins.top)/(double)sourceSize.cy);
    sourceSize.cx *= scale;
    sourceSize.cy *= scale;

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
}


TaskThumbnail::~TaskThumbnail() {
    DwmUnregisterThumbnail(this->thumbnail);
}


void TaskThumbnail::Activate() {
    if (this->targetWindow == desktopWindow) {
        /*
        IShellDispatch4 *pShellDisp = NULL;
        CoCreateInstance(CLSID_Shell, NULL, CLSCTX_SERVER, IID_IDispatch, (LPVOID *) &pShellDisp);
        pShellDisp->ToggleDesktop();
        pShellDisp->Release();
        */
        LiteStep::LSExecute(NULL, "!MinimizeWindows", SW_NORMAL);
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
    this->window->ActivateState(this->stateSelected);
}


void TaskThumbnail::Deselect() {
    this->window->ClearState(this->stateSelected);
}


LRESULT WINAPI TaskThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra) {
    UNREFERENCED_PARAMETER(extra);

    switch (message) {
    case WM_MOUSEMOVE:
        {
            this->window->ActivateState(this->stateHover);
            ((TaskSwitcher*)this->parent)->HoveringOverTask(this);
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->window->ClearState(this->stateHover);
        }
        return 0;

    case WM_NCLBUTTONDOWN:
        {
            ((TaskSwitcher*)this->parent)->Hide();
        }
        return 0;
    }

    return DefWindowProc(window, message, wParam, lParam);
}
