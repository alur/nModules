/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  CommandItem.cpp
 *  The nModules Project
 *
 *  Represents a popup item which executes a command.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "CommandItem.hpp"
#include "../nShared/LSModule.hpp"
#include "Popup.hpp"


CommandItem::CommandItem(LPCTSTR title, LPCTSTR command, Drawable* parent)
    : PopupItem(parent, L"Item", PopupItem::Type::Command)
    , title(_wcsdup(title))
    , command(_wcsdup(command))
{
    mWindow->Initialize(((Popup*)mParent)->mPopupSettings.mCommandWindowSettings, &((Popup*)mParent)->mPopupSettings.mCommandStateRender);
    mWindow->SetText(title);
}


CommandItem::CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, LPCTSTR customIcon)
    : CommandItem(title, command, parent)
{
    ParseDotIcon(customIcon);
    mWindow->Show();
}


CommandItem::CommandItem(Drawable* parent, LPCTSTR title, LPCTSTR command, HICON icon)
    : CommandItem(title, command, parent)
{
    if (icon != nullptr)
    {
        AddIcon(icon);
    }
    mWindow->Show();
}


CommandItem::~CommandItem() {
    free((LPVOID)this->title);
    free((LPVOID)this->command);
}


int CommandItem::GetDesiredWidth(int maxWidth) {
    SIZE s;
    mWindow->GetDesiredSize(maxWidth, (int)mWindow->GetSize().height, &s);
    return s.cx;
}


LRESULT CommandItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID) {
    switch (msg) {
    case WM_LBUTTONDOWN:
        {
            LiteStep::LSExecute(NULL, this->command, SW_SHOW);
            ((Popup*)mParent)->Close();
        }
        return 0;

    case WM_RBUTTONDOWN:
        {

        }
        return 0;

    case WM_MOUSEMOVE:
        {
            ((Popup*)mParent)->mPopupSettings.mCommandStateRender.ActivateState(State::Hover, mWindow);
            ((Popup*)mParent)->CloseChild();
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            ((Popup*)mParent)->mPopupSettings.mCommandStateRender.ClearState(State::Hover, mWindow);
        }
        return 0;
    }
    return DefWindowProc(window, msg, wParam, lParam);
}
