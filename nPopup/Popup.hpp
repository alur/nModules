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

    // Called by the parent


    // Called by items or children
    void Close();

    //
    LPCSTR GetBang();

    //
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

protected:
    //
    virtual void PreShow() = 0;
    virtual void PostClose() = 0;

    //
    void Size();

    //
    vector<PopupItem*> items;

private:
    //
    bool CheckFocus(HWND newActive, __int8 direction);

    //
    int itemSpacing;

    //
    RECT padding;

    //
    LPCSTR bang;

    // True if the popup is already sized properly.
    bool sized;

    // The currently open child, or NULL
    Popup* openChild;

    // The popup which opened this popup, or NULL
    Popup* owner;

    // True if the mouse is currently above the popup.
    bool mouseOver;
};
