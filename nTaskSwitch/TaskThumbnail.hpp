/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskThumbnail.hpp
 *  The nModules Project
 *
 *  A single task in the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include <dwmapi.h>
#include "../nShared/Drawable.hpp"
#include "../nShared/DrawableWindow.hpp"

class TaskThumbnail : public Drawable {
public:
    explicit TaskThumbnail(Drawable* parent, HWND targetWindow, int x, int y, int width, int height);
    virtual ~TaskThumbnail();

    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

    void Activate();

    void Select();
    void Deselect();

    HWND targetWindow;

private:
    HTHUMBNAIL thumbnail;

    DrawableWindow::STATE stateHover, stateSelected;
    RECT thumbnailMargins;
};
