/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "FolderItem.hpp"
#include "../nShared/LSModule.hpp"


extern LSModule* g_LSModule;


nPopup::FolderItem::FolderItem(Drawable* parent, LPCSTR title, Popup* popup, LPCSTR customIcon) : PopupItem(parent, "FolderItem") {
    Init(title, popup);
    ParseDotIcon(customIcon);
    this->window->Show();
}


nPopup::FolderItem::FolderItem(Drawable* parent, LPCSTR title, Popup* popup, HICON icon) : PopupItem(parent, "FolderItem") {
    Init(title, popup);
    AddIcon(icon);
    this->window->Show();
}


void nPopup::FolderItem::Init(LPCSTR title, Popup* popup) {
    this->title = _strdup(title);
    this->popup = popup;
    this->itemType = PopupItemType::FOLDER;

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));

    StateSettings* defaultState = new StateSettings();
    defaultState->backgroundBrush.color = 0xAA00FFFF;
    defaultState->textBrush.color = 0xFF000000;
    StringCchCopy(defaultState->textVerticalAlign, sizeof(defaultState->textVerticalAlign), "Middle");
    defaultState->textOffsetLeft = 20;
    defaultState->textOffsetRight = 5;

    this->window->Initialize(defaults, defaultState);

    this->hoverState = this->window->AddState("Hover", 100, new StateSettings(*defaultState));
    this->openState = this->window->AddState("Open", 80, new StateSettings(*defaultState));
}


nPopup::FolderItem::~FolderItem() {
    if (this->popup != NULL) {
        delete this->popup;
    }
    free((LPVOID)this->title);
}


int nPopup::FolderItem::GetDesiredWidth(int maxWidth) {
    SIZE s;
    this->window->GetDesiredSize(maxWidth, this->window->GetDrawingSettings()->height, &s);
    return s.cx;
}


LRESULT nPopup::FolderItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg) {
    case WM_MOUSEMOVE:
        {
            this->window->ActivateState(this->hoverState);
            if (this->popup != NULL) {
                RECT r;
                this->window->GetScreenRect(&r);
                ((Popup*)this->parent)->OpenChild(this->popup, &r, this);
                this->window->ActivateState(this->openState);
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


/// <summary>
/// Called by the parent when it is closing the popup for whatever reason.
/// </summary>
void nPopup::FolderItem::ClosingPopup() {
    this->window->ClearState(this->openState);
}
