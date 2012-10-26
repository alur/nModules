/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Balloon.cpp
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Balloon.hpp"


Balloon::Balloon(LPCSTR prefix, Settings* parentSettings) : Tooltip(prefix, parentSettings) {
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

    // Not working...
    SetParent(this->window->GetWindow(), NULL);
    SetWindowPos(this->window->GetWindow(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}


Balloon::~Balloon() {
}



LRESULT WINAPI Balloon::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(window, message, wParam, lParam);
}
