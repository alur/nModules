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
#include "../Utilities/EnumArray.hpp"

class TaskButton : public Drawable {
private:
    enum class State {
        Active = 0,
        ActiveHover,
        Flashing,
        FlashingHover,
        Hover,
        Minimized,
        MinimizedHover,
        MinimizedFlashing,
        MinimizedFlashingHover,
        Count
    };

public:
    explicit TaskButton(Drawable* parent, HWND hWnd);
    virtual ~TaskButton();

    void Reposition(UINT x, UINT y, UINT width, UINT height);

    void Activate();
    void Deactivate();

    void SetIcon(HICON hIcon);
    void SetOverlayIcon(HICON hOverlayIcon);
    void SetText(LPCWSTR pszTitle);
    void Flash();

    void GetMinRect(LPPOINTS lpPoints);
    void ShowMenu();
    void Show();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);
    
    void SetMinmizedState(bool value);

private:
    void SetFlashingState(bool value);
    void SetActiveState(bool value);
    void SetHoverState(bool value);

private:
    // Settings for painting the icon in the button.
    Settings *iconSettings;

    // The window related to this button.
    HWND watchedWindow;

    // The system context menu for this window.
    HMENU menu;

    bool mouseIsOver;
    bool isFlashing;
    bool useFlashing;
    bool flashOn;
    int flashInterval;
    UINT_PTR flashTimer;

    //
    bool mNoIcons;
    D2D1_RECT_F mIconRect;

    EnumArray<DrawableWindow::STATE, State> mStates;

    DrawableWindow::OVERLAY icon, iconOverlay;
};
