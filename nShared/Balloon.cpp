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
    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 150;
    defaults->height = 40;
    defaults->color = 0xCCFAFAD2;
    defaults->fontColor = 0xFF000000;
    defaults->textOffsetTop = 4;
    defaults->textOffsetBottom = 4;
    defaults->textOffsetRight = 4;
    defaults->textOffsetLeft = 40;
    defaults->alwaysOnTop = true;
    defaults->outlineColor = 0xAA000000;
    defaults->outlineWidth = 1.5f;
    defaults->cornerRadiusX = 4.0f;
    defaults->cornerRadiusY = 4.0f;
    defaults->wordWrap = true;
    this->window->Initialize(defaults);

    this->clickedMessage = clickedMessage;
    this->callbackHandler = callbackHandler;

    //this->closeBtnWindow = new DrawableWindow(this->window, 

    this->titleSettings = parentSettings->CreateChild("Title");
    this->titleWindow = this->window->CreateChild(this->titleSettings, this);
    DrawableSettings* titleDefaults = new DrawableSettings();
    titleDefaults->x = 4;
    titleDefaults->y = 4;
    titleDefaults->fontColor = 0xFF000000;
    titleDefaults->color = 0;
    StringCchCopy(titleDefaults->fontWeight, sizeof(titleDefaults->fontWeight), "Bold");
    this->titleWindow->Initialize(titleDefaults);
    this->titleWindow->Show();

    this->closeBtnSettings = parentSettings->CreateChild("CloseButton");
    this->closeBtnWindow = this->window->CreateChild(this->closeBtnSettings, this);
    DrawableSettings* closeBtnDefaults = new DrawableSettings();
    closeBtnDefaults->color = 0xAA77AACC;
    closeBtnDefaults->fontColor = 0xFF000000;
    closeBtnDefaults->width = 16;
    closeBtnDefaults->height = 16;
    closeBtnDefaults->cornerRadiusX = 2.0f;
    closeBtnDefaults->cornerRadiusY = 2.0f;
    closeBtnDefaults->outlineColor = 0xFF000000;
    closeBtnDefaults->outlineWidth = 1.0f;
    StringCchCopy(closeBtnDefaults->fontWeight, sizeof(closeBtnDefaults->fontWeight), "Bold");
    StringCchCopy(closeBtnDefaults->textAlign, sizeof(closeBtnDefaults->textAlign), "Center");
    StringCchCopy(closeBtnDefaults->textVerticalAlign, sizeof(closeBtnDefaults->textVerticalAlign), "Middle");
    this->closeBtnWindow->Initialize(closeBtnDefaults);
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
    if (icon != NULL) {
        D2D1_RECT_F f = { 4, 4, float(4 + iconSize->cx), float(4 + iconSize->cy) };
        this->window->AddOverlay(f, icon);

        this->window->SetTextOffsets(float(iconSize->cx + 8), 20, 4, 4);
        this->titleWindow->Move(iconSize->cx + 8, 4);
    }
    else {
        this->window->SetTextOffsets(4, 20, 4, 4);
        this->titleWindow->Move(4, 4);
    }

    this->titleWindow->SetText(title);
    this->titleWindow->SizeToText(400, 16, 0, 16);

    this->window->SetText(text);
    this->window->SizeToText(400, 400, this->titleWindow->GetDrawingSettings()->width + (int)this->window->GetDrawingSettings()->textOffsetLeft + 24, 8 + iconSize->cy);

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
