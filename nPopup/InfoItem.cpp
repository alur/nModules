/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InfoItem.cpp
 *  The nModules Project
 *
 *  Represents a line of info.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "InfoItem.hpp"
#include "../nShared/LSModule.hpp"


extern LSModule* g_LSModule;


InfoItem::InfoItem(LPCSTR title, LPCSTR customIcon) : PopupItem() {
    this->title = _strdup(title);
}


InfoItem::~InfoItem() {
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
    free((LPVOID)this->title);
}


LRESULT InfoItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return this->window->HandleMessage(window, msg, wParam, lParam);
}


void InfoItem::Init(Settings* parentSettings, DrawableWindow* parentWindow) {
    this->settings = parentSettings->CreateChild("Item");

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    defaults->color = 0x55FF00FF;
    MultiByteToWideChar(CP_ACP, 0, this->title, strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    StringCchCopy(defaults->textVerticalAlign, sizeof(defaults->textVerticalAlign), "Middle");
    defaults->textOffsetLeft = 20;
    this->window = new DrawableWindow(parentWindow->GetWindow(), (LPCSTR)g_LSModule->GetWindowClass(2), g_LSModule->GetInstance(), this->settings, defaults, this);
    this->window->Show();
}
