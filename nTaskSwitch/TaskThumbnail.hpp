/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TaskThumbnail.hpp
 *  The nModules Project
 *
 *  A single task in the alt-tab window.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../Utilities/Common.h"
#include <dwmapi.h>
#include "../nShared/Drawable.hpp"
#include "../nShared/StateRender.hpp"
#include "ThumbnailSettings.hpp"
#include "../nShared/Window.hpp"

class TaskThumbnail : public Drawable
{
public:
    enum class State
    {
        Base = 0,
        Hover,
        Selected,
        SelectedHover,
        Count
    };

    enum class IconState
    {
        Base = 0,
        Count
    };

public:
    explicit TaskThumbnail(Drawable* parent, HWND targetWindow, float x, float y, float width, float height, ThumbnailSettings& thumbnailSettings);
    virtual ~TaskThumbnail();

public:
    TaskThumbnail & operator=(const TaskThumbnail &) /* = deleted */;

public:
    LRESULT WINAPI HandleMessage(HWND, UINT, WPARAM, LPARAM, LPVOID);

public:
    void Activate();

    void Select();
    void Deselect();
    
    void UpdateIconPosition();

    HWND mTargetWindow;

public:
    void ActivateState(State state);
    void ClearState(State state);

private:
    void UpdateIcon();
    void SetIcon(HICON icon);
    static void CALLBACK UpdateIconCallback(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult);

private:
    ThumbnailSettings& mThumbnailSettings;

    // Since the icons are independent top-level windows, they can not share this.
    StateRender<TaskThumbnail::IconState> mIconStateRender;

    HTHUMBNAIL mThumbnail;
    WPARAM mRequestedIcon;

    Window* mIconOverlayWindow;
};
