/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.cpp
 *  The nModules Project
 *
 *  Implementation of the Taskbar class. Handles layout of the taskbar buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"
#include "../nShared/LSModule.hpp"
#include "Taskbar.hpp"
#include "../Utilities/Math.h"
#include "../nShared/DWMColorVal.hpp"


/// <summary>
/// Constructor
/// </summary>
Taskbar::Taskbar(LPCTSTR name) : Drawable(name) {
    this->thumbnail = new WindowThumbnail(_T("Thumbnail"), mSettings);

    LoadSettings();

    LayoutSettings defaults;
    mLayoutSettings.Load(mSettings, &defaults);

    DrawableSettings drawableDefaults;
    drawableDefaults.width = GetSystemMetrics(SM_CXSCREEN);
    drawableDefaults.height = 36;

    StateSettings stateDefaults;
    stateDefaults.backgroundBrush.color = std::unique_ptr<IColorVal>(new DWMColorVal());
    
    mWindow->Initialize(&drawableDefaults, &stateDefaults);
    mWindow->Show();
}


/// <summary>
/// Destructor
/// </summary>
Taskbar::~Taskbar() {
    // Remove all buttons
    for (map<HWND, TaskButton*>::const_iterator iter = this->buttons.begin(); iter != this->buttons.end(); iter++) {
        delete iter->second;
    }
    this->buttons.clear();

    SAFEDELETE(this->thumbnail);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Taskbar::LoadSettings(bool /* isRefresh */) {
    Settings* buttonSettings = mSettings->CreateChild(_T("Button"));

    this->buttonWidth = buttonSettings->GetInt(_T("Width"), 150);
    this->buttonHeight = buttonSettings->GetInt(_T("Height"), 36);
    this->buttonMaxWidth = buttonSettings->GetInt(_T("MaxWidth"), this->buttonWidth);
    this->buttonMaxHeight = buttonSettings->GetInt(_T("MaxHeight"), this->buttonHeight);
    this->monitor = mSettings->GetMonitor(_T("Monitor"), 0xFFFFFFFF);
    this->noThumbnails = mSettings->GetBool(_T("NoThumbnails"), false);
}


/// <summary>
/// Adds the specified task to this taskbar
/// </summary>
TaskButton* Taskbar::AddTask(HWND hWnd, UINT monitor, bool noLayout) {
    if (monitor == this->monitor || this->monitor == 0xFFFFFFFF) {
        TaskButton* pButton = new TaskButton(this, hWnd);
        this->buttons.insert(this->buttons.end(), std::pair<HWND, TaskButton*>(hWnd, pButton));

        if (hWnd == GetForegroundWindow()) {
            pButton->Activate();
        }

        if (!noLayout) {
            Relayout();
            Repaint();
        }

        return pButton;
    }
    return NULL;
}


/// <summary>
/// Removes the specified task from this taskbar, if it is on it
/// </summary>
void Taskbar::RemoveTask(HWND hWnd) {
    RemoveTask(this->buttons.find(hWnd));
}


/// <summary>
/// Removes the specified task from this taskbar, if it is on it
/// </summary>
void Taskbar::RemoveTask(map<HWND, TaskButton*>::iterator iter) {
    if (iter != this->buttons.end()) {
        delete iter->second;
        this->buttons.erase(iter);
        Relayout();
        Repaint();
    }
}


/// <summary>
/// Called when the specified taskbar has moved to a different monitor.
/// </summary>
/// <param name="pOut">If a taskbutton was added or removed, the pointer to that button. Otherwise NULL.</param>
/// <returns>True if the task should be contained on this taskbar. False otherwise.</returns>
bool Taskbar::MonitorChanged(HWND hWnd, UINT monitor, TaskButton** pOut) {
    map<HWND, TaskButton*>::iterator iter = this->buttons.find(hWnd);
    *pOut = NULL;
    // If we should contain the task
    if (monitor == this->monitor || this->monitor == 0xFFFFFFFF) {
        if (iter == this->buttons.end()) {
            *pOut = AddTask(hWnd, monitor, false);
        }
        return true;
    }
    else {
        if (iter != this->buttons.end()) {
            *pOut = iter->second;
            RemoveTask(iter);
        }
        return false;
    }
}


void Taskbar::Repaint() {
    mWindow->Repaint();
}


/// <summary>
/// Repositions/Resizes all buttons.  
/// </summary>
void Taskbar::Relayout() {
    DrawableSettings* drawingSettings = mWindow->GetDrawingSettings();
    int spacePerLine, lines, buttonSize, x0, y0, xdir, ydir;

    if (this->buttons.size() == 0) return;

    switch (mLayoutSettings.mStartPosition) {
    default:
    case LayoutSettings::StartPosition::TopLeft:
        {
            x0 = mLayoutSettings.mPadding.left;
            y0 = mLayoutSettings.mPadding.top;
            xdir = 1;
            ydir = 1;
        }
        break;

    case LayoutSettings::StartPosition::TopRight:
        {
            x0 = drawingSettings->width - mLayoutSettings.mPadding.right;
            y0 = mLayoutSettings.mPadding.top;
            xdir = -1;
            ydir = 1;
        }
        break;

    case LayoutSettings::StartPosition::BottomLeft:
        {
            x0 = mLayoutSettings.mPadding.left;
            y0 = drawingSettings->height - mLayoutSettings.mPadding.bottom;
            xdir = 1;
            ydir = -1;
        }
        break;

    case LayoutSettings::StartPosition::BottomRight:
        {
            x0 = drawingSettings->width - mLayoutSettings.mPadding.right;
            y0 = drawingSettings->height - mLayoutSettings.mPadding.bottom;
            xdir = -1;
            ydir = -1;
        }
        break;
    }

    if (mLayoutSettings.mPrimaryDirection == LayoutSettings::Direction::Horizontal) {
        spacePerLine = drawingSettings->width - mLayoutSettings.mPadding.left - mLayoutSettings.mPadding.right;
        lines = (drawingSettings->height + mLayoutSettings.mRowSpacing - mLayoutSettings.mPadding.top - mLayoutSettings.mPadding.bottom)/(mLayoutSettings.mRowSpacing + this->buttonHeight);
        // We need to consider that buttons can't be split between multiple lines.
        buttonSize = (int)min(this->buttonMaxWidth, min(spacePerLine * lines / (int)this->buttons.size(), spacePerLine / (int)ceil(this->buttons.size() / (float)lines)) - mLayoutSettings.mColumnSpacing);
        if (ydir == -1) y0 -= this->buttonHeight;
        if (xdir == -1) x0 -= buttonSize;
        int x = x0, y = y0;
        for (map<HWND, TaskButton*>::const_iterator iter = this->buttons.begin(); iter != this->buttons.end(); iter++) {
            iter->second->Reposition(x, y, buttonSize, this->buttonHeight);
            x += xdir*(buttonSize + mLayoutSettings.mColumnSpacing);
            if (x < mLayoutSettings.mPadding.left || x > drawingSettings->width - mLayoutSettings.mPadding.right - buttonSize) {
                x = x0;
                y += ydir*(this->buttonHeight + mLayoutSettings.mRowSpacing);
            }
            iter->second->Show();
        }
    }
    else {
        spacePerLine = drawingSettings->height - mLayoutSettings.mPadding.top - mLayoutSettings.mPadding.bottom;
        lines = (drawingSettings->width + mLayoutSettings.mColumnSpacing - mLayoutSettings.mPadding.left - mLayoutSettings.mPadding.right)/(mLayoutSettings.mColumnSpacing + this->buttonWidth);
        buttonSize = (int)min(this->buttonMaxHeight, min(spacePerLine * lines / (int)this->buttons.size(), spacePerLine / (int)ceil(this->buttons.size() / (float)lines)) - mLayoutSettings.mRowSpacing);
        if (ydir == -1) y0 -= buttonSize;
        if (xdir == -1) x0 -= this->buttonWidth;
        int x = x0, y = y0;
        for (map<HWND, TaskButton*>::const_iterator iter = this->buttons.begin(); iter != this->buttons.end(); iter++) {
            iter->second->Reposition(x, y, this->buttonWidth, buttonSize);
            y += ydir*(buttonSize + mLayoutSettings.mRowSpacing);
            if (y < mLayoutSettings.mPadding.top || y > drawingSettings->height - mLayoutSettings.mPadding.bottom - buttonSize) {
                y = y0;
                x += xdir*(this->buttonWidth + mLayoutSettings.mColumnSpacing);
            }
            iter->second->Show();
        }
    }

    this->Repaint();
}


/// <summary>
/// Handles window messages for this taskbar.
/// </summary>
LRESULT WINAPI Taskbar::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam, LPVOID) {
    mEventHandler->HandleMessage(window, message, wParam, lParam);
    return DefWindowProc(window, message, wParam, lParam);
}


/// <summary>
/// 
/// </summary>
void Taskbar::ShowThumbnail(HWND hwnd, LPRECT position) {
    if (!this->noThumbnails) {
        this->thumbnail->Show(hwnd, position);
    }
}


/// <summary>
/// 
/// </summary>
void Taskbar::HideThumbnail() {
    this->thumbnail->Hide();
}
