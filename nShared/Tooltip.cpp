/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Tooltip.hpp"


Tooltip::Tooltip(LPCSTR prefix, Settings* parentSettings) : Drawable(prefix, parentSettings) {
    DrawableSettings defaults;
    defaults.width = 150;
    defaults.height = 40;
    defaults.alwaysOnTop = true;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = 0xCCFAFAD2;
    defaultState.textBrush.color = 0xFF000000;
    defaultState.textOffsetTop = 2;
    defaultState.textOffsetBottom = 2;
    defaultState.textOffsetRight = 2;
    defaultState.textOffsetLeft = 2;
    defaultState.outlineBrush.color = 0xFF000000;
    defaultState.outlineWidth = 1.5f;

    this->window->Initialize(&defaults, &defaultState);

    this->maxHeight = settings->GetInt("MaxHeight", 100);
    this->maxWidth = settings->GetInt("MaxWidth", 300);
}


Tooltip::~Tooltip() {
}


void Tooltip::Show(LPCWSTR text, LPRECT position) {
    this->window->SetText(text);
    this->window->SizeToText(this->maxWidth, this->maxHeight);

    // Show it centerd on x, 5 px above, while forcing it to stay on the virtual desktop
    MonitorInfo* monInfo = this->window->GetMonitorInformation();
    this->window->Move(
        min(max(monInfo->m_virtualDesktop.rect.left, position->left + (position->right - position->left)/2 - this->window->GetDrawingSettings()->width/2),
            monInfo->m_virtualDesktop.rect.right - this->window->GetDrawingSettings()->width),
        min(max(monInfo->m_virtualDesktop.rect.top, position->top - this->window->GetDrawingSettings()->height - 5),
            monInfo->m_virtualDesktop.rect.bottom - this->window->GetDrawingSettings()->height));

    this->window->Show();
}


void Tooltip::Hide() {
    this->window->Hide();
}


void Tooltip::LoadSettings(bool /*bIsRefresh*/) {

}


LRESULT WINAPI Tooltip::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    return DefWindowProc(window, message, wParam, lParam);
}
