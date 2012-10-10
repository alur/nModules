/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderItem.hpp
 *  The nModules Project
 *
 *  A static folder.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

// Unfortunately there is a naming collition with shlobj :/
#define __FolderItem_INTERFACE_DEFINED__
#define __FolderItem2_INTERFACE_DEFINED__
#define __FolderItem_FWD_DEFINED__

#include "PopupItem.hpp"
#include "Popup.hpp"

class FolderItem : public PopupItem {
public:
    explicit FolderItem(Drawable* parent, LPCSTR title, Popup* popup, LPCSTR customIcon = NULL);
    explicit FolderItem(Drawable* parent, LPCSTR title, Popup* popup, HICON customIcon);
    virtual ~FolderItem();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
    void Init(LPCSTR title, Popup* popup);

    Popup* popup;
    LPCSTR title;

    DrawableWindow::STATE hoverState;
};
