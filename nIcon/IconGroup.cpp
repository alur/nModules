/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  IconGroup.cpp                                                 August, 2012
 *  The nModules Project
 *
 *  
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "IconGroup.hpp"

// 
extern HINSTANCE g_hInstance;
extern LPCSTR g_szGroupHandler;


/// <summary>
/// Constructor
/// </summary>
IconGroup::IconGroup() {
    m_pPaintSettings = new PaintSettings("DesktopIcons");
    m_pPaintSettings->position.bottom = 1100;
    m_pPaintSettings->position.top = 50;
    m_pPaintSettings->position.left = 1970;
    m_pPaintSettings->position.right = 3790;
    m_pPaintSettings->setText(L"");
    m_pPaintSettings->backColor = m_pPaintSettings->ARGBToD2DColor(0x6600FF00);
    m_pWindow = new DrawableWindow(FindWindow("DesktopBackgroundClass", ""), g_szGroupHandler, m_pPaintSettings, g_hInstance);
    SetWindowLongPtr(m_pWindow->getWindow(), 0, (LONG_PTR)this);
    m_pWindow->Show();
}


/// <summary>
/// Destructor
/// </summary>
IconGroup::~IconGroup() {
    if (m_pWindow) delete m_pWindow;
    if (m_pPaintSettings) delete m_pPaintSettings;
}


/// <summary>
/// 
/// </summary>
LRESULT WINAPI IconGroup::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	return m_pWindow->HandleMessage(uMsg, wParam, lParam);
}
