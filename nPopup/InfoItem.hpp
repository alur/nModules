/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InfoItem.hpp
 *  The nModules Project
 *
 *  Represents a line of info.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class InfoItem : public PopupItem {
public:
    explicit InfoItem(LPCSTR title, LPCSTR customIcon = NULL);
    virtual ~InfoItem();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
};
