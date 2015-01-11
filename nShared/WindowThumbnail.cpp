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
#include <algorithm>
#include "../Utilities/Math.h"
#include "../nCoreCom/Core.h"

using std::min;
using std::max;


WindowThumbnail::WindowThumbnail(LPCTSTR prefix, Settings* parentSettings) : Drawable(prefix, parentSettings)
{
    this->thumbnailHandle = nullptr;

    LoadSettings();
}


WindowThumbnail::~WindowThumbnail()
{
}


void WindowThumbnail::Show(HWND hwnd, LPRECT position)
{
    HRESULT hr = S_OK;
    SIZE sourceSize = {0};

    this->hwnd = hwnd;

    if (this->thumbnailHandle != nullptr)
    {
        DwmUnregisterThumbnail(this->thumbnailHandle);
    }

    hr = DwmRegisterThumbnail(mWindow->GetWindowHandle(), hwnd, &this->thumbnailHandle);
    
    if (SUCCEEDED(hr))
    {
        hr = DwmQueryThumbnailSourceSize(this->thumbnailHandle, &sourceSize);
    }

    if (SUCCEEDED(hr))
    {
        int x, y, width, height, maxWidth, maxHeight;
        double scale;
        MonitorInfo &monitorInfo = nCore::FetchMonitorInfo();
        const MonitorInfo::Monitor &monitor = monitorInfo.GetMonitor(
          monitorInfo.MonitorFromRECT(*position));
        switch (this->position)
        {
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

                // Ensure that the entire preview is on the same monitor as the button.
                x = Clamp(monitor.rect.left, (long)x, monitor.rect.right - width);
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

                // Ensure that the entire preview is on the same monitor as the button.
                y = Clamp(monitor.rect.top, (long)y, monitor.rect.bottom - height);
            }
            break;
        }

        if (mAnimate)
        {
            switch (this->position)
            {
                case TOP:
                    mWindow->SetPosition((float)position->left, (float)position->top - 1, (float)position->right - (float)position->left, 1);
                    break;

                case BOTTOM:
                    mWindow->SetPosition((float)position->left, (float)position->bottom, (float)position->right - (float)position->left, 1);
                    break;

                case LEFT:
                    mWindow->SetPosition((float)position->left, (float)position->bottom, 1, (float)position->bottom - (float)position->top);
                    break;

                case RIGHT:
                    mWindow->SetPosition((float)position->right, (float)position->bottom, 1, (float)position->bottom - (float)position->top);
                    break;
            }
            mWindow->SetAnimation((float)x, (float)y, (float)width, (float)height, mAnimationDuration, Easing::Type::Sine);
        }
        else
        {
            mWindow->SetPosition((float)x, (float)y, (float)width, (float)height);
            HandleMessage(nullptr, WM_SIZE, 0, MAKELPARAM(width, height), nullptr);
        }

        DWM_THUMBNAIL_PROPERTIES properties;
        properties.dwFlags = DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY | DWM_TNP_OPACITY;
        properties.fSourceClientAreaOnly = FALSE;
        properties.fVisible = TRUE;
        properties.opacity = (BYTE)this->thumbnailOpacity;

        hr = DwmUpdateThumbnailProperties(this->thumbnailHandle, &properties);
    }

    if (SUCCEEDED(hr))
    {
        mWindow->Show();
    }
}


void WindowThumbnail::Hide()
{
    DwmUnregisterThumbnail(this->thumbnailHandle);
    this->thumbnailHandle = NULL;
    this->hwnd = NULL;
    mWindow->Hide();
}


void WindowThumbnail::LoadSettings(bool /*bIsRefresh*/)
{
    // Load Window settings
    WindowSettings windowSettings;
    WindowSettings windowDefaults;
    windowDefaults.width = 150;
    windowDefaults.height = 40;
    windowDefaults.alwaysOnTop = true;
    windowSettings.Load(mSettings, &windowDefaults);

    // Load state Settings
    StateRender<States>::InitData stateDefaults;
    stateDefaults[States::Base].defaults.brushSettings[State::BrushType::Background].color = Color::Create(0xAA009900);
    stateDefaults[States::Base].defaults.brushSettings[State::BrushType::Text].color = Color::Create(0xFF000000);
    stateDefaults[States::Base].defaults.textOffsetTop = 2;
    stateDefaults[States::Base].defaults.textOffsetBottom = 2;
    stateDefaults[States::Base].defaults.textOffsetRight = 2;
    stateDefaults[States::Base].defaults.textOffsetLeft = 2;
    stateDefaults[States::Base].defaults.brushSettings[State::BrushType::Outline].color = Color::Create(0xAAFFFFFF);
    stateDefaults[States::Base].defaults.outlineWidth = 2.0f;
    mStateRender.Load(stateDefaults, mSettings);

    mWindow->Initialize(windowSettings, &mStateRender);

    TCHAR szBuf[32];

    this->distanceFromButton = mSettings->GetInt(L"DistanceFromButton", 2);
    this->maxHeight = mSettings->GetInt(L"MaxHeight", 300);
    this->maxWidth = mSettings->GetInt(L"MaxWidth", 300);
    this->offset = mSettings->GetOffsetRect(L"Offset", 5, 5, 5, 5);
    mSettings->GetString(L"Position", szBuf, sizeof(szBuf), L"Top"); 
    if (_wcsicmp(szBuf, L"Left") == 0)
    {
        this->position = LEFT;
    }
    else if (_wcsicmp(szBuf, L"Right") == 0)
    {
        this->position = RIGHT;
    }
    else if (_wcsicmp(szBuf, L"Bottom") == 0)
    {
        this->position = BOTTOM;
    }
    else
    {
        this->position = TOP;
    }
    mAnimate = mSettings->GetBool(L"Animate", false);
    mAnimationDuration = mSettings->GetInt(L"AnimationDuration", 200);
    this->sizeToButton = mSettings->GetBool(L"SizeToButton", true);
    this->thumbnailOpacity = mSettings->GetInt(L"ThumbnailOpacity", 255);
}


LRESULT WINAPI WindowThumbnail::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (message)
    {
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
