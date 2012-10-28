/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include "Balloon.hpp"
#include <strsafe.h>
#include "Macros.h"


Balloon::Balloon(LPCSTR prefix, Settings* parentSettings, UINT clickedMessage, MessageHandler* callbackHandler) : Drawable(prefix, parentSettings) {
    // Inialize the window.
    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 150;
    defaults->height = 40;
    defaults->alwaysOnTop = true;

    StateSettings* defaultState = new StateSettings();
    defaultState->color = 0xCCFAFAD2;
    defaultState->fontColor = 0xFF000000;
    defaultState->textOffsetTop = 4;
    defaultState->textOffsetBottom = 4;
    defaultState->textOffsetRight = 4;
    defaultState->textOffsetLeft = 40;
    defaultState->outlineColor = 0xAA000000;
    defaultState->outlineWidth = 1.5f;
    defaultState->wordWrap = true;
    defaultState->cornerRadiusX = 4.0f;
    defaultState->cornerRadiusY = 4.0f;

    this->window->Initialize(defaults, defaultState);

    this->clickedMessage = clickedMessage;
    this->callbackHandler = callbackHandler;

    // Create the a child window for the title of the balloon.
    this->titleSettings = parentSettings->CreateChild("Title");
    this->titleWindow = this->window->CreateChild(this->titleSettings, this);
    DrawableSettings* titleDefaults = new DrawableSettings();
    titleDefaults->x = 4;
    titleDefaults->y = 4;

    StateSettings* titleBaseStateDefaults = new StateSettings();
    titleBaseStateDefaults->fontColor = 0xFF000000;
    titleBaseStateDefaults->color = 0;
    StringCchCopy(titleBaseStateDefaults->fontWeight, sizeof(titleBaseStateDefaults->fontWeight), "Bold");

    this->titleWindow->Initialize(titleDefaults, titleBaseStateDefaults);
    this->titleWindow->Show();

    // And another for the x button in the top right corner.
    this->closeBtnSettings = parentSettings->CreateChild("CloseButton");
    this->closeBtnWindow = this->window->CreateChild(this->closeBtnSettings, this);
    DrawableSettings* closeBtnDefaults = new DrawableSettings();
    closeBtnDefaults->width = 16;
    closeBtnDefaults->height = 16;

    StateSettings* closeBtnBaseStateDefaults = new StateSettings();
    closeBtnBaseStateDefaults->color = 0xAA77AACC;
    closeBtnBaseStateDefaults->fontColor = 0xFF000000;
    closeBtnBaseStateDefaults->outlineColor = 0xFF000000;
    closeBtnBaseStateDefaults->outlineWidth = 1.0f;
    closeBtnBaseStateDefaults->cornerRadiusX = 2.0f;
    closeBtnBaseStateDefaults->cornerRadiusY = 2.0f;
    StringCchCopy(closeBtnBaseStateDefaults->fontWeight, sizeof(closeBtnBaseStateDefaults->fontWeight), "Bold");
    StringCchCopy(closeBtnBaseStateDefaults->textAlign, sizeof(closeBtnBaseStateDefaults->textAlign), "Center");
    StringCchCopy(closeBtnBaseStateDefaults->textVerticalAlign, sizeof(closeBtnBaseStateDefaults->textVerticalAlign), "Middle");

    this->closeBtnWindow->Initialize(closeBtnDefaults, closeBtnBaseStateDefaults);
    this->closeBtnWindow->SetText(L"X");
    this->closeBtnWindow->Show();

    // Not working...
    SetParent(this->window->GetWindowHandle(), NULL);
    SetWindowPos(this->window->GetWindowHandle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


Balloon::~Balloon() {
    SAFEDELETE(this->titleWindow);
    SAFEDELETE(this->closeBtnWindow);
    SAFEDELETE(this->titleSettings);
    SAFEDELETE(this->closeBtnSettings);
}


LRESULT WINAPI Balloon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID drawableWindow) {
    if (message == WM_LBUTTONDOWN) {
        this->callbackHandler->HandleMessage(window, this->clickedMessage, drawableWindow == this->closeBtnWindow ? 1 : NULL, NULL, drawableWindow);
        return 0;
    }
    return DefWindowProc(window, message, wParam, lParam);
}


void Balloon::Show(LPCWSTR title, LPCWSTR text, HICON icon, LPSIZE iconSize, LPRECT targetPosition) {
    int offsetLeft;
    if (icon != NULL) {
        D2D1_RECT_F f = { 4, 4, float(4 + iconSize->cx), float(4 + iconSize->cy) };
        this->window->AddOverlay(f, icon);

        offsetLeft = iconSize->cx + 8;
        this->titleWindow->Move(iconSize->cx + 8, 4);
    }
    else {
        offsetLeft = 4;
        this->titleWindow->Move(4, 4);
    }
    this->window->SetTextOffsets((float)offsetLeft, 20, 4, 4);

    this->titleWindow->SetText(title);
    this->titleWindow->SizeToText(400, 16, 0, 16);

    this->window->SetText(text);
    this->window->SizeToText(400, 400, this->titleWindow->GetDrawingSettings()->width + offsetLeft + 24, 8 + iconSize->cy);

    this->closeBtnWindow->Move(this->window->GetDrawingSettings()->width - 20, 4);

    // Show it centerd on x, 5 px above, while forcing it to stay on the virtual desktop
    MonitorInfo* monInfo = this->window->GetMonitorInformation();
    this->window->Move(
        min(max(monInfo->m_virtualDesktop.rect.left, targetPosition->left - this->window->GetDrawingSettings()->width/2),
            monInfo->m_virtualDesktop.rect.right - this->window->GetDrawingSettings()->width),
        min(max(monInfo->m_virtualDesktop.rect.top, targetPosition->top - this->window->GetDrawingSettings()->height - 5),
            monInfo->m_virtualDesktop.rect.bottom - this->window->GetDrawingSettings()->height));

    this->window->Show();
}


void Balloon::Hide() {
    this->window->ClearOverlays();
    this->window->Hide();
}
