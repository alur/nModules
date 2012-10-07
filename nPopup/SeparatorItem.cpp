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


SeparatorItem::SeparatorItem(Drawable* parent) : PopupItem(parent) {
    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 5;
    defaults->color = 0xAA000000;
    defaults->fontColor = 0xFF000000;
    this->window->Initialize(defaults);
    this->window->Show();
}


SeparatorItem::~SeparatorItem() {
}


LRESULT SeparatorItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(window, msg, wParam, lParam);
}
