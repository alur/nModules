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


CommandItem::CommandItem(Drawable* parent, LPCSTR title, LPCSTR command, LPCSTR customIcon) : PopupItem(parent, "Item") {
    Init(title, command);
    ParseDotIcon(customIcon);
    this->window->Show();
}


CommandItem::CommandItem(Drawable* parent, LPCSTR title, LPCSTR command, HICON icon) : PopupItem(parent, "Item") {
    Init(title, command);
    if (icon != NULL) {
        AddIcon(icon);
    }
    this->window->Show();
}


void CommandItem::Init(LPCSTR title, LPCSTR command) {
    this->title = _strdup(title);
    this->command = _strdup(command);
    this->itemType = PopupItemType::COMMAND;

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));

    StateSettings* defaultState = new StateSettings();
    defaultState->backgroundBrush.color = 0xAAFFFF00;
    defaultState->textBrush.color = 0xFF000000;
    StringCchCopy(defaultState->textVerticalAlign, sizeof(defaultState->textVerticalAlign), "Middle");
    defaultState->textOffsetLeft = 20;
    defaultState->textOffsetRight = 5;

    this->window->Initialize(defaults, defaultState);
    this->hoverState = this->window->AddState("Hover", 100, new StateSettings(*defaultState));
}


CommandItem::~CommandItem() {
    free((LPVOID)this->title);
    free((LPVOID)this->command);
}


int CommandItem::GetDesiredWidth(int maxWidth) {
    SIZE s;
    this->window->GetDesiredSize(maxWidth, this->window->GetDrawingSettings()->height, &s);
    return s.cx;
}


LRESULT CommandItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        {
            LiteStep::LSExecute(NULL, this->command, SW_SHOW);
            ((Popup*)this->parent)->Close();
        }
        return 0;

    case WM_MOUSEMOVE:
        {
            this->window->ActivateState(this->hoverState);
            ((Popup*)this->parent)->CloseChild();
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
