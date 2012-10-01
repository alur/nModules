/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "../nCoreCom/Core.h"
#include "Label.hpp"
#include "../nShared/Macros.h"

extern LPCSTR g_szLabelHandler;
extern HINSTANCE g_hInstance;


Label::Label(LPCSTR pszName) {
    m_pszName = pszName;
    this->settings = new Settings(m_pszName);
    m_pWindow = new DrawableWindow(NULL, g_szLabelHandler, g_hInstance, this->settings, new DrawableSettings(), this);
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
