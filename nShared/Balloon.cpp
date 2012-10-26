/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Balloon.hpp"
#include <strsafe.h>


Balloon::Balloon(LPCSTR prefix, Settings* parentSettings) : Drawable(prefix, parentSettings) {
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
    this->window->Initialize(defaults);

    // Not working...
    SetParent(this->window->GetWindow(), NULL);
    SetWindowPos(this->window->GetWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


Balloon::~Balloon() {

}


LRESULT WINAPI Balloon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(window, message, wParam, lParam);
}


void Balloon::Show(LPCWSTR title, LPCWSTR text, HICON icon, LPSIZE iconSize, LPRECT targetPosition) {
    if (icon != NULL) {
        DrawableWindow::OVERLAY overlay;
        D2D1_RECT_F f = { 4, 4, 36, 36 };
        this->window->AddOverlay(f, icon, &overlay);
    }

    WCHAR buf[1024];
    StringCchPrintfW(buf, 1024, L"%s\n%s", title, text);

    this->window->SetText(buf);
    this->window->SizeToText(250, 100, 0, 40);

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
