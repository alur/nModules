/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SeparatorItem.cpp
 *  The nModules Project
 *
 *  Represents a separator.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "SeparatorItem.hpp"
#include "../nShared/LSModule.hpp"


extern LSModule* g_LSModule;


SeparatorItem::SeparatorItem() : PopupItem() {

}


SeparatorItem::~SeparatorItem() {
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
}


LRESULT SeparatorItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return this->window->HandleMessage(window, msg, wParam, lParam);
}


void SeparatorItem::Init(Settings* parentSettings, DrawableWindow* parentWindow) {
    this->settings = parentSettings->CreateChild("SeparatorItem");

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 5;
    defaults->color = 0x55000000;
    this->window = new DrawableWindow(parentWindow->GetWindow(), (LPCSTR)g_LSModule->GetWindowClass(2), g_LSModule->GetInstance(), this->settings, defaults, this);
    this->window->Show();
}
