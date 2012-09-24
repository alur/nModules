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

enum {
    WM_FILE_ENUM = WM_USER+1,
    WM_FILE_ADDED,
    WM_FILE_REMOVED,
    WM_FILE_RENAMED,
    WM_FILE_MODIFIED
};


/// <summary>
/// Constructor
/// </summary>
IconGroup::IconGroup() {
    char szPath[MAX_PATH];

    m_pPaintSettings = new PaintSettings("DesktopIcons");
    m_pPaintSettings->position.bottom = 1100;
    m_pPaintSettings->position.top = 50;
    m_pPaintSettings->position.left = 1970;
    m_pPaintSettings->position.right = 3790;
    m_pPaintSettings->setText(L"");
    m_pPaintSettings->backColor = m_pPaintSettings->ARGBToD2DColor(0x6600FF00);

    m_pPaintSettings->GetSettings()->GetString("Folder", szPath, sizeof(szPath), "C:\\");

    m_pWindow = new DrawableWindow(FindWindow("DesktopBackgroundClass", ""), g_szGroupHandler, m_pPaintSettings, g_hInstance);
    SetWindowLongPtr(m_pWindow->getWindow(), 0, (LONG_PTR)this);
    m_pWindow->Show();

    DirectoryManager::MessageMap a = {
        WM_FILE_ENUM,
        WM_FILE_ADDED,
        WM_FILE_REMOVED,
        WM_FILE_RENAMED,
        WM_FILE_MODIFIED
    };

    m_pDirectoryManager = new DirectoryManager(L"c:\\test", m_pWindow->getWindow(), a);
}


/// <summary>
/// Destructor
/// </summary>
IconGroup::~IconGroup() {
    if (m_pWindow) delete m_pWindow;
    if (m_pPaintSettings) delete m_pPaintSettings;
    if (m_pDirectoryManager) delete m_pDirectoryManager;
}


/// <summary>
/// 
/// </summary>
LRESULT WINAPI IconGroup::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_FILE_ENUM:
        return 0;

    case WM_FILE_ADDED:
        return 0;

    case WM_FILE_REMOVED:
        return 0;

    case WM_FILE_RENAMED:
        return 0;

    case WM_FILE_MODIFIED:
        return 0;

    default:
        return m_pWindow->HandleMessage(uMsg, wParam, lParam);
    }
}
