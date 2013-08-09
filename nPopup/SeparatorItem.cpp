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


SeparatorItem::SeparatorItem(Drawable* parent) : PopupItem(parent, L"SeparatorItem") {
    DrawableSettings defaults;
    defaults.width = 190;
    defaults.height = 5;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = Color::Create(0xAA000000);
    defaultState.textBrush.color = Color::Create(0xFF000000);

    mWindow->Initialize(&defaults, &defaultState);

    this->hoverState = mWindow->AddState(L"Hover", 100, &defaultState);

    mWindow->Show();
}


SeparatorItem::~SeparatorItem() {
}


int SeparatorItem::GetDesiredWidth(int /* maxWidth */) {
    return 0;
}


LRESULT SeparatorItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    return DefWindowProc(window, msg, wParam, lParam);
}
