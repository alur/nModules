/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskButton.hpp
 *  The nModules Project
 *
 *  Declaration of the TaskButton class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/Window.hpp"
#include "../nShared/Drawable.hpp"
#include "../Utilities/EnumArray.hpp"

class TaskButton : public Drawable
{
private:
    // All button states
    enum class State
    {
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

public:
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

public:
    void Reposition(UINT x, UINT y, UINT width, UINT height);

    void Activate();
    void Deactivate();

    void SetIcon(HICON icon);
    void SetOverlayIcon(HICON overlayIcon);
    void SetText(LPCWSTR title);
    void SetProgressState(TBPFLAG state);
    void SetProgressValue(USHORT progress);
    void Flash();

    void GetMinRect(LPPOINTS points);
    void ShowMenu();
    void Show();

    void LoadSettings(bool = false);
    
    void SetMinmizedState(bool value);

private:
    void SetFlashingState(bool value);
    void SetActiveState(bool value);
    void SetHoverState(bool value);

private:
    // Settings for painting the icon in the button.
    Settings *mIconSettings;

    // The window related to this button.
    HWND mWatchedWindow;

    // The system context menu for this window.
    HMENU mMenu;

    bool mMouseIsOver;
    bool mIsFlashing;
    bool mUseFlashing;
    bool mFlashOn;
    int mFlashInterval;
    UINT_PTR mFlashTimer;

    //
    bool mNoIcons;
    D2D1_RECT_F mIconRect;
    D2D1_RECT_F mOverlayIconRect;

    EnumArray<Window::STATE, State> mStates;

    Window::OVERLAY mIcon;
    Window::OVERLAY mIconOverlay;
};
