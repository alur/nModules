/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowThumbnail.hpp
 *  The nModules Project
 *
 *  Draws a thumbnail of some window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "Drawable.hpp"
#include "StateRender.hpp"
#include "Window.hpp"
#include <dwmapi.h>

class WindowThumbnail : public Drawable
{
public:
    enum Position
    {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

    explicit WindowThumbnail(LPCTSTR prefix, Settings* parentSettings);
    virtual ~WindowThumbnail();

    void Show(HWND hwnd, LPRECT position);
    void Hide();

    void LoadSettings(bool = false);
    LRESULT WINAPI HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID Window);

private:
    enum class States
    {
        Base = 0,
        Count
    };

private:
    HWND hwnd;
    HTHUMBNAIL thumbnailHandle;
    Position position;
    RECT offset;
    int distanceFromButton;
    bool sizeToButton;
    int maxWidth;
    int maxHeight;
    int thumbnailOpacity;
    bool mAnimate;
    int mAnimationDuration;

    StateRender<States> mStateRender;
};
