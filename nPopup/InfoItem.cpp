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


InfoItem::InfoItem(Drawable* parent, LPCSTR title, LPCSTR customIcon) : PopupItem(parent, "InfoItem") {
    WCHAR titleWide[MAX_LINE_LENGTH];

    this->title = _strdup(title);
    this->itemType = PopupItemType::INFO;

    DrawableSettings defaults;
    defaults.width = 190;
    defaults.height = 20;
    defaults.evaluateText = true;
    MultiByteToWideChar(CP_ACP, 0, this->title, (int)strlen(this->title)+1, titleWide, MAX_LINE_LENGTH);

    StateSettings defaultState;
    defaultState.backgroundBrush.color = 0xAAFF00FF;
    defaultState.textBrush.color = 0xFF000000;
    defaultState.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    defaultState.textOffsetLeft = 20;
    defaultState.textOffsetRight = 5;

    this->window->Initialize(&defaults, &defaultState);
    this->window->SetText(titleWide);

    ParseDotIcon(customIcon);

    this->hoverState = this->window->AddState("Hover", 100, &defaultState);

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


LRESULT InfoItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
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
