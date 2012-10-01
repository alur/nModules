/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "../nShared/LSModule.hpp"
#include "Label.hpp"
#include "../nShared/Macros.h"

extern LSModule* g_LSModule;

Label::Label(LPCSTR pszName) {
    m_pszName = pszName;
    this->settings = new Settings(m_pszName);
    m_pWindow = new DrawableWindow(NULL, (LPCSTR)g_LSModule->GetWindowClass(1), g_LSModule->GetInstance(), this->settings, new DrawableSettings(), this);
    m_pWindow->Show();
}


Label::~Label() {
    SAFEDELETE(this->m_pWindow);
    SAFEDELETE(this->settings);
    free((LPVOID)m_pszName);
}


void Label::LoadSettings(bool /* bIsRefresh */) {
}


LRESULT WINAPI Label::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return m_pWindow->HandleMessage(hWnd, uMsg, wParam, lParam);
}
