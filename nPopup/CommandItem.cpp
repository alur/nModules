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
    defaults->color = 0xAAFFFF00;
    defaults->fontColor = 0xFF000000;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    StringCchCopy(defaults->textVerticalAlign, sizeof(defaults->textVerticalAlign), "Middle");
    defaults->textOffsetLeft = 20;
    this->window->Initialize(defaults);
    this->hoverState = this->window->AddState("Hover", new DrawableSettings(*defaults), 100);
}


CommandItem::~CommandItem() {
    free((LPVOID)this->title);
    free((LPVOID)this->command);
}


LRESULT CommandItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        {
            ((Popup*)this->parent)->Close();
            LiteStep::LSExecute(NULL, this->command, SW_SHOW);
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
