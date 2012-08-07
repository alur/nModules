/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Bangs.cpp														July, 2012
 *	The nModules Project
 *
 *	Handles bang commands
 *      
 *													             Erik Welander
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../headers/lsapi.h"
#include <strsafe.h>
#include "Bangs.h"
#include "WorkArea.h"
#include "ClickHandler.hpp"

extern MonitorInfo* g_pMonitorInfo;
extern ClickHandler* g_pClickHandler;

namespace Bangs {
	BangItem BangMap[] = {
		{"SetWorkArea", SetWorkArea},
		{"On", On},
		{"Off", Off},
		{NULL, NULL}
	};
}

/// <summary>
///	Registers bangs.
/// </summary>
void Bangs::_Register() {
	char szBangName[MAX_BANGCOMMAND];
	for (int i = 0; BangMap[i].pCommand != NULL; i++) {
		StringCchPrintf(szBangName, MAX_BANGCOMMAND, "!nDesk%s", BangMap[i].szName);
		AddBangCommand(szBangName, BangMap[i].pCommand);
	}
}

/// <summary>
///	Unregisters bangs.
/// </summary>
void Bangs::_Unregister() {
	char szBangName[MAX_BANGCOMMAND];
	for (int i = 0; BangMap[i].pCommand != NULL; i++) {
		StringCchPrintf(szBangName, MAX_BANGCOMMAND, "!nDesk%s", BangMap[i].szName);
		RemoveBangCommand(szBangName);
	}
}

/// <summary>
/// Sets the work area.
/// </summary>
void Bangs::SetWorkArea(HWND, LPCSTR pszArgs) {
	WorkArea::ParseLine(g_pMonitorInfo, pszArgs);
}

/// <summary>
///	Adds a click handler
/// </summary>
void Bangs::On(HWND, LPCSTR pszArgs) {
	g_pClickHandler->AddHandler(pszArgs);
}

/// <summary>
///	Removes click handlers.
/// </summary>
void Bangs::Off(HWND, LPCSTR pszArgs) {
	g_pClickHandler->RemoveHandlers(pszArgs);
}
