/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.cpp                                                     July, 2012
 *  The nModules Project
 *
 *  Implementation of the Taskbar class. Handles layout of the taskbar buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"
#include "Taskbar.hpp"

extern HINSTANCE g_hInstance;
extern LPCSTR g_szTaskbarHandler;


/// <summary>
/// Constructor
/// </summary>
Taskbar::Taskbar(LPCSTR pszName) {
    m_pszName = pszName;

    this->settings = new Settings(m_pszName);
    LoadSettings();
    
    m_pWindow = new DrawableWindow(NULL, g_szTaskbarHandler, g_hInstance, this->settings, new DrawableSettings());
    SetWindowLongPtr(m_pWindow->GetWindow(), 0, (LONG_PTR)this);
    m_pWindow->Show();
}


/// <summary>
/// Destructor
/// </summary>
Taskbar::~Taskbar() {
    // Remove all buttons
    for (map<HWND, TaskButton*>::const_iterator iter = m_buttons.begin(); iter != m_buttons.end(); iter++) {
        delete iter->second;
    }
    m_buttons.clear();

    if (m_pWindow) delete m_pWindow;
    if (this->settings) delete this->settings;
    free((void *)m_pszName);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Taskbar::LoadSettings(bool /* isRefresh */) {
    m_uMaxButtonWidth = this->settings->GetInt("MaxButtonWidth", 500);
    m_uMonitor = this->settings->GetMonitor("Monitor", (UINT)-1);
}


/// <summary>
/// Adds the specified task to this taskbar
/// </summary>
TaskButton* Taskbar::AddTask(HWND hWnd, UINT monitor, bool noLayout) {
    if (monitor == m_uMonitor || m_uMonitor == -1) {
        TaskButton* pButton = new TaskButton(m_pWindow->GetWindow(), hWnd, m_pszName, this->settings);
        m_buttons.insert(m_buttons.end(), std::pair<HWND, TaskButton*>(hWnd, pButton));

        if (hWnd == GetForegroundWindow()) {
            pButton->Activate();
        }

        if (!noLayout)
            Relayout();

        return pButton;
    }
    return NULL;
}


/// <summary>
/// Removes the specified task from this taskbar, if it is on it
/// </summary>
void Taskbar::RemoveTask(HWND hWnd) {
    RemoveTask(m_buttons.find(hWnd));
}


/// <summary>
/// Removes the specified task from this taskbar, if it is on it
/// </summary>
void Taskbar::RemoveTask(map<HWND, TaskButton*>::iterator iter) {
    if (iter != m_buttons.end()) {
        delete iter->second;
        m_buttons.erase(iter);
        Relayout();
    }
}


/// <summary>
/// Called when the specified taskbar has moved to a different monitor.
/// </summary>
/// <param name="pOut">If a taskbutton was added or removed, the pointer to that button. Otherwise NULL.</param>
/// <returns>True if the task should be contained on this taskbar. False otherwise.</returns>
bool Taskbar::MonitorChanged(HWND hWnd, UINT monitor, TaskButton** pOut) {
    map<HWND, TaskButton*>::iterator iter = m_buttons.find(hWnd);
    *pOut = NULL;
    // If we should contain the task
    if (monitor == m_uMonitor || m_uMonitor == -1) {
        if (iter == m_buttons.end()) {
            *pOut = AddTask(hWnd, monitor, false);
        }
        return true;
    }
    else {
        if (iter != m_buttons.end()) {
            *pOut = iter->second;
            RemoveTask(iter);
        }
        return false;
    }
}


/// <summary>
/// Repositions/Resizes all buttons.  
/// </summary>
void Taskbar::Relayout() {
    UINT taskbarWidth = this->m_pWindow->GetSettings()->width;
    UINT buttonWidth = m_buttons.empty() ? m_uMaxButtonWidth : min(m_uMaxButtonWidth, (taskbarWidth-5)/(UINT)m_buttons.size());
    UINT startPos = 5;
    for (map<HWND, TaskButton*>::const_iterator iter = m_buttons.begin(); iter != m_buttons.end(); iter++) {
        iter->second->Reposition(startPos, 5, buttonWidth-5, this->m_pWindow->GetSettings()->height - 10);
        startPos += buttonWidth;
        iter->second->Show();
    }
}


/// <summary>
/// Handles window events for the taskbar.
/// </summary>
LRESULT WINAPI Taskbar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return m_pWindow->HandleMessage(uMsg, wParam, lParam);
}
