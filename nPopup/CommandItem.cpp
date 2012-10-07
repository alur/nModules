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


CommandItem::CommandItem(Drawable* parent, LPCSTR title, LPCSTR command, LPCSTR customIcon) : PopupItem(parent) {
    this->title = _strdup(title);
    this->command = _strdup(command);

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    defaults->color = 0xAAFFFF00;
    defaults->fontColor = 0xFF000000;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    StringCchCopy(defaults->textVerticalAlign, sizeof(defaults->textVerticalAlign), "Middle");
    defaults->textOffsetLeft = 20;
    this->window->Initialize(defaults);

    if (customIcon[0] != '\0') {
        D2D1_RECT_F f;
        LPSTR index = (LPSTR)strrchr(customIcon, ',');
        index++[0] = NULL;
        f.bottom = 18; f.top = 2; f.left = 2; f.right = 18;
        HICON icon = ExtractIcon(g_LSModule->GetInstance(), customIcon, atoi(index));
        if (icon != NULL) {
            this->window->AddOverlay(f, icon);
            DestroyIcon(icon);
        }
    }

    this->hoverState = this->window->AddState("Hover", new DrawableSettings(), 100);

    this->window->Show();
}


CommandItem::~CommandItem() {
    free((LPVOID)this->title);
    free((LPVOID)this->command);
}


LRESULT CommandItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        {
            LSExecute(NULL, this->command, SW_SHOW);
        }
        return 0;

    case WM_MOUSEMOVE:
        {
            this->window->ActivateState(this->hoverState);
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
