/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowThumbnail.cpp
 *  The nModules Project
 *
 *  Draws a thumbnail of some window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "WindowThumbnail.hpp"
#include <dwmapi.h>


WindowThumbnail::WindowThumbnail(LPCSTR prefix, Settings* parentSettings) : Drawable(prefix, parentSettings) {
    this->thumbnailHandle = NULL;

    LoadSettings();

    // Not working...
    SetWindowPos(this->window->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


WindowThumbnail::~WindowThumbnail() {
}


void WindowThumbnail::Show(HWND hwnd, LPRECT position) {
    HRESULT hr = S_OK;
    SIZE sourceSize = {0};

    this->hwnd = hwnd;

    if (this->thumbnailHandle != NULL) {
        DwmUnregisterThumbnail(this->thumbnailHandle);
    }

    hr = DwmRegisterThumbnail(this->window->GetWindowHandle(), hwnd, &this->thumbnailHandle);
    
    if (SUCCEEDED(hr)) {
        hr = DwmQueryThumbnailSourceSize(this->thumbnailHandle, &sourceSize);
    }

    if (SUCCEEDED(hr)) {
        int x, y, width, height, maxWidth, maxHeight;
        double scale;
        switch (this->position) {
        default:
        case TOP:
        case BOTTOM:
            {
                maxWidth = (this->sizeToButton ? position->right - position->left : this->maxWidth) - this->offset.left - this->offset.right;
                maxHeight = (this->maxHeight - this->offset.top - this->offset.bottom);
                scale = min(maxWidth/(double)sourceSize.cx, maxHeight/(double)sourceSize.cy);
                width = int(scale*sourceSize.cx) + this->offset.left + this->offset.right;
                height = int(scale*sourceSize.cy) + this->offset.top + this->offset.bottom;
                x = position->left + (position->right - position->left - width)/2;
                y = this->position == BOTTOM ? position->bottom + this->distanceFromButton : position->top - height - this->distanceFromButton;
            }
            break;

        case LEFT:
        case RIGHT:
            {
                maxWidth = this->maxWidth - this->offset.left - this->offset.right;
                maxHeight = (this->sizeToButton ? position->bottom - position->top : this->maxHeight) - this->offset.top - this->offset.bottom;
                scale = min(maxWidth/(double)sourceSize.cx, maxHeight/(double)sourceSize.cy);
                width = int(scale*sourceSize.cx) + this->offset.left + this->offset.right;
                height = int(scale*sourceSize.cy) + this->offset.top + this->offset.bottom;
                y = position->top;
                x = this->position == LEFT ? position->left - width - this->distanceFromButton : position->right + this->distanceFromButton;
            }
            break;
        }

        switch (this->position) {
            case TOP:
                this->window->SetPosition(position->left, position->top - 1, position->right - position->left, 1);
                break;

            case BOTTOM:
                this->window->SetPosition(position->left, position->bottom, position->right - position->left, 1);
                break;

            case LEFT:
                this->window->SetPosition(position->left, position->bottom, 1, position->bottom - position->top);
                break;

            case RIGHT:
                this->window->SetPosition(position->right, position->bottom, 1, position->bottom - position->top);
                break;
        }
        this->window->SetAnimation(x, y, width, height, 200, Easing::SINE);

        DWM_THUMBNAIL_PROPERTIES properties;
        properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_OPACITY;
        properties.fSourceClientAreaOnly = FALSE;
        properties.fVisible = TRUE;
        properties.opacity = (BYTE)this->thumbnailOpacity;

        hr = DwmUpdateThumbnailProperties(this->thumbnailHandle, &properties);
    }

    if (SUCCEEDED(hr)) {
        this->window->Show();
        SetWindowPos(this->window->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }
}


void WindowThumbnail::Hide() {
    DwmUnregisterThumbnail(this->thumbnailHandle);
    this->thumbnailHandle = NULL;
    this->hwnd = NULL;
    this->window->Hide();
}


void WindowThumbnail::LoadSettings(bool /*bIsRefresh*/) {
    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 150;
    defaults->height = 40;
    defaults->alwaysOnTop = true;
    
    StateSettings* defaultState = new StateSettings();
    defaultState->color = 0xAA009900;
    defaultState->fontColor = 0xFF000000;
    defaultState->textOffsetTop = 2;
    defaultState->textOffsetBottom = 2;
    defaultState->textOffsetRight = 2;
    defaultState->textOffsetLeft = 2;
    defaultState->outlineColor = 0xAAFFFFFF;
    defaultState->outlineWidth = 2.0f;

    this->window->Initialize(defaults,  defaultState);

    char szBuf[32];

    this->distanceFromButton = settings->GetInt("DistanceFromButton", 2);
    this->maxHeight = settings->GetInt("MaxHeight", 300);
    this->maxWidth = settings->GetInt("MaxWidth", 300);
    this->settings->GetOffsetRect("OffsetLeft", "OffsetTop", "OffsetRight", "OffsetBottom", &this->offset, 5, 5, 5, 5);
    this->settings->GetString("Position", szBuf, sizeof(szBuf), "Top"); 
    if (_stricmp(szBuf, "Left") == 0) {
        this->position = LEFT;
    }
    else if (_stricmp(szBuf, "Right") == 0) {
        this->position = RIGHT;
    }
    else if (_stricmp(szBuf, "Bottom") == 0) {
        this->position = BOTTOM;
    }
    else {
        this->position = TOP;
    }
    this->sizeToButton = settings->GetBool("SizeToButton", true);
    this->thumbnailOpacity = settings->GetInt("ThumbnailOpacity", 255);
}


LRESULT WINAPI WindowThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (message) {
    case WM_SIZE:
        {
            DWM_THUMBNAIL_PROPERTIES properties;
            properties.dwFlags = DWM_TNP_RECTDESTINATION;
            properties.rcDestination.left = this->offset.left;
            properties.rcDestination.right = LOWORD(lParam) - this->offset.right;
            properties.rcDestination.top = this->offset.top;
            properties.rcDestination.bottom = HIWORD(lParam) - this->offset.bottom;
            DwmUpdateThumbnailProperties(this->thumbnailHandle, &properties);
        }
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}
