/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CommandItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "CommandItem.hpp"
#include "../nShared/LSModule.hpp"


extern LSModule* g_LSModule;


CommandItem::CommandItem(LPCSTR title, LPCSTR command, LPCSTR customIcon) : PopupItem() {
    this->title = _strdup(title);
    this->command = _strdup(command);
}


CommandItem::~CommandItem() {
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
    free((LPVOID)this->title);
    free((LPVOID)this->command);
}


LRESULT CommandItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        LSExecute(NULL, this->command, SW_SHOW);
        return 0;
    }
    return this->window->HandleMessage(window, msg, wParam, lParam);
}


void CommandItem::Init(Settings* parentSettings, DrawableWindow* parentWindow) {
    this->settings = parentSettings->CreateChild("Item");

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    defaults->color = 0x55FFFF00;
    MultiByteToWideChar(CP_ACP, 0, this->title, strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    StringCchCopy(defaults->textVerticalAlign, sizeof(defaults->textVerticalAlign), "Middle");
    defaults->textOffsetLeft = 20;
    this->window = new DrawableWindow(parentWindow->GetWindow(), (LPCSTR)g_LSModule->GetWindowClass(2), g_LSModule->GetInstance(), this->settings, defaults, this);
    this->window->Show();
}
