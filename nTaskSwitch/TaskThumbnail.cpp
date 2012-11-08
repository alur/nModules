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


TaskThumbnail::TaskThumbnail(Drawable* parent, HWND targetWindow, int x, int y, int width, int height) : Drawable(parent, "Task") {
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

    this->settings->GetOffsetRect("MarginLeft", "MarginTop", "MarginRight", "MarginBottom", &this->thumbnailMargins, 5, 5, 5, 5);

    DWM_THUMBNAIL_PROPERTIES properties;
    properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTDESTINATION;
    properties.fSourceClientAreaOnly = FALSE;
    properties.fVisible = TRUE;
    properties.rcDestination.bottom = y + height - this->thumbnailMargins.bottom;
    properties.rcDestination.top = y + this->thumbnailMargins.top;
    properties.rcDestination.left = x + this->thumbnailMargins.left;
    properties.rcDestination.right = x + width - this->thumbnailMargins.right;

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
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->window->ClearState(this->stateHover);
        }
        return 0;
    }

    return DefWindowProc(window, message, wParam, lParam);
}
