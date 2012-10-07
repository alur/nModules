/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "FolderItem.hpp"
#include "../nShared/LSModule.hpp"


extern LSModule* g_LSModule;


FolderItem::FolderItem(Drawable* parent, LPCSTR title, Popup* popup, LPCSTR customIcon) : PopupItem(parent) {
    this->popup = popup;
    this->title = _strdup(title);

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    defaults->color = 0xAA00FFFF;
    defaults->fontColor = 0xFF000000;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    StringCchCopy(defaults->textVerticalAlign, sizeof(defaults->textVerticalAlign), "Middle");
    defaults->textOffsetLeft = 20;
    this->window->Initialize(defaults);

    this->hoverState = this->window->AddState("Hover", new DrawableSettings(), 100);

    this->window->Show();
}


FolderItem::~FolderItem() {
    if (this->popup != NULL) {
        delete this->popup;
    }
    free((LPVOID)this->title);
}


LRESULT FolderItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEMOVE:
        {
            this->window->ActivateState(this->hoverState);
            if (this->popup != NULL) {
                RECT r;
                this->window->GetScreenRect(&r);
                this->popup->Show(r.right, r.top);
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
