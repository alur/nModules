/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayTip.hpp
 *  The nModules Project
 *
 *  Declaration of the TrayTip class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once
#include "../nShared/DrawableWindow.hpp"

class TrayTip : public MessageHandler {
public:
    explicit TrayTip(LPCSTR pszPrefix);
    virtual ~TrayTip();

    void Reposition(UINT x, UINT y, UINT width, UINT height);
    void Show();
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    HWND GetHWND();

private:
    // The tip's window
    DrawableWindow* m_pWindow;

    // Settings for painting the tip
    Settings* settings;
};
