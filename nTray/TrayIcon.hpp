/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayIcon.hpp                                                    July, 2012
 *  The nModules Project
 *
 *  Declaration of the TrayIcon class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef TRAYICON_HPP
#define TRAYICON_HPP

#include "../nShared/DrawableWindow.hpp"

class TrayIcon {
public:
    explicit TrayIcon(HWND parent, LPCSTR prefix, LPLSNOTIFYICONDATA pNID);
    virtual ~TrayIcon();

    void Reposition(UINT x, UINT y, UINT width, UINT height);
    void Update();
    void Show();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);
    void SendCallback(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void UpdateIcon();
    HWND GetHWND();

private:
    // The icon's window
    DrawableWindow* m_pWindow;

    // Settings for painting the icon
    PaintSettings* m_pPaintSettings;

    //
    LPLSNOTIFYICONDATA m_pNotifyData;
};

#endif /* TRAYICON_HPP */
