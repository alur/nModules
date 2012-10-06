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


InfoItem::InfoItem(Drawable* parent, LPCSTR title, LPCSTR customIcon) : PopupItem(parent) {
    this->title = _strdup(title);

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    defaults->color = 0xAAFF00FF;
    defaults->fontColor = 0xFF000000;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    StringCchCopy(defaults->textVerticalAlign, sizeof(defaults->textVerticalAlign), "Middle");
    defaults->textOffsetLeft = 20;
    this->window->Initialize(defaults);
    this->window->Show();
}


InfoItem::~InfoItem() {
    SAFEDELETE(this->window);
    SAFEDELETE(this->settings);
    free((LPVOID)this->title);
}


LRESULT InfoItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    return this->window->HandleMessage(window, msg, wParam, lParam);
}
