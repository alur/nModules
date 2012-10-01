/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.hpp
 *  The nModules Project
 *
 *  Declaration of the TaskButton class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/DrawableWindow.hpp"

class TaskButton {
public:
    explicit TaskButton(HWND parent, HWND window, LPCSTR prefix, Settings* parentSettings);
    virtual ~TaskButton();

    void Reposition(UINT x, UINT y, UINT width, UINT height);

    void Activate();
    void Deactivate();

    void SetIcon(HICON hIcon);
    void SetText(LPCWSTR pszTitle);
    void Flash();

    void GetMinRect(LPPOINTS lpPoints);
    void Menu();
    void Show();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(UINT, WPARAM, LPARAM);

private:
    // The button's window
    DrawableWindow* m_pWindow;

    // Settings for painting the button
    Settings* settings;

    // Settings for painting the icon in the button.
    Settings* iconSettings;

    // The window related to this button
    HWND m_hWnd;

    HWND m_hWndParent;

    TRACKMOUSEEVENT m_TrackMouseStruct;

    bool m_bMouseIsOver;
    bool m_bIsActive;
    bool m_bIsFlashing;

    DrawableWindow::STATE stateHover, stateActive, stateFlashing;

    HICON m_hIcon;

    HMENU hMenu;
};
