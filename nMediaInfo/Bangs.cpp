/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Bangs.cpp
 *  The nModules Project
 *
 *  Handles bang commands
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include <strsafe.h>
#include "Bangs.h"
#include "../nCoreCom/Core.h"

#define IPC_STARTPLAY 102

namespace Bangs {
    BangItem BangMap[] = {
        { "MediaPrevious",      MediaPrevious       },
        { "MediaPlay",          MediaPlay           },
        { "MediaPause",         MediaPause          },
        { "MediaPlayPause",     MediaPlayPause      },
        { "MediaStop",          MediaStop           },
        { "MediaNext",          MediaNext           },
        { NULL,                 NULL                }
    };
}


/// <summary>
/// Registers bangs.
/// </summary>
void Bangs::_Register() {
    char szBangName[64];
    for (int i = 0; BangMap[i].pCommand != NULL; i++) {
        StringCchPrintf(szBangName, sizeof(szBangName), "!%s", BangMap[i].szName);
        LiteStep::AddBangCommand(szBangName, BangMap[i].pCommand);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
    char szBangName[64];
    for (int i = 0; BangMap[i].pCommand != NULL; i++) {
        StringCchPrintf(szBangName, sizeof(szBangName), "!%s", BangMap[i].szName);
        LiteStep::RemoveBangCommand(szBangName);
    }
}


/// <summary>
/// Sets the work area.
/// </summary>
void Bangs::MediaPrevious(HWND, LPCSTR pszArgs) {
    HWND WA2Window = FindWindow("Winamp v1.x", NULL);
    if (WA2Window) {
        SendMessage(WA2Window, WM_COMMAND, 40044, 0);
    }
}


/// <summary>
/// Adds a click handler.
/// </summary>
void Bangs::MediaPlay(HWND, LPCSTR pszArgs) {
    HWND WA2Window = FindWindow("Winamp v1.x", NULL);
    if (WA2Window) {
        SendMessage(WA2Window, WM_COMMAND, 40045, 0);
    }
}


/// <summary>
/// Removes click handlers.
/// </summary>
void Bangs::MediaPause(HWND, LPCSTR pszArgs) {
    HWND WA2Window = FindWindow("Winamp v1.x", NULL);
    if (WA2Window) {
        SendMessage(WA2Window, WM_COMMAND, 40046, 0);
    }
}


/// <summary>
/// Sets the transition duration.
/// </summary>
void Bangs::MediaPlayPause(HWND, LPCSTR pszArgs) {
}


/// <summary>
/// Sets the transition square size. Force it to be >= 2.
/// </summary>
void Bangs::MediaStop(HWND, LPCSTR pszArgs) {
    HWND WA2Window = FindWindow("Winamp v1.x", NULL);
    if (WA2Window) {
        SendMessage(WA2Window, WM_COMMAND, 40047, 0);
    }
}


/// <summary>
/// Sets the transition effect.
/// </summary>
void Bangs::MediaNext(HWND, LPCSTR pszArgs) {
    HWND WA2Window = FindWindow("Winamp v1.x", NULL);
    if (WA2Window) {
        SendMessage(WA2Window, WM_COMMAND, 40048, 0);
    }
}
