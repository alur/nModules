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

    this->window->Initialize();
    this->window->Show();

    this->targetWindow = targetWindow;
    DwmRegisterThumbnail(this->window->GetWindowHandle(), targetWindow, &this->thumbnail);

    DWM_THUMBNAIL_PROPERTIES properties;
    properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_RECTDESTINATION;
    properties.fSourceClientAreaOnly = FALSE;
    properties.fVisible = TRUE;
    properties.rcDestination.bottom = y + height - 10;
    properties.rcDestination.top = y + 10;
    properties.rcDestination.left = x + 10; 
    properties.rcDestination.right = x + width - 10;

    DwmUpdateThumbnailProperties(this->thumbnail, &properties);
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


LRESULT WINAPI TaskThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID extra) {
    UNREFERENCED_PARAMETER(extra);

    return DefWindowProc(window, message, wParam, lParam);
}
