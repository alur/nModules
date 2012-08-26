/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayTip.hpp                                                   August, 2012
 *  The nModules Project
 *
 *  Declaration of the TrayTip class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef TRAYTIP_HPP
#define TRAYTIP_HPP

#include "../nShared/DrawableWindow.hpp"

class TrayTip {
public:
    explicit TrayTip(LPCSTR pszPrefix);
    virtual ~TrayTip();

    void Reposition(UINT x, UINT y, UINT width, UINT height);
    void Show();
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    HWND GetHWND();

private:
    // The tip's window
    DrawableWindow* m_pWindow;

    // Settings for painting the tip
    PaintSettings* m_pPaintSettings;
};

#endif /* TRAYTIP_HPP */
