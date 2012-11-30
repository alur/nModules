/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  TrayManager.cpp
 *  The nModules Project
 *
 *  Keeps track of the system tray icons and notifies the trays of changes.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "Tray.hpp"
#include "TrayManager.h"
#include "../nShared/Debugging.h"


// The various trays
extern map<string, Tray*> g_Trays;


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
/// Gets the screen rect of an icon.
/// </summary>
void TrayManager::GetScreenRect(TRAYICONDATAITER icon, LPRECT rect) {
    if ((*icon)->icons->size() > 0) {
        (*(*icon)->icons)[0]->GetScreenRect(rect);
    }
    else {
        // We could define a rectangle for icons that arent included anywhere, instead of just zeroing.
        ZeroMemory(rect, sizeof(RECT));
    }
}


/// <summary>
/// Finds a matching icon in g_currentIcons.
/// </summary>
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(GUID guid) {
    for (TRAYICONDATAITER iter = g_currentIcons.begin(); iter != g_currentIcons.end(); iter++) {
        if ((*iter)->guidItem == guid) {
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
        if ((*iter)->hwnd == hWnd && (*iter)->uID == uID) {
            return iter;
        }
    }
    return g_currentIcons.end();
}


/// <summary>
/// Finds a matching icon in g_currentIcons.
/// </summary>
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
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
TrayManager::TRAYICONDATAITER TrayManager::FindIcon(LiteStep::LPSYSTRAYINFOEVENT pSTE) {
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
void TrayManager::AddIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
    if (FindIcon(pNID) == g_currentIcons.end()) {
        TRAYICONDATA* tData = (TRAYICONDATA*)malloc(sizeof(TRAYICONDATA));
        tData->icons = new vector<TrayIcon*>;
        if ((NIF_GUID & pNID->uFlags) == NIF_GUID) {
            tData->guidItem = pNID->guidItem;
        }
        else {
            tData->guidItem = GUID_NULL;
        }
        tData->hwnd = pNID->hWnd;
        tData->uID = pNID->uID;
        
        for (TRAYSCITER iter = g_Trays.begin(); iter != g_Trays.end(); iter++) {
            TrayIcon* pIcon = iter->second->AddIcon(pNID);
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
void TrayManager::DeleteIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
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
/// <param name="pNID"></param>
void TrayManager::ModifyIcon(LiteStep::LPLSNOTIFYICONDATA pNID) {
    TRAYICONDATAITER icon = FindIcon(pNID);
    if (icon != g_currentIcons.end()) {
        for (vector<TrayIcon*>::const_iterator iter = (*icon)->icons->begin(); iter != (*icon)->icons->end(); iter++) {
            (*iter)->HandleModify(pNID);
        }
    }
    else {
        TRACE("Tried to modify non-existing icon");
    }
}


/// <summary>
/// Returns the focus to one of the trays.
/// </summary>
/// <param name="pNID">The </param>
void TrayManager::SetFocus(LiteStep::LPLSNOTIFYICONDATA pNID) {
    UNREFERENCED_PARAMETER(pNID);
}


/// <summary>
/// Changes the version of an existing tray icon.
/// </summary>
void TrayManager::SetVersion(LiteStep::LPLSNOTIFYICONDATA pNID) {
    TRAYICONDATAITER icon = FindIcon(pNID);
    if (icon != g_currentIcons.end()) {
        for (vector<TrayIcon*>::const_iterator iter = (*icon)->icons->begin(); iter != (*icon)->icons->end(); iter++) {
            (*iter)->HandleSetVersion(pNID);
        }
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
            AddIcon((LiteStep::LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_DELETE:
            DeleteIcon((LiteStep::LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_MODIFY:
            ModifyIcon((LiteStep::LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_SETFOCUS:
            SetFocus((LiteStep::LPLSNOTIFYICONDATA)lParam);
            break;
            
        case NIM_SETVERSION:
            SetVersion((LiteStep::LPLSNOTIFYICONDATA)lParam);
            break;

        default:
            TRACE("TrayManager::Unknown LM_SYSTRAY wParam: %u", wParam);
            return FALSE;
        }
        
        return TRUE;

    case LM_SYSTRAYINFOEVENT:
        {
            LiteStep::LPSYSTRAYINFOEVENT lpSTE = (LiteStep::LPSYSTRAYINFOEVENT)wParam;
            TRAYICONDATAITER icon = FindIcon(lpSTE);
            RECT r;

            if (icon == g_currentIcons.end()) {
                return FALSE;
            }
            GetScreenRect(icon, &r);

            switch (lpSTE->dwEvent) {
            case TRAYEVENT_GETICONPOS:
                {
                    *(LRESULT*)lParam = MAKELPARAM(r.left, r.top);
                }
                break;

            case TRAYEVENT_GETICONSIZE:
                {
                    *(LRESULT*)lParam = MAKELPARAM(r.right - r.left, r.bottom - r.top);
                }
                break;

            default:
                {
                    TRACE("TrayManager::Unknown LM_SYSTRAYINFOEVENT: %u", lpSTE->dwEvent);
                }
                return FALSE;
            }
        }
        return TRUE;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


/// <summary>
/// Called when the init phase has ended.
/// <summary>
void TrayManager::InitCompleted() {
    for (TRAYSCITER iter = g_Trays.begin(); iter != g_Trays.end(); iter++) {
        iter->second->InitCompleted();
    }
}
