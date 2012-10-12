/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Popup.h
 *  The nModules Project
 *
 *  Represents a popup box.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"
#include <vector>
#include "../nShared/MessageHandler.hpp"
#include "../nShared/Settings.hpp"
#include "../nShared/DrawableWindow.hpp"

using std::vector;

class Popup : public Drawable {
public:
    explicit Popup(LPCSTR title, LPCSTR bang, LPCSTR prefix);
    virtual ~Popup();

    void AddItem(PopupItem* item);
    void RemoveItem(PopupItem* item);
    void Show();
    void Show(int x, int y, Popup* owner = NULL);
    
    // Called by PopupItems
    void OpenChild(Popup*, int y, int x);
    void CloseChild();

    // Called by child popups
    void ChildClosing(bool closeParent);
    void HandleInactivate(HWND newActive);

    // Called by items or children
    void Close(bool closeAll = true);

    //
    LPCSTR GetBang();

    //
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

protected:
    //
    virtual void PreShow() = 0;
    virtual void PostClose() = 0;

    //
    vector<PopupItem*> items;

private:
    int itemSpacing;

    RECT padding;

    //
    LPCSTR bang;

    // True if the popup is already sized properly.
    bool sized;

    // The currently open child, or NULL
    Popup* openChild;

    //
    Popup* owner;

    //
    bool mouseOver;
};
