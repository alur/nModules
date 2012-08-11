/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayManager.h                                                   July, 2012
 *  The nModules Project
 *
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include "Tray.hpp"
#include "TrayManager.h"
#include "../nShared/Debugging.h"

// The various trays
extern map<LPCSTR, Tray*> g_Trays;

namespace TrayManager {
	// All current tray icons
    vector<LPTRAYICONDATA> g_currentIcons;
}


/// <summary>
/// Stops the tray manager. 
/// </summary>
void TrayManager::Stop() {
    for (TRAYICONDATAITER iter = g_currentIcons.begin(); iter != g_currentIcons.end(); iter++) {
        delete (*iter)->icons;
        free((LPVOID)*iter);
    }
    g_currentIcons.clear();
}


/// <summary>
/// Finds a matching icon in g_currentIcons.
/// </summary>
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(GUID guid) {
    for (TRAYICONDATAITER iter = g_currentIcons.begin(); iter != g_currentIcons.end(); iter++) {
        if ((*iter)->NID.guidItem == guid) {
            return iter;
        }
    }
    return g_currentIcons.end();
}


/// <summary>
/// Finds a matching icon in g_currentIcons.
/// </summary>
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(HWND hWnd, UINT uID) {
    for (TRAYICONDATAITER iter = g_currentIcons.begin(); iter != g_currentIcons.end(); iter++) {
        if ((*iter)->NID.hWnd == hWnd && (*iter)->NID.uID == uID) {
            return iter;
        }
    }
    return g_currentIcons.end();
}


/// <summary>
/// Finds a matching icon in g_currentIcons.
/// </summary>
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(LPLSNOTIFYICONDATA pNID) {
    // There are 2 ways to identify an icon. Same guidItem, or same HWND and same uID.

    // uID & hWnd is ignored if guidItem is set
    if ((pNID->uFlags & NIF_GUID) == NIF_GUID) {
        return FindIcon(pNID->guidItem);
    }

    return FindIcon(pNID->hWnd, pNID->uID);
}


/// <summary>
/// Finds a matching icon in g_currentIcons.
/// </summary>
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(LPSYSTRAYINFOEVENT pSTE) {
    // There are 2 ways to identify an icon. Same guidItem, or same HWND and same uID.
    TRAYICONDATAITER ret;

    ret = FindIcon(pSTE->guidItem);
    if (ret == g_currentIcons.end()) {
        ret = FindIcon(pSTE->hWnd, pSTE->uID);
    }

    return ret;
}


/// <summary>
/// Adds the specified icon to the tray, if it exists.
/// </summary>
void TrayManager::AddIcon(LPLSNOTIFYICONDATA pNID) {
    if (FindIcon(pNID) == g_currentIcons.end()) {
        TRAYICONDATA* tData = (TRAYICONDATA*)malloc(sizeof(TRAYICONDATA));
        memcpy(&tData->NID, pNID, sizeof(tData->NID));
        tData->icons = new vector<TrayIcon*>;
        
        for (TRAYSCITER iter = g_Trays.begin(); iter != g_Trays.end(); iter++) {
            TrayIcon* pIcon = iter->second->AddIcon(&tData->NID);
            if (pIcon != NULL) {
                tData->icons->push_back(pIcon);
            }
        }

        g_currentIcons.push_back(tData);
    }
}


/// <summary>
/// Deletes the specified icon from all trays, if it exists.
/// </summary>
void TrayManager::DeleteIcon(LPLSNOTIFYICONDATA pNID) {
    TRAYICONDATAITER icon = FindIcon(pNID);
    if (icon != g_currentIcons.end()) {
        for (TRAYSCITER iter = g_Trays.begin(); iter != g_Trays.end(); iter++) {
            for (vector<TrayIcon*>::const_iterator iter2 = (*icon)->icons->begin(); iter2 != (*icon)->icons->end(); iter2++) {
                iter->second->RemoveIcon(*iter2);
            }
        }
        (*icon)->icons->clear();
        delete (*icon)->icons;
        free((LPVOID)*icon);
        g_currentIcons.erase(icon);
    }
}


/// <summary>
/// Modifies an existing icon.
/// </summary>
void TrayManager::ModifyIcon(LPLSNOTIFYICONDATA pNID) {
    TRAYICONDATAITER icon = FindIcon(pNID);
    if (icon != g_currentIcons.end()) {

        // Check if we should update the message ID
        if ((pNID->uFlags & NIF_MESSAGE) == NIF_MESSAGE) {
            (*icon)->NID.uCallbackMessage = pNID->uCallbackMessage;
        }

        // Check if we should update the icon
        if ((pNID->uFlags & NIF_ICON) == NIF_ICON) {
            if ((*icon)->NID.hIcon != pNID->hIcon) {
                (*icon)->NID.hIcon = pNID->hIcon;
                for (vector<TrayIcon*>::const_iterator iter = (*icon)->icons->begin(); iter != (*icon)->icons->end(); iter++) {
                    (*iter)->UpdateIcon();
                }
            }
        }
    }
}


/// <summary>
/// ?
/// </summary>
void TrayManager::SetFocus(LPLSNOTIFYICONDATA pNID) {

}


/// <summary>
/// Changes the version of an existing tray icon.
/// </summary>
void TrayManager::SetVersion(LPLSNOTIFYICONDATA pNID) {
    TRAYICONDATAITER icon = FindIcon(pNID);
    if (icon != g_currentIcons.end()) {
        (*icon)->NID.uVersion = pNID->uVersion;
    }
}


/// <summary>
/// Handles LiteStep shell messages.
/// <summary>
LRESULT TrayManager::ShellMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case LM_SYSTRAY:
        switch ((DWORD)wParam) {
        case NIM_ADD:
            AddIcon((LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_DELETE:
            DeleteIcon((LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_MODIFY:
            ModifyIcon((LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_SETFOCUS:
            SetFocus((LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_SETVERSION:
            SetVersion((LPLSNOTIFYICONDATA)lParam);
            break;

        default:
            return FALSE;
        }
        
        return TRUE;

    case LM_SYSTRAYINFOEVENT:
        {
            LPSYSTRAYINFOEVENT lpSTE = (LPSYSTRAYINFOEVENT)wParam;
            switch (lpSTE->dwEvent) {
            case TRAYEVENT_GETICONPOS:
                *(LRESULT*)lParam = MAKELPARAM(100, 100);
                break;

            case TRAYEVENT_GETICONSIZE:
                *(LRESULT*)lParam = MAKELPARAM(20, 20);
                break;

            default:
                TRACE("TrayManager::Unknown LM_SYSTRAYINFOEVENT: %u", lpSTE->dwEvent);
                return FALSE;
            }
        }
        return TRUE;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
