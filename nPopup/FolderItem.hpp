/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  FolderItem.hpp
 *  The nModules Project
 *
 *  A static folder.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"
//#include "Popup.hpp"

class Popup;

namespace nPopup
{
    class FolderItem : public PopupItem
    {
    public:
        enum class State
        {
            Base = 0,
            Open,
            Hover,
            Count
        };

    public:
        explicit FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, LPCTSTR customIcon = nullptr);
        explicit FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, HICON customIcon);
        explicit FolderItem(Drawable* parent, LPCTSTR title, std::function<Popup*(LPVOID)> popupCreator, LPVOID creationData);
        virtual ~FolderItem();

    private:
        explicit FolderItem(Drawable* parent, Popup* popup, LPCTSTR title);

    public:
        LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

        void ClosingPopup();
        int GetDesiredWidth(int maxWidth);
        Popup* GetPopup();

    private:
        Popup *mPopup;
        LPCTSTR mTitle;
        std::function<Popup*(LPVOID)> mPopupCreator;
        LPVOID mCreationData;
    };
}
