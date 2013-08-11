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
#include "../Utilities/Math.h"


ContainerItem::ContainerItem(Drawable* parent, LPCTSTR prefix) : PopupItem(parent, prefix, true) {
    this->itemType = PopupItemType::CONTAINER;

    WindowSettings defaults;
    defaults.registerWithCore = true;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = Color::Create(0xAAFFFFFF);
    defaultState.textBrush.color = Color::Create(0xFF000000);

    mWindow->Initialize(&defaults, &defaultState);

    this->hoverState = mWindow->AddState(L"Hover", 100, &defaultState);

    mWindow->Show();
}


ContainerItem::~ContainerItem() {
}


int ContainerItem::GetDesiredWidth(int maxWidth) {
    return min(maxWidth, mWindow->GetDrawingSettings()->width);
}


LRESULT ContainerItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg) {
    case WM_MOUSEMOVE:
        {
            if (!this->hoverState->active) {
                mWindow->ActivateState(this->hoverState);
                ((Popup*)mParent)->CloseChild();
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            mWindow->ClearState(this->hoverState);
        }
        return 0;
    }
    return DefWindowProc(window, msg, wParam, lParam);
}
