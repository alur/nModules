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
    void Show();
    void Show(int x, int y);
    LPCSTR GetBang();
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
    vector<PopupItem*> items;
    LPCSTR bang;

    // True if the popup is already sized properly.
    bool sized;
};
