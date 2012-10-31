/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  SeparatorItem.cpp
 *  The nModules Project
 *
 *  Represents a separator.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "SeparatorItem.hpp"
#include "../nShared/LSModule.hpp"


extern LSModule* g_LSModule;


SeparatorItem::SeparatorItem(Drawable* parent) : PopupItem(parent, "SeparatorItem") {
    DrawableSettings defaults;
    defaults.width = 190;
    defaults.height = 5;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = 0xAA000000;
    defaultState.textBrush.color = 0xFF000000;

    this->window->Initialize(&defaults, &defaultState);

    this->hoverState = this->window->AddState("Hover", 100, &defaultState);

    this->window->Show();
}


SeparatorItem::~SeparatorItem() {
}


int SeparatorItem::GetDesiredWidth(int /* maxWidth */) {
    return 0;
}


LRESULT SeparatorItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    return DefWindowProc(window, msg, wParam, lParam);
}
