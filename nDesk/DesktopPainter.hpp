/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*   DesktopPainter.hpp
*   The nModules Project
*
*   Function declarations for the DesktopPainter class.
*   
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <d2d1.h>
#include "TransitionEffects.h"
#include "../nShared/Window.hpp"

class DesktopPainter : protected Window {
public:
    // Available transition types
    enum TransitionType {
        NONE,

        FADE_IN,
        FADE_OUT,

        SLIDE_BOTH_LEFT,
        SLIDE_BOTH_RIGHT,
        SLIDE_BOTH_UP,
        SLIDE_BOTH_DOWN,
        SLIDE_IN_LEFT,
        SLIDE_IN_RIGHT,
        SLIDE_IN_UP,
        SLIDE_IN_DOWN,
        SLIDE_OUT_LEFT,
        SLIDE_OUT_RIGHT,
        SLIDE_OUT_UP,
        SLIDE_OUT_DOWN,

        SCAN_LEFT,
        SCAN_RIGHT,
        SCAN_UP,
        SCAN_DOWN,

        SQUARES_RANDOM_IN,
        SQUARES_RANDOM_OUT,
        SQUARES_LINEAR_VERTICAL_IN,
        SQUARES_LINEAR_VERTICAL_OUT,
        SQUARES_LINEAR_HORIZONTAL_IN,
        SQUARES_LINEAR_HORIZONTAL_OUT,
        SQUARES_TRIANGULAR_BOTTOM_RIGHT_IN,
        SQUARES_TRIANGULAR_BOTTOM_RIGHT_OUT,
        SQUARES_CLOCKWISE_IN,
        SQUARES_CLOCKWISE_OUT,
        SQUARES_COUNTERCLOCKWISE_IN,
        SQUARES_COUNTERCLOCKWISE_OUT
    };

    explicit DesktopPainter(HWND hWnd);
    virtual ~DesktopPainter();

    void SetTransitionType(TransitionType);
    void SetTransitionTime(int);
    void SetSquareSize(int);

    void SetInvalidateAllOnUpdate(bool);

    void UpdateWallpaper(bool bNoTransition = false);
    void Resize();
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM);
    HWND GetWindow();

private:
    void CalculateSizeDepdenentStuff();
    HRESULT ReCreateDeviceResources();
    void DiscardDeviceResources();

    void Paint();
    void PaintComposite();
    void Redraw();

    void TransitionStart();
    void TransitionEnd();
    TransitionEffect* TransitionEffectFromType(TransitionType transitionType);

    HRESULT CreateWallpaperBrush(ID2D1BitmapBrush** ppBitmapBrush);

    //
    HWND m_hWnd;

    //
    DWORD transitionEndTime;
    DWORD transitionStartTime;

    // Direct2D targets
    ID2D1BitmapBrush* m_pWallpaperBrush;
    ID2D1BitmapBrush* m_pOldWallpaperBrush;

    //
    bool mDontRenderWallpaper;

    // The type of transition we should use
    TransitionType m_TransitionType;

    // The transition effect currently in use
    TransitionEffect* m_TransitionEffect;

    // Holds all settings for transitions. Passed in to the transitions on init.
    TransitionEffect::TransitionSettings m_TransitionSettings;

    // If on, every window will be repainted when the wallpaper is changed instead of just the
    // desktop background. Fixes issues with other modules (xModules).
    bool m_bInvalidateAllOnUpdate;
};
