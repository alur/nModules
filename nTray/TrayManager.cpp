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
/// Finds a matching icon in g_currentIcons
/// </summary>
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(LPLSNOTIFYICONDATA pNID) {
    // There are 2 ways to identify an icon. Same guidItem, or same HWND and same uID.
    // TODO::Look into the guidItem part in the core
    for (TRAYICONDATAITER iter = g_currentIcons.begin(); iter != g_currentIcons.end(); iter++) {
        if ((pNID->uFlags & NIF_GUID) == NIF_GUID) {
            if ((*iter)->NID.guidItem == pNID->guidItem) {
                return iter;
            }
        }
        // uID is ignored if guidItem is set
        else if ((*iter)->NID.hWnd == pNID->hWnd && (*iter)->NID.uID == pNID->uID) {
            return iter;
        }
    }
    return g_currentIcons.end();
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
        switch ((DWORD)wParam) {
        case 1:
            return MAKELPARAM(100,100);
        case 2:
            return MAKELPARAM(16,16);
        default:
            TRACE("TrayManager::Unknown LM_SYSTRAYINFOEVENT message: %u", wParam);
            return 0;
        }
        
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
