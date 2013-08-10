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


nPopup::FolderItem::FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, LPCTSTR customIcon) : PopupItem(parent, L"FolderItem")
{
    Init(title, popup);
    ParseDotIcon(customIcon);
    mWindow->Show();
}


nPopup::FolderItem::FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, HICON icon) : PopupItem(parent, L"FolderItem")
{
    Init(title, popup);
    AddIcon(icon);
    mWindow->Show();
}


void nPopup::FolderItem::Init(LPCTSTR title, Popup* popup)
{
    this->title = _tcsdup(title);
    this->popup = popup;
    this->itemType = PopupItemType::FOLDER;

    DrawableSettings defaults;
    defaults.width = 190;
    defaults.height = 20;

    StateSettings defaultState;
    defaultState.backgroundBrush.color = Color::Create(0xAA00FFFF);
    defaultState.textBrush.color = Color::Create(0xFF000000);
    defaultState.textVerticalAlign = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
    defaultState.textOffsetLeft = 20;
    defaultState.textOffsetRight = 5;

    mWindow->Initialize(&defaults, &defaultState);
    mWindow->SetText(title);

    this->hoverState = mWindow->AddState(L"Hover", 100, &defaultState);
    this->openState = mWindow->AddState(L"Open", 80, &defaultState);
}


nPopup::FolderItem::~FolderItem()
{
    if (this->popup != NULL)
    {
        delete this->popup;
    }
    free((LPVOID)this->title);
}


int nPopup::FolderItem::GetDesiredWidth(int maxWidth)
{
    SIZE s;
    mWindow->GetDesiredSize(maxWidth, mWindow->GetDrawingSettings()->height, &s);
    return s.cx;
}


LRESULT nPopup::FolderItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
        {
            mWindow->ActivateState(this->hoverState);
            if (this->popup != NULL)
            {
                RECT r;
                mWindow->GetScreenRect(&r);
                ((Popup*)mParent)->OpenChild(this->popup, &r, this);
                mWindow->ActivateState(this->openState);
            }
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


/// <summary>
/// Called by the parent when it is closing the popup for whatever reason.
/// </summary>
void nPopup::FolderItem::ClosingPopup()
{
    mWindow->ClearState(this->openState);
}


/// <summary>
/// 
/// </summary>
Popup* nPopup::FolderItem::GetPopup()
{
    return this->popup;
}
