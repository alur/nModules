/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp                                                       July, 2012
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
    m_pWindow = new DrawableWindow(NULL, g_szLabelHandler, g_hInstance, this->settings, new DrawableSettings());
    SetWindowLongPtr(m_pWindow->GetWindow(), 0, (LONG_PTR)this);

    // Try to put the window right ontop of the desktop
    SetWindowPos(m_pWindow->GetWindow(), FindWindow("DesktopBackgroundClass", ""), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
    SetWindowPos(FindWindow("DesktopBackgroundClass", ""), m_pWindow->GetWindow(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE ); 

    m_pWindow->Show();
}


Label::~Label() {
    SAFEDELETE(this->m_pWindow);
    SAFEDELETE(this->settings);
    free((void *)m_pszName);
}


void Label::LoadSettings(bool /* bIsRefresh */) {
}


LRESULT WINAPI Label::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return m_pWindow->HandleMessage(uMsg, wParam, lParam);
}
