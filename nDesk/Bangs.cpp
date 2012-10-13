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
#include "WorkArea.h"
#include "ClickHandler.hpp"
#include "DesktopPainter.hpp"
#include "Settings.h"
#include "../nCoreCom/Core.h"

extern MonitorInfo* g_pMonitorInfo;
extern ClickHandler* g_pClickHandler;
extern DesktopPainter* g_pDesktopPainter;

namespace Bangs {
    BangItem BangMap[] = {
        { "SetWorkArea",                SetWorkArea                },
        { "On",                         On                         },
        { "Off",                        Off                        },
        { "SetTransitionDuration",      SetTransitionDuration      },
        { "SetTransitionFrameInterval", SetTransitionFrameInterval },
        { "SetTransitionSquareSize",    SetTransitionSquareSize    },
        { "SetTransitionEffect",        SetTransitionEffect        },
        { "SetInvalidateAllOnUpdate",   SetInvalidateAllOnUpdate   },
        { NULL,                         NULL                       }
    };
}


/// <summary>
/// Registers bangs.
/// </summary>
void Bangs::_Register() {
    char szBangName[64];
    for (int i = 0; BangMap[i].pCommand != NULL; i++) {
        StringCchPrintf(szBangName, sizeof(szBangName), "!nDesk%s", BangMap[i].szName);
        LiteStep::AddBangCommand(szBangName, BangMap[i].pCommand);
    }
}


/// <summary>
/// Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
    char szBangName[64];
    for (int i = 0; BangMap[i].pCommand != NULL; i++) {
        StringCchPrintf(szBangName, sizeof(szBangName), "!nDesk%s", BangMap[i].szName);
        LiteStep::RemoveBangCommand(szBangName);
    }
}


/// <summary>
/// Sets the work area.
/// </summary>
void Bangs::SetWorkArea(HWND, LPCSTR pszArgs) {
    WorkArea::ParseLine(g_pMonitorInfo, pszArgs);
}


/// <summary>
/// Adds a click handler.
/// </summary>
void Bangs::On(HWND, LPCSTR pszArgs) {
    g_pClickHandler->AddHandler(pszArgs);
}


/// <summary>
/// Removes click handlers.
/// </summary>
void Bangs::Off(HWND, LPCSTR pszArgs) {
    g_pClickHandler->RemoveHandlers(pszArgs);
}


/// <summary>
/// Sets the transition duration.
/// </summary>
void Bangs::SetTransitionDuration(HWND, LPCSTR pszArgs) {
    g_pDesktopPainter->SetTransitionTime(atoi(pszArgs));
}


/// <summary>
/// Sets the transition frame interval. Force it to be in the range of 10 to 50.
/// </summary>
void Bangs::SetTransitionFrameInterval(HWND, LPCSTR pszArgs) {
    g_pDesktopPainter->SetFrameInterval(min(50, max(10, atoi(pszArgs))));
}


/// <summary>
/// Sets the transition square size. Force it to be >= 2.
/// </summary>
void Bangs::SetTransitionSquareSize(HWND, LPCSTR pszArgs) {
    g_pDesktopPainter->SetSquareSize(max(2, atoi(pszArgs)));
}


/// <summary>
/// Sets the transition effect.
/// </summary>
void Bangs::SetTransitionEffect(HWND, LPCSTR pszArgs) {
    g_pDesktopPainter->SetTransitionType(nDesk::Settings::TransitionTypeFromString(pszArgs));
}


/// <summary>
/// Sets the transition effect.
/// </summary>
void Bangs::SetInvalidateAllOnUpdate(HWND, LPCSTR pszArgs) {
    g_pDesktopPainter->SetInvalidateAllOnUpdate(nCore::InputParsing::ParseBool(pszArgs));
}
