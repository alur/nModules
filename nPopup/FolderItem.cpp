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
#include "Popup.hpp"
#include "ContentPopup.hpp"


nPopup::FolderItem::FolderItem(Drawable* parent, Popup* popup, LPCTSTR title)
    : PopupItem(parent, L"FolderItem", PopupItem::Type::Folder)
    , mPopup(popup)
    , mPopupCreator(nullptr)
    , mCreationData(nullptr)
    , mTitle(_wcsdup(title))
{
    mWindow->Initialize(((Popup*)mParent)->mPopupSettings.mFolderWindowSettings, &((Popup*)mParent)->mPopupSettings.mFolderStateRender);
    mWindow->SetText(title);
}


nPopup::FolderItem::FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, LPCTSTR customIcon)
    : FolderItem(parent, popup, title)
{
    ParseDotIcon(customIcon);
    mWindow->Show();
}


nPopup::FolderItem::FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, HICON icon)
    : FolderItem(parent, popup, title)
{
    AddIcon(icon);
    mWindow->Show();
}


nPopup::FolderItem::FolderItem(Drawable* parent, LPCTSTR title, std::function<Popup*(CreationData*)> popupCreator, CreationData* creationData)
    : FolderItem(parent, nullptr, title)
{
    mPopupCreator = popupCreator;
    mCreationData = creationData;
    mWindow->Show();
}


nPopup::FolderItem::~FolderItem()
{
    delete mPopup;
    delete mCreationData;
    free((LPVOID)mTitle);
}


int nPopup::FolderItem::GetDesiredWidth(int maxWidth)
{
    SIZE s;
    mWindow->GetDesiredSize(maxWidth, (int)mWindow->GetSize().height, &s);
    return s.cx;
}


LRESULT nPopup::FolderItem::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam, LPVOID)
{
    switch (msg)
    {
    case WM_MOUSEMOVE:
        {
            ((Popup*)mParent)->mPopupSettings.mFolderStateRender.ActivateState(State::Hover, mWindow);
            if (GetPopup())
            {
                RECT r;
                mWindow->GetScreenRect(&r);
                ((Popup*)mParent)->OpenChild(mPopup, &r, this);
                ((Popup*)mParent)->mPopupSettings.mFolderStateRender.ActivateState(State::Open, mWindow);
            }
        }
        return 0;

    case WM_MOUSELEAVE:
        {
            ((Popup*)mParent)->mPopupSettings.mFolderStateRender.ClearState(State::Hover, mWindow);
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
    ((Popup*)mParent)->mPopupSettings.mFolderStateRender.ClearState(State::Open, mWindow);
}


void nPopup::FolderItem::AddPath(LPCTSTR path)
{
    if (mPopup)
    {
        ((ContentPopup*) mPopup)->AddPath(path);
    }
    else if (mCreationData)
    {
        mCreationData->paths.emplace_back();
        StringCchCopy(mCreationData->paths.back(), MAX_PATH, path);
    }
}


/// <summary>
/// 
/// </summary>
Popup* nPopup::FolderItem::GetPopup()
{
    if (!mPopup && mPopupCreator)
    {
        mPopup = mPopupCreator(mCreationData);
        delete mCreationData;
        mCreationData = nullptr;
        mPopupCreator = nullptr;
    }
    return mPopup;
}
