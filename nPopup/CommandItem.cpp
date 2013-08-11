/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CommandItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "CommandItem.hpp"
#include "../nShared/LSModule.hpp"
#include "Popup.hpp"


CommandItem::CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, LPCTSTR customIcon) : PopupItem(parent, L"Item") {
    Init(title, command);
    ParseDotIcon(customIcon);
    mWindow->Show();
}


CommandItem::CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, HICON icon) : PopupItem(parent, L"Item") {
    Init(title, command);
    if (icon != NULL) {
        AddIcon(icon);
    }
    mWindow->Show();
}


void CommandItem::Init(LPCTSTR title, LPCTSTR command) {
    this->title = _tcsdup(title);
    this->command = _tcsdup(command);
    this->itemType = PopupItemType::COMMAND;

    WindowSettings defaults;
    defaults.width = 190;
    defaults.height = 20;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = Color::Create(0xAAFFFF00);
    defaultState.textBrush.color = Color::Create(0xFF000000);
    defaultState.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    defaultState.textOffsetLeft = 20;
    defaultState.textOffsetRight = 5;

    mWindow->Initialize(&defaults, &defaultState);
    mWindow->SetText(title);

    this->hoverState = mWindow->AddState(L"Hover", 100, &defaultState);
}


CommandItem::~CommandItem() {
    free((LPVOID)this->title);
    free((LPVOID)this->command);
}


int CommandItem::GetDesiredWidth(int maxWidth) {
    SIZE s;
    mWindow->GetDesiredSize(maxWidth, mWindow->GetDrawingSettings()->height, &s);
    return s.cx;
}


LRESULT CommandItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        {
            LiteStep::LSExecute(NULL, this->command, SW_SHOW);
            ((Popup*)mParent)->Close();
        }
        return 0;

    case WM_RBUTTONDOWN:
        {

        }
        return 0;

    case WM_MOUSEMOVE:
        {
            mWindow->ActivateState(this->hoverState);
            ((Popup*)mParent)->CloseChild();
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
