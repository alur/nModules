/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Tooltip.hpp"


Tooltip::Tooltip(LPCSTR prefix) : Drawable(prefix) {
    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 150;
    defaults->height = 40;
    defaults->color = 0xCCFAFAD2;
    defaults->fontColor = 0xFF000000;
    defaults->textOffsetTop = 2;
    defaults->textOffsetBottom = 2;
    defaults->textOffsetRight = 2;
    defaults->textOffsetLeft = 2;
    defaults->alwaysOnTop = true;
    this->window->Initialize(defaults);

    this->maxHeight = settings->GetInt("MaxHeight", 100);
    this->maxWidth = settings->GetInt("MaxWidth", 300);

    // Not working...
    SetParent(this->window->GetWindow(), NULL);
    SetWindowPos(this->window->GetWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


Tooltip::~Tooltip() {
}


void Tooltip::Show(LPCWSTR text, int x, int y, int duration) {
    this->window->SetText(text);
    this->window->SizeToText(this->maxWidth, this->maxHeight);

    // Show it centerd on x, 5 px above, while forcing it to stay on the virtual desktop
    MonitorInfo* monInfo = this->window->GetMonitorInformation();
    this->window->Move(
        min(max(monInfo->m_virtualDesktop.rect.left, x - this->window->GetDrawingSettings()->width/2),
            monInfo->m_virtualDesktop.rect.right - this->window->GetDrawingSettings()->width),
        min(max(monInfo->m_virtualDesktop.rect.top, y - this->window->GetDrawingSettings()->height - 5),
            monInfo->m_virtualDesktop.rect.bottom - this->window->GetDrawingSettings()->height));
    this->window->Show();
}


void Tooltip::Hide() {
    this->window->Hide();
}


void Tooltip::LoadSettings(bool /*bIsRefresh*/) {

}


LRESULT WINAPI Tooltip::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(window, message, wParam, lParam);
}
