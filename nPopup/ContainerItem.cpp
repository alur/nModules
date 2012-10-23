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

    DrawableSettings* defaults = new DrawableSettings();
    defaults->color = 0xAAFFFFFF;
    defaults->fontColor = 0xFF000000;
    defaults->registerWithCore = true;
    this->window->Initialize(defaults);

    this->hoverState = this->window->AddState("Hover", new DrawableSettings(*defaults), 100);

    this->window->Show();
}


ContainerItem::~ContainerItem() {
}


LRESULT ContainerItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEMOVE:
        {
            if (!this->hoverState->active) {
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
