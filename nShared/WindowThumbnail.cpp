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
    SetParent(this->window->GetWindow(), NULL);
    SetWindowPos(this->window->GetWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


WindowThumbnail::~WindowThumbnail() {
}


void WindowThumbnail::Show(HWND hwnd, LPRECT position) {
    HRESULT hr = S_OK;
    SIZE sourceSize = {0};
    RECT destRect = {0};

    this->hwnd = hwnd;

    if (this->thumbnailHandle != NULL) {
        DwmUnregisterThumbnail(this->thumbnailHandle);
    }

    hr = DwmRegisterThumbnail(this->window->GetWindow(), hwnd, &this->thumbnailHandle);
    
    if (SUCCEEDED(hr)) {
        hr = DwmQueryThumbnailSourceSize(this->thumbnailHandle, &sourceSize);
    }

    if (SUCCEEDED(hr)) {
        int x, y, width, height, maxWidth, maxHeight;
        double scale;
        switch (this->position) {
        case TOP:
        case BOTTOM:
            {
                maxWidth = (this->sizeToButton ? position->right - position->left : this->maxWidth) - this->offset.left - this->offset.right;
                maxHeight = this->maxHeight - this->offset.top - this->offset.bottom;
                scale = min(maxWidth/(double)sourceSize.cx, maxHeight/(double)sourceSize.cy);
                width = scale*sourceSize.cx + this->offset.left + this->offset.right;
                height = scale*sourceSize.cy + this->offset.top + this->offset.bottom;
                x = position->left + (position->right - position->left - width)/2;
                y = this->position == TOP ? position->top - height - this->distanceFromButton : position->bottom + this->distanceFromButton;
            }
            break;

        case LEFT:
        case RIGHT:
            {
                maxWidth = this->maxWidth - this->offset.left - this->offset.right;
                maxHeight = (this->sizeToButton ? position->bottom - position->top : this->maxHeight) - this->offset.top - this->offset.bottom;
                scale = min(maxWidth/(double)sourceSize.cx, maxHeight/(double)sourceSize.cy);
                width = scale*sourceSize.cx + this->offset.left + this->offset.right;
                height = scale*sourceSize.cy + this->offset.top + this->offset.bottom;
                y = position->top;
                x = this->position == LEFT ? position->left - width - this->distanceFromButton : position->right + this->distanceFromButton;
            }
            break;
        }

        this->window->SetPosition(x, y, width, height);

        RECT dest = { this->offset.left, this->offset.top, width - this->offset.right, height - this->offset.bottom };
        DWM_THUMBNAIL_PROPERTIES properties;
        properties.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_OPACITY;
        properties.fSourceClientAreaOnly = FALSE;
        properties.fVisible = TRUE;
        properties.opacity = this->thumbnailOpacity;
        properties.rcDestination = dest;

        hr = DwmUpdateThumbnailProperties(this->thumbnailHandle, &properties);
    }

    if (SUCCEEDED(hr)) {
        this->window->Show();
        SetWindowPos(this->window->GetWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    }
}


void WindowThumbnail::Hide() {
    DwmUnregisterThumbnail(this->thumbnailHandle);
    this->window->Hide();
}


void WindowThumbnail::LoadSettings(bool /*bIsRefresh*/) {
    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 150;
    defaults->height = 40;
    defaults->color = 0xAA009900;
    defaults->fontColor = 0xFF000000;
    defaults->textOffsetTop = 2;
    defaults->textOffsetBottom = 2;
    defaults->textOffsetRight = 2;
    defaults->textOffsetLeft = 2;
    defaults->alwaysOnTop = true;
    this->window->Initialize(defaults);

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


LRESULT WINAPI WindowThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(window, message, wParam, lParam);
}
