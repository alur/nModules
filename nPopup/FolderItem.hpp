/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderItem.hpp
 *  The nModules Project
 *
 *  A static folder.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"
#include "Popup.hpp"

namespace nPopup {
    class FolderItem : public PopupItem {
    public:
        explicit FolderItem(Drawable* parent, LPCSTR title, Popup* popup, LPCSTR customIcon = NULL);
        explicit FolderItem(Drawable* parent, LPCSTR title, Popup* popup, HICON customIcon);
        virtual ~FolderItem();

        LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

        void ClosingPopup();

    private:
        void Init(LPCSTR title, Popup* popup);

        Popup* popup;
        LPCSTR title;

        DrawableWindow::STATE hoverState;
        DrawableWindow::STATE openState;
    };
}
