/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Label.cpp                                                       July, 2012
 *  The nModules Project
 *
 *  Implementation of the Label class.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <strsafe.h>
#include "../headers/lsapi.h"
#include "../nCoreCom/Core.h"
#include "Label.hpp"

extern LPCSTR g_szLabelHandler;
extern HINSTANCE g_hInstance;


Label::Label(LPCSTR pszName) {
    m_pszName = pszName;
    m_pPaintSettings = new PaintSettings(m_pszName);
    m_pWindow = new DrawableWindow(NULL, g_szLabelHandler, m_pPaintSettings, g_hInstance);
    SetWindowLongPtr(m_pWindow->getWindow(), 0, (LONG_PTR)this);

    // Try to put the window right ontop of the desktop
    SetWindowPos(m_pWindow->getWindow(), FindWindow("DesktopBackgroundClass", ""), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );
    SetWindowPos(FindWindow("DesktopBackgroundClass", ""), m_pWindow->getWindow(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE ); 

    m_pWindow->Show();
}


Label::~Label() {
    if (m_pWindow) { delete m_pWindow; m_pWindow = NULL; }
    if (m_pPaintSettings) { delete m_pPaintSettings; m_pPaintSettings = NULL; }
    free((void *)m_pszName);
}


void Label::LoadSettings(bool /* bIsRefresh */) {
}


LRESULT WINAPI Label::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    return m_pWindow->HandleMessage(uMsg, wParam, lParam);
}
