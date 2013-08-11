/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Tooltip.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Tooltip.hpp"
#include <algorithm>


Tooltip::Tooltip(LPCTSTR prefix, Settings* parentSettings) : Drawable(prefix, parentSettings) {
    WindowSettings defaults;
    defaults.width = 150;
    defaults.height = 40;
    defaults.alwaysOnTop = true;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = Color::Create(0xCCFAFAD2);
    defaultState.textBrush.color = Color::Create(0xFF000000);
    defaultState.textOffsetTop = 2;
    defaultState.textOffsetBottom = 2;
    defaultState.textOffsetRight = 2;
    defaultState.textOffsetLeft = 2;
    defaultState.outlineBrush.color = Color::Create(0xFF000000);
    defaultState.outlineWidth = 0.75f;

    mWindow->Initialize(&defaults, &defaultState);

    this->maxHeight = mSettings->GetInt(_T("MaxHeight"), 100);
    this->maxWidth = mSettings->GetInt(_T("MaxWidth"), 300);
}


Tooltip::~Tooltip() {
}


void Tooltip::Show(LPCWSTR text, LPRECT position) {
    mWindow->SetText(text);
    mWindow->SizeToText(this->maxWidth, this->maxHeight);

    // Show it centerd on x, 5 px above, while forcing it to stay on the virtual desktop
    MonitorInfo* monInfo = mWindow->GetMonitorInformation();
    mWindow->Move(
        std::min(std::max(monInfo->m_virtualDesktop.rect.left, position->left + (position->right - position->left)/2 - mWindow->GetDrawingSettings()->width/2),
            monInfo->m_virtualDesktop.rect.right - mWindow->GetDrawingSettings()->width),
        std::min(std::max(monInfo->m_virtualDesktop.rect.top, position->top - mWindow->GetDrawingSettings()->height - 5),
            monInfo->m_virtualDesktop.rect.bottom - mWindow->GetDrawingSettings()->height));

    mWindow->Show();
}


void Tooltip::Hide() {
    mWindow->Hide();
}


void Tooltip::LoadSettings(bool /*bIsRefresh*/) {

}


LRESULT WINAPI Tooltip::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    return DefWindowProc(window, message, wParam, lParam);
}
