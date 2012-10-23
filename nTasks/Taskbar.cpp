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

extern LSModule* g_LSModule;

/// <summary>
/// Constructor
/// </summary>
Taskbar::Taskbar(LPCSTR name) : Drawable(name) {
    this->name = name;

    this->thumbnail = new WindowThumbnail("Thumbnail", this->settings);

    LoadSettings();

    this->layoutSettings = new LayoutSettings();
    LayoutSettings* defaults = new LayoutSettings();
    this->layoutSettings->Load(this->settings, defaults);
    delete defaults;
    
    this->window->Initialize(new DrawableSettings());
    this->window->Show();
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

    SAFEDELETE(this->layoutSettings);
    SAFEDELETE(this->thumbnail);
    free((void *)name);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Taskbar::LoadSettings(bool /* isRefresh */) {
    Settings* buttonSettings = settings->CreateChild("Button");

    this->buttonWidth = buttonSettings->GetInt("Width", 100);
    this->buttonHeight = buttonSettings->GetInt("Height", 20);
    this->buttonMaxWidth = buttonSettings->GetInt("MaxWidth", this->buttonWidth);
    this->buttonMaxHeight = buttonSettings->GetInt("MaxHeight", this->buttonHeight);
    this->monitor = this->settings->GetMonitor("Monitor", 0xFFFFFFFF);
    this->noThumbnails = this->settings->GetBool("NoThumbnails", false);
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
    this->window->Repaint();
}


/// <summary>
/// Repositions/Resizes all buttons.  
/// </summary>
void Taskbar::Relayout() {
    DrawableSettings* drawingSettings = this->window->GetDrawingSettings();
    int spacePerLine, lines, buttonSize, x0, y0, xdir, ydir;

    if (this->buttons.size() == 0) return;

    switch (this->layoutSettings->startPosition) {
    default:
    case LayoutSettings::TOPLEFT:
        {
            x0 = this->layoutSettings->padding.left;
            y0 = this->layoutSettings->padding.top;
            xdir = 1;
            ydir = 1;
        }
        break;

    case LayoutSettings::TOPRIGHT:
        {
            x0 = drawingSettings->width - this->layoutSettings->padding.right;
            y0 = this->layoutSettings->padding.top;
            xdir = -1;
            ydir = 1;
        }
        break;

    case LayoutSettings::BOTTOMLEFT:
        {
            x0 = this->layoutSettings->padding.left;
            y0 = drawingSettings->height - this->layoutSettings->padding.bottom;
            xdir = 1;
            ydir = -1;
        }
        break;

    case LayoutSettings::BOTTOMRIGHT:
        {
            x0 = drawingSettings->width - this->layoutSettings->padding.right;
            y0 = drawingSettings->height - this->layoutSettings->padding.bottom;
            xdir = -1;
            ydir = -1;
        }
        break;
    }

    if (this->layoutSettings->primaryDirection == LayoutSettings::HORIZONTAL) {
        spacePerLine = drawingSettings->width - this->layoutSettings->padding.left - this->layoutSettings->padding.right;
        lines = (drawingSettings->height + this->layoutSettings->rowSpacing - this->layoutSettings->padding.top - this->layoutSettings->padding.bottom)/(this->layoutSettings->rowSpacing + this->buttonHeight);
        // We need to consider that buttons can't be split between multiple lines.
        buttonSize = (int)min(this->buttonMaxWidth, min(spacePerLine * lines / (int)this->buttons.size(), spacePerLine / (int)ceil(this->buttons.size() / (float)lines)) - this->layoutSettings->columnSpacing);
        if (ydir == -1) y0 -= this->buttonHeight;
        if (xdir == -1) x0 -= buttonSize;
        int x = x0, y = y0;
        for (map<HWND, TaskButton*>::const_iterator iter = this->buttons.begin(); iter != this->buttons.end(); iter++) {
            iter->second->Reposition(x, y, buttonSize, this->buttonHeight);
            x += xdir*(buttonSize + this->layoutSettings->columnSpacing);
            if (x < this->layoutSettings->padding.left || x > drawingSettings->width - this->layoutSettings->padding.right - buttonSize) {
                x = x0;
                y += ydir*(this->buttonHeight + this->layoutSettings->rowSpacing);
            }
            iter->second->Show();
        }
    }
    else {
        spacePerLine = drawingSettings->height - this->layoutSettings->padding.top - this->layoutSettings->padding.bottom;
        lines = (drawingSettings->width + this->layoutSettings->columnSpacing - this->layoutSettings->padding.left - this->layoutSettings->padding.right)/(this->layoutSettings->columnSpacing + this->buttonWidth);
        buttonSize = (int)min(this->buttonMaxHeight, min(spacePerLine * lines / (int)this->buttons.size(), spacePerLine / (int)ceil(this->buttons.size() / (float)lines)) - this->layoutSettings->rowSpacing);
        if (ydir == -1) y0 -= buttonSize;
        if (xdir == -1) x0 -= this->buttonWidth;
        int x = x0, y = y0;
        for (map<HWND, TaskButton*>::const_iterator iter = this->buttons.begin(); iter != this->buttons.end(); iter++) {
            iter->second->Reposition(x, y, this->buttonWidth, buttonSize);
            y += ydir*(buttonSize + this->layoutSettings->rowSpacing);
            if (y < this->layoutSettings->padding.top || y > drawingSettings->height - this->layoutSettings->padding.bottom - buttonSize) {
                y = y0;
                x += xdir*(this->buttonWidth + this->layoutSettings->columnSpacing);
            }
            iter->second->Show();
        }
    }

    this->Repaint();
}


/// <summary>
/// Handles window messages for this taskbar.
/// </summary>
LRESULT WINAPI Taskbar::HandleMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
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
