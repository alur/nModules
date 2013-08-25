/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  PopupItem.cpp
 *  The nModules Project
 *
 *  Represents an item in a popup.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/LiteStep.h"
#include "../nShared/Drawable.hpp"
#include "../nShared/Window.hpp"
#include "../nShared/MessageHandler.hpp"
#include <Shlobj.h>

class PopupItem : public Drawable {
protected:
    enum class Type
    {
        Command = 0,
        Info,
        Folder,
        Container,
        Separator
    };

public:
    explicit PopupItem(Drawable* parent, LPCTSTR prefix, Type type, bool independent = false);
    virtual ~PopupItem();
    void Position(int x, int y);
    virtual LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID) = 0;
    int GetHeight();
    bool CompareTo(PopupItem* b);
    void SetIcon(IExtractIconW* extractIcon);
    void SetWidth(int width);
    virtual int GetDesiredWidth(int maxWidth) = 0;
    bool CheckMerge(LPCWSTR name);

protected:
    bool ParseDotIcon(LPCTSTR dotIcon);
    void AddIcon(HICON icon);

    Settings* iconSettings;

    Type mItemType;

    Window::OVERLAY iconOverlay;

private:
    UINT32 mIconHash;
    bool mHasIcon;
};
