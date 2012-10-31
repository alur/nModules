/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ContainerItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which can hold child windows.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "ContainerItem.hpp"
#include "../nShared/LSModule.hpp"
#include "Popup.hpp"


ContainerItem::ContainerItem(Drawable* parent, LPCSTR prefix) : PopupItem(parent, prefix, true) {
    this->itemType = PopupItemType::CONTAINER;

    DrawableSettings defaults;
    defaults.registerWithCore = true;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = 0xAAFFFFFF;
    defaultState.textBrush.color = 0xFF000000;

    this->window->Initialize(&defaults, &defaultState);

    this->hoverState = this->window->AddState("Hover", 100, &defaultState);

    this->window->Show();
}


ContainerItem::~ContainerItem() {
}


int ContainerItem::GetDesiredWidth(int maxWidth) {
    return min(maxWidth, this->window->GetDrawingSettings()->width);
}


LRESULT ContainerItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg) {
    case WM_MOUSEMOVE:
        {
            if (!(*this->hoverState)->active) {
                this->window->ActivateState(this->hoverState);
                ((Popup*)this->parent)->CloseChild();
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            this->window->ClearState(this->hoverState);
        }
        return 0;
    }
    return DefWindowProc(window, msg, wParam, lParam);
}
