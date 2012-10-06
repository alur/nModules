/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.hpp
 *  The nModules Project
 *
 *  Declaration of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"

class Label : public Drawable, public MessageHandler {
public:
    explicit Label(LPCSTR name);
    virtual ~Label();

    void LoadSettings(bool = false);
    void CreateLabelWindow();
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
    // The name of this label
    LPCSTR name;
};
