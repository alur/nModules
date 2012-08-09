/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Taskbar.cpp                                                     July, 2012
 *  The nModules Project
 *
 *  Implementation of the Taskbar class. Handles layout of the taskbar buttons.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "Taskbar.hpp"

extern HINSTANCE g_hInstance;
extern LPCSTR g_szTaskbarHandler;


/// <summary>
/// Constructor
/// </summary>
Taskbar::Taskbar(LPCSTR pszName) {
    m_pszName = pszName;
    LoadSettings();

    m_pPaintSettings = new PaintSettings(m_pszName);
    m_pWindow = new DrawableWindow(NULL, g_szTaskbarHandler, m_pPaintSettings, g_hInstance);
    SetWindowLongPtr(m_pWindow->getWindow(), 0, (LONG_PTR)this);
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
    if (m_pPaintSettings) delete m_pPaintSettings;
    free((void *)m_pszName);
}


/// <summary>
/// Loads settings from LiteStep's RC files.
/// </summary>
void Taskbar::LoadSettings(bool /* bIsRefresh */) {
    using namespace nCore::InputParsing;
    m_uMaxButtonWidth = GetPrefixedRCInt(m_pszName, "MaxButtonWidth", 500);
    m_uMonitor = GetPrefixedRCMonitor(m_pszName, "Monitor", (UINT)-1);
}


/// <summary>
/// Adds the specified task to this taskbar
/// </summary>
TaskButton* Taskbar::AddTask(HWND hWnd, UINT monitor, bool noLayout) {
    if (monitor == m_uMonitor || m_uMonitor == -1) {
        TaskButton* pButton = new TaskButton(m_pWindow->getWindow(), hWnd, m_pszName);
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
    map<HWND, TaskButton*>::iterator iter = m_buttons.find(hWnd);
    if (iter != m_buttons.end()) {
        delete iter->second;
        m_buttons.erase(iter);
    }
    Relayout();
}


/// <summary>
/// Repositions/Resizes all buttons
/// </summary>
void Taskbar::Relayout() {
    UINT taskbarWidth = m_pPaintSettings->position.right - m_pPaintSettings->position.left;
    UINT buttonWidth = m_buttons.empty() ? m_uMaxButtonWidth : min(m_uMaxButtonWidth, (taskbarWidth-5)/(UINT)m_buttons.size());
    UINT startPos = 5;
    for (map<HWND, TaskButton*>::const_iterator iter = m_buttons.begin(); iter != m_buttons.end(); iter++) {
        iter->second->Reposition(startPos, 5, buttonWidth-5, 40);
        startPos += buttonWidth;
        iter->second->Show();
    }
}


/// <summary>
/// Handles window events for the taskbar
/// </summary>
LRESULT WINAPI Taskbar::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_LBUTTONDOWN:
        return 0;
    default:
        return m_pWindow->HandleMessage(uMsg, wParam, lParam);
    }
}
