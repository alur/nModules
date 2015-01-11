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
#include <strsafe.h>

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

        // Used for lazy child popup creation
        struct CreationData
        {
            CreationData(LPCTSTR command, LPCTSTR name, LPCTSTR prefix)
            {
                StringCchCopy(this->command, _countof(this->command), command);
                StringCchCopy(this->name, _countof(this->name), name);
                StringCchCopy(this->prefix, _countof(this->prefix), prefix);
            }

            std::list<TCHAR[MAX_PATH]> paths;
            TCHAR command[MAX_LINE_LENGTH];
            TCHAR name[MAX_PATH];
            TCHAR prefix[MAX_RCCOMMAND];
        };

    public:
        explicit FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, LPCTSTR customIcon = nullptr);
        explicit FolderItem(Drawable* parent, LPCTSTR title, Popup* popup, HICON customIcon);
        explicit FolderItem(Drawable* parent, LPCTSTR title, std::function<Popup*(CreationData*)> popupCreator, CreationData* creationData);
        virtual ~FolderItem();

    private:
        explicit FolderItem(Drawable* parent, Popup* popup, LPCTSTR title);

    public:
        LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

        void ClosingPopup();
        int GetDesiredWidth(int maxWidth);
        Popup* GetPopup();
        void AddPath(LPCTSTR path);

    private:
        Popup *mPopup;
        LPCTSTR mTitle;
        std::function<Popup*(CreationData*)> mPopupCreator;
        CreationData *mCreationData;
    };
}
