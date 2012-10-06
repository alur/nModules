/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayIcon.hpp
 *  The nModules Project
 *
 *  Declaration of the TrayIcon class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"

class TrayIcon : public Drawable, public MessageHandler {
public:
    explicit TrayIcon(Drawable* parent, LPLSNOTIFYICONDATA pNID, Settings* parentSettings);
    virtual ~TrayIcon();

    void Reposition(UINT x, UINT y, UINT width, UINT height);
    void Update();
    void Show();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    void SendCallback(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void UpdateIcon();
    void GetScreenRect(LPRECT rect);

private:
    //
    LPLSNOTIFYICONDATA m_pNotifyData;

    int iconSize;
};
