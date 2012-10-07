/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.hpp
 *  The nModules Project
 *
 *  Declaration of the TaskButton class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/DrawableWindow.hpp"
#include "../nShared/Drawable.hpp"

class TaskButton : public Drawable {
public:
    explicit TaskButton(Drawable* parent, HWND hWnd);
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
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);

private:
    // Settings for painting the icon in the button.
    Settings* iconSettings;

    // The window related to this button
    HWND m_hWnd;

    HWND m_hWndParent;

    bool m_bMouseIsOver;
    bool m_bIsActive;
    bool m_bIsFlashing;
    bool useFlashing;

    bool flashOn;
    int flashInterval;
    UINT_PTR flash_timer;

    DrawableWindow::STATE stateHover, stateActive, stateFlashing;

    HICON m_hIcon;

    HMENU hMenu;
};
