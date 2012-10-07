/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "../nShared/LSModule.hpp"
#include "Label.hpp"
#include "../nShared/Macros.h"

extern LSModule* g_LSModule;

Label::Label(LPCSTR name) : Drawable(name) {
    this->name = name;
    this->settings = new Settings(name);
    this->window->Initialize(new DrawableSettings());
    this->window->Show();
}


Label::~Label() {
    free((LPVOID)this->name);
}


void Label::LoadSettings(bool /* bIsRefresh */) {
}


LRESULT WINAPI Label::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
