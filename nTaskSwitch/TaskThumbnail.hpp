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
    
    void UpdateIconPosition();

    HWND targetWindow;

private:
    void UpdateIcon();
    void SetIcon(HICON icon);
    static void CALLBACK UpdateIconCallback(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult);

    HTHUMBNAIL thumbnail;
    WPARAM requestedIcon;

    DrawableWindow::STATE stateHover, stateSelected, stateSelectedHover;
    DrawableWindow* iconOverlayWindow;
    RECT thumbnailMargins;
};
