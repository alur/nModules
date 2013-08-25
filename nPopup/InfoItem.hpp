/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  InfoItem.hpp
 *  The nModules Project
 *
 *  Represents a line of info.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "PopupItem.hpp"

class InfoItem : public PopupItem
{
public:
    enum class State
    {
        Base = 0,
        Hover,
        Count
    };

public:
    explicit InfoItem(Drawable* parent, LPCTSTR title, LPCTSTR customIcon = nullptr);
    virtual ~InfoItem();
    
    int GetDesiredWidth(int maxWidth);
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

private:
    LPCTSTR title;
};
