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

class TrayIcon : public Drawable {
public:
    explicit TrayIcon(Drawable* parent, LiteStep::LPLSNOTIFYICONDATA pNID, Settings* parentSettings);
    virtual ~TrayIcon();
    
    void Reposition(RECT rect);
    void Reposition(UINT x, UINT y, UINT width, UINT height);
    void Show();

    void LoadSettings(bool isRefresh = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID drawableWindow);
    void SendCallback(UINT message, WPARAM wParam, LPARAM lParam);
    void GetScreenRect(LPRECT rect);

    void SetIcon(HICON icon);

    void HandleAdd(LiteStep::LPLSNOTIFYICONDATA pNID);
    void HandleModify(LiteStep::LPLSNOTIFYICONDATA pNID);
    void HandleSetVersion(LiteStep::LPLSNOTIFYICONDATA pNID);

private:
    //
    int iconSize;
    bool showingTip;

    bool showTip; // True if we should show the tooltip

    DrawableWindow::OVERLAY iconOverlay;

    // Tray data
    WCHAR tip[TRAY_MAX_TIP_LENGTH];
    UINT version;
    HICON icon;
    HWND callbackWindow;
    UINT callbackID;
    UINT callbackMessage;
    GUID guid;
    DWORD flags;
    DWORD mProcessID;
};
