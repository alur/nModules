/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.hpp                                                  July, 2012
 *  The nModules Project
 *
 *  Declaration of the TaskButton class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef TASKBUTTON_HPP
#define TASKBUTTON_HPP

#include "../nShared/DrawableWindow.hpp"

class TaskButton {
public:
    explicit TaskButton(HWND parent, HWND window, LPCSTR prefix);
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
    PaintSettings* m_pPaintSettings;

    // Settings for painting the icon in the button.
    PaintSettings* m_pIconPaintSettings;

    // The window related to this button
    HWND m_hWnd;

    HWND m_hWndParent;

    TRACKMOUSEEVENT m_TrackMouseStruct;

    LPCSTR m_pszPrefix;
    LPCSTR m_pszPrefixActive;
    LPCSTR m_pszPrefixHover;
    LPCSTR m_pszPrefixFlashing;
    LPCSTR m_pszPrefixIcon;

    bool m_bMouseIsOver;
    bool m_bIsActive;
    bool m_bIsFlashing;

    HICON m_hIcon;

    HMENU hMenu;
};

#endif /* TASKBUTTON_HPP */
