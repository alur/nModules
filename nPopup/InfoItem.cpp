/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InfoItem.cpp
 *  The nModules Project
 *
 *  Represents a line of info.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "InfoItem.hpp"
#include "../nShared/LSModule.hpp"


extern LSModule* g_LSModule;


InfoItem::InfoItem(Drawable* parent, LPCSTR title, LPCSTR customIcon) : PopupItem(parent, "InfoItem") {
    this->title = _strdup(title);
    this->itemType = PopupItemType::INFO;

    DrawableSettings* defaults = new DrawableSettings();
    defaults->width = 190;
    defaults->height = 20;
    defaults->color = 0xAAFF00FF;
    defaults->evaluateText = true;
    defaults->fontColor = 0xFF000000;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, defaults->text, sizeof(defaults->text)/sizeof(defaults->text[0]));
    StringCchCopy(defaults->textVerticalAlign, sizeof(defaults->textVerticalAlign), "Middle");
    defaults->textOffsetLeft = 20;
    defaults->textOffsetRight = 5;
    this->window->Initialize(defaults);

    ParseDotIcon(customIcon);

    this->hoverState = this->window->AddState("Hover", new DrawableSettings(*defaults), 100);

    this->window->Show();
}


InfoItem::~InfoItem() {
    free((LPVOID)this->title);
}


int InfoItem::GetDesiredWidth(int maxWidth) {
    SIZE s;
    this->window->GetDesiredSize(maxWidth, this->window->GetDrawingSettings()->height, &s);
    return s.cx;
}


LRESULT InfoItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
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
