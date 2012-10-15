/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/LSModule.hpp"
#include "Label.hpp"
#include "../nShared/Macros.h"

extern LSModule* g_LSModule;

Label::Label(LPCSTR name) : Drawable(name) {
    this->name = name;
    this->settings = new Settings(name);
    this->window->Initialize(new DrawableSettings());
    this->stateHover = this->window->AddState("Hover", new DrawableSettings(), 100);
    this->window->Show();
}


Label::~Label() {
    free((LPVOID)this->name);
}


void Label::LoadSettings(bool /* bIsRefresh */) {
}


LRESULT WINAPI Label::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_MOUSEMOVE) {
        this->window->ActivateState(this->stateHover);
    }
    else if (uMsg == WM_MOUSELEAVE) {
        this->window->ClearState(this->stateHover);
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
