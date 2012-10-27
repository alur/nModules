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
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/MessageHandler.hpp"
#include <Shlobj.h>

class PopupItem : public Drawable {
public:
    explicit PopupItem(Drawable* parent, LPCSTR prefix, bool independent = false);
    virtual ~PopupItem();
    void Position(int x, int y);
    virtual LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID) = 0;
    int GetHeight();
    bool CompareTo(PopupItem* b);
    void SetIcon(IExtractIconW* extractIcon);
    void SetWidth(int width);
    virtual int GetDesiredWidth(int maxWidth) = 0;

protected:
    enum PopupItemType {
        COMMAND = 0,
        INFO,
        FOLDER,
        CONTAINER,
        SEPARATOR
    };
    
    bool ParseDotIcon(LPCSTR dotIcon);
    void AddIcon(HICON icon);

    Settings* iconSettings;

    PopupItemType itemType;

    DrawableWindow::OVERLAY iconOverlay;
};
