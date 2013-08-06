/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/Common.h"
#include "Balloon.hpp"
#include <algorithm>


Balloon::Balloon(LPCTSTR prefix, Settings* parentSettings, UINT clickedMessage, MessageHandler* callbackHandler) : Drawable(prefix, parentSettings)
{
    // Inialize the window.
    DrawableSettings defaults;
    defaults.width = 150;
    defaults.height = 40;
    defaults.alwaysOnTop = true;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = 0xCCFAFAD2;
    defaultState.textBrush.color = 0xFF000000;
    defaultState.textOffsetTop = 4;
    defaultState.textOffsetBottom = 4;
    defaultState.textOffsetRight = 4;
    defaultState.textOffsetLeft = 40;
    defaultState.outlineBrush.color = 0xAA000000;
    defaultState.outlineWidth = 1.5f;
    defaultState.wordWrapping = DWRITE_WORD_WRAPPING_WRAP;
    defaultState.cornerRadiusX = 4.0f;
    defaultState.cornerRadiusY = 4.0f;

    mWindow->Initialize(&defaults, &defaultState);

    mClickedMessage = clickedMessage;
    mCallbackHandler = callbackHandler;

    // Create the a child window for the title of the balloon.
    mTitleSettings = parentSettings->CreateChild(_T("Title"));
    mTitleWindow = mWindow->CreateChild(mTitleSettings, this);
    DrawableSettings titleDefaults;
    titleDefaults.x = 4;
    titleDefaults.y = 4;

    StateSettings titleBaseStateDefaults;
    titleBaseStateDefaults.textBrush.color = 0xFF000000;
    titleBaseStateDefaults.backgroundBrush.color = 0;
    titleBaseStateDefaults.fontWeight = DWRITE_FONT_WEIGHT_BOLD;

    mTitleWindow->Initialize(&titleDefaults, &titleBaseStateDefaults);
    mTitleWindow->Show();

    // And another for the x button in the top right corner.
    mCloseBtnSettings = parentSettings->CreateChild(_T("CloseButton"));
    mCloseBtnWindow = mWindow->CreateChild(mCloseBtnSettings, this);
    DrawableSettings* closeBtnDefaults = new DrawableSettings();
    closeBtnDefaults->width = 16;
    closeBtnDefaults->height = 16;

    StateSettings closeBtnBaseStateDefaults;
    closeBtnBaseStateDefaults.backgroundBrush.color = 0xAA77AACC;
    closeBtnBaseStateDefaults.textBrush.color = 0xFF000000;
    closeBtnBaseStateDefaults.outlineBrush.color = 0xFF000000;
    closeBtnBaseStateDefaults.outlineWidth = 1.0f;
    closeBtnBaseStateDefaults.cornerRadiusX = 2.0f;
    closeBtnBaseStateDefaults.cornerRadiusY = 2.0f;
    closeBtnBaseStateDefaults.fontWeight = DWRITE_FONT_WEIGHT_BOLD;
    closeBtnBaseStateDefaults.textAlign = DWRITE_TEXT_ALIGNMENT_CENTER;
    closeBtnBaseStateDefaults.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;

    mCloseBtnWindow->Initialize(closeBtnDefaults, &closeBtnBaseStateDefaults);
    mCloseBtnWindow->SetText(L"X");
    mCloseBtnWindow->Show();

    // Not working...
    SetParent(mWindow->GetWindowHandle(), NULL);
    SetWindowPos(mWindow->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetWindowPos(mWindow->GetWindowHandle(), HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


Balloon::~Balloon() {
    SAFEDELETE(mTitleWindow);
    SAFEDELETE(mCloseBtnWindow);
    SAFEDELETE(mTitleSettings);
    SAFEDELETE(mCloseBtnSettings);
}


LRESULT WINAPI Balloon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID drawableWindow)
{
    if (message == WM_LBUTTONDOWN)
    {
        mCallbackHandler->HandleMessage(window, mClickedMessage, drawableWindow == mCloseBtnWindow ? 1 : NULL, NULL, drawableWindow);
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


void Balloon::Hide()
{
    mWindow->ClearOverlays();
    mWindow->Hide();
}


void Balloon::Show(LPCWSTR title, LPCWSTR text, HICON icon, LPSIZE iconSize, LPRECT targetPosition)
{
    int offsetLeft;
    if (icon != nullptr) {
        D2D1_RECT_F f = { 4, 4, float(4 + iconSize->cx), float(4 + iconSize->cy) };
        mWindow->AddOverlay(f, icon);

        offsetLeft = iconSize->cx + 8;
        mTitleWindow->Move(iconSize->cx + 8, 4);
    }
    else {
        offsetLeft = 4;
        mTitleWindow->Move(4, 4);
    }
    mWindow->SetTextOffsets((float)offsetLeft, 20, 4, 4);

    mTitleWindow->SetText(title);
    mTitleWindow->SizeToText(400, 16, 0, 16);

    mWindow->SetText(text);
    mWindow->SizeToText(400, 400, mTitleWindow->GetDrawingSettings()->width + offsetLeft + 24, 8 + iconSize->cy);

    mCloseBtnWindow->Move(mWindow->GetDrawingSettings()->width - 20, 4);

    // Show it centerd on x, 5 px above, while forcing it to stay on the virtual desktop
    MonitorInfo* monInfo = mWindow->GetMonitorInformation();
    mWindow->Move(

        std::min(std::max(monInfo->m_virtualDesktop.rect.left, targetPosition->left - mWindow->GetDrawingSettings()->width/2),
            monInfo->m_virtualDesktop.rect.right - mWindow->GetDrawingSettings()->width),
        std::min(std::max(monInfo->m_virtualDesktop.rect.top, targetPosition->top - mWindow->GetDrawingSettings()->height - 5),
            monInfo->m_virtualDesktop.rect.bottom - mWindow->GetDrawingSettings()->height));

    mWindow->Show();
}
