/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.hpp
 *  The nModules Project
 *
 *  Declaration of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/DrawableWindow.hpp"

class Label : public IDrawableMessageHandler {
public:
    explicit Label(LPCSTR);
    virtual ~Label();

    void LoadSettings(bool = false);
    void CreateLabelWindow();
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
    // The name of this label
    LPCSTR m_pszName;

    // The label's window
    DrawableWindow* m_pWindow;

    // PaintSettings
    Settings* settings;

    // The parent window
    HWND m_hWndParent;
};
