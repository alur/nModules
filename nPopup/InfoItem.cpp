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


InfoItem::InfoItem(Drawable* parent, LPCTSTR title, LPCTSTR customIcon) : PopupItem(parent, L"InfoItem") {
    this->title = _tcsdup(title);
    this->itemType = PopupItemType::INFO;

    DrawableSettings defaults;
    defaults.width = 190;
    defaults.height = 20;
    defaults.evaluateText = true;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = Color::Create(0xAAFF00FF);
    defaultState.textBrush.color = Color::Create(0xFF000000);
    defaultState.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    defaultState.textOffsetLeft = 20;
    defaultState.textOffsetRight = 5;

    mWindow->Initialize(&defaults, &defaultState);
    mWindow->SetText(title);

    ParseDotIcon(customIcon);

    this->hoverState = mWindow->AddState(L"Hover", 100, &defaultState);

    mWindow->Show();
}


InfoItem::~InfoItem() {
    free((LPVOID)this->title);
}


int InfoItem::GetDesiredWidth(int maxWidth) {
    SIZE s;
    mWindow->GetDesiredSize(maxWidth, mWindow->GetDrawingSettings()->height, &s);
    return s.cx;
}


LRESULT InfoItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg) {
    case WM_MOUSEMOVE:
        {
            mWindow->ActivateState(this->hoverState);
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            mWindow->ClearState(this->hoverState);
        }
        return 0;
    }
    return DefWindowProc(window, msg, wParam, lParam);
}
