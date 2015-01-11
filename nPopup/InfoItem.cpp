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
#include "Popup.hpp"


InfoItem::InfoItem(Drawable* parent, LPCTSTR title, LPCTSTR customIcon)
    : PopupItem(parent, L"InfoItem", PopupItem::Type::Info)
    , title(_wcsdup(title))
{
    mWindow->Initialize(((Popup*)mParent)->mPopupSettings.mInfoWindowSettings, &((Popup*)mParent)->mPopupSettings.mInfoStateRender);
    mWindow->SetText(title);

    ParseDotIcon(customIcon);

    mWindow->Show();
}


InfoItem::~InfoItem()
{
    free((LPVOID)this->title);
}


int InfoItem::GetDesiredWidth(int maxWidth)
{
    SIZE s;
    mWindow->GetDesiredSize(maxWidth, (int)mWindow->GetSize().height, &s);
    return s.cx;
}


LRESULT InfoItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
        {
            ((Popup*)mParent)->mPopupSettings.mInfoStateRender.ActivateState(State::Hover, mWindow);
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            ((Popup*)mParent)->mPopupSettings.mInfoStateRender.ClearState(State::Hover, mWindow);
        }
        return 0;
    }
    return DefWindowProc(window, msg, wParam, lParam);
}
