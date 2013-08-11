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

namespace nPopup
{
    class FolderItem : public PopupItem
    {
    public:
        explicit FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, LPCTSTR customIcon = nullptr);
        explicit FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, HICON customIcon);
        virtual ~FolderItem();

        LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

        void ClosingPopup();
        int GetDesiredWidth(int maxWidth);
        Popup* GetPopup();

    private:
        void Init(LPCTSTR title, Popup* popup);

        Popup* popup;
        LPCTSTR title;

        Window::STATE hoverState;
        Window::STATE openState;
    };
}
