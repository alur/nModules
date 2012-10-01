/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  ClickHandler.cpp
 *  The nModules Project
 *
 *  Handles clicks on the desktop.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <Windows.h>
#include <strsafe.h>
#include <Windowsx.h>
#include "ClickHandler.hpp"
#include "../nShared/MonitorInfo.hpp"
#include "../nCoreCom/Core.h"

extern MonitorInfo * g_pMonitorInfo;


/// <summary>
/// Creates a new instance of the ClickHandler class.
/// </summary>
ClickHandler::ClickHandler() {
    LoadSettings();
}


/// <summary>
/// Destroys this instance of the ClickHandler class.
/// </summary>
ClickHandler::~ClickHandler() {
    m_clickHandlers.clear();
}


/// <summary>
/// Call this when a click is triggered.
/// </summary>
void ClickHandler::HandleClick(UINT msg, WPARAM wParam, LPARAM lParam) {
    ClickData cData;

    // Find the type of this click event
    switch (msg) {
        case WM_MOUSEWHEEL: cData.type = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? WHEELUP : WHEELDOWN; break;
        case WM_MOUSEHWHEEL: cData.type = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? WHEELRIGHT : WHEELLEFT; break;
        case WM_LBUTTONDOWN: cData.type = LEFTDOWN; break;
        case WM_LBUTTONUP: cData.type = LEFTUP; break;
        case WM_LBUTTONDBLCLK: cData.type = LEFTDOUBLE; break;
        case WM_MBUTTONDOWN: cData.type = MIDDLEDOWN; break;
        case WM_MBUTTONUP: cData.type = MIDDLEUP; break;
        case WM_MBUTTONDBLCLK: cData.type = MIDDLEDOUBLE; break;
        case WM_RBUTTONDOWN: cData.type = RIGHTDOWN; break;
        case WM_RBUTTONUP: cData.type = RIGHTUP; break;
        case WM_RBUTTONDBLCLK: cData.type = RIGHTDOUBLE; break;
        case WM_XBUTTONDOWN:
            switch (GET_XBUTTON_WPARAM(wParam)) {
                case XBUTTON1: cData.type = X1DOWN; break;
                case XBUTTON2: cData.type = X2DOWN; break;
                default: cData.type = UNKNOWN; break;
            }
            break;
        case WM_XBUTTONUP:
            switch (GET_XBUTTON_WPARAM(wParam)) {
                case XBUTTON1: cData.type = X1UP; break;
                case XBUTTON2: cData.type = X2UP; break;
                default: cData.type = UNKNOWN; break;
            }
            break;
        case WM_XBUTTONDBLCLK:
            switch (GET_XBUTTON_WPARAM(wParam)) {
                case XBUTTON1: cData.type = X1DOUBLE; break;
                case XBUTTON2: cData.type = X2DOUBLE; break;
                default: cData.type = UNKNOWN; break;
            }
            break;
        default: cData.type = UNKNOWN; break;
    }

    cData.mods = GET_KEYSTATE_WPARAM(wParam) & (4 | 8);
    // GET_X_LPARAM and GET_Y_LPARAM are relative to the desktop window.
    // cData.area is relative to the virual desktop.
    cData.area.left = cData.area.right = GET_X_LPARAM(lParam) + g_pMonitorInfo->m_virtualDesktop.rect.left; 
    cData.area.top = cData.area.bottom = GET_Y_LPARAM(lParam) + g_pMonitorInfo->m_virtualDesktop.rect.top;

    for (vector<ClickData>::const_iterator iter = m_clickHandlers.begin(); iter != m_clickHandlers.end(); iter++) {
        if (ClickHandler::Matches(cData, *iter)) {
            LSExecute(NULL, iter->action, SW_SHOW);
        }
    }
}


/// <summary>
/// Reloads click settings.
/// </summary>
void ClickHandler::Refresh() {
    LoadSettings(true);
}


/// <summary>
/// Loads click settings.
/// </summary>
void ClickHandler::LoadSettings(bool /* bIsRefresh */) {
    char szLine[MAX_LINE_LENGTH];
    LPVOID f = LCOpen(NULL);
    while (LCReadNextConfig(f, "*nDeskOn", szLine, sizeof(szLine))) {
        AddHandler(szLine+strlen("*nDeskOn")+1);
    }
    LCClose(f);
}


/// <summary>
/// Parses a click line.
/// </summary>
ClickHandler::ClickData ClickHandler::ParseLine(LPCSTR szLine) {
    // !nDeskOn <type> <mods> <action>
    // !nDeskOn <type> <mods> <left> <top> <right> <bottom> <action>
    char szToken[MAX_LINE_LENGTH];
    LPCSTR pszNext = szLine;
    ClickData cData;

    using nCore::InputParsing::ParseCoordinate;

    // Type
    GetToken(pszNext, szToken, &pszNext, false);
    cData.type = TypeFromString(szToken);

    // ModKeys
    GetToken(pszNext, szToken, &pszNext, false);
    cData.mods = ModsFromString(szToken);

    // Guess that the rest is an action for now
    StringCchCopy(cData.action, sizeof(cData.action), pszNext);
    cData.area = g_pMonitorInfo->m_virtualDesktop.rect;

    // Check if we have 4 valid coordinates followed by some action
    int left, top, width, height;
    if (GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseCoordinate(szToken, &left)) return cData;

    if (GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseCoordinate(szToken, &top)) return cData;

    if (GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseCoordinate(szToken, &width)) return cData;

    if (GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseCoordinate(szToken, &height)) return cData;

    // If these are all valid coordinates
    cData.area.left = left;
    cData.area.right =  left + width;
    cData.area.top = top;
    cData.area.bottom = top + height;

    // Then the rest is the action
    StringCchCopy(cData.action, sizeof(cData.action), pszNext);

    return cData;
}


/// <summary>
/// Parses a click line.
/// </summary>
void ClickHandler::AddHandler(LPCSTR szLine) {
    ClickData cData = ParseLine(szLine);
    if (cData.type == UNKNOWN) {
        // TODO::Throw an error
        return;
    }

    this->m_clickHandlers.push_back(cData);
}


/// <summary>
/// Removes any handlers matching the spcified criterias.
/// </summary>
void ClickHandler::RemoveHandlers(LPCSTR szLine) {
    ClickData cData = ParseLine(szLine);
    for (vector<ClickData>::const_iterator iter = m_clickHandlers.begin(); iter != m_clickHandlers.end(); iter++) {
        if (Matches(*iter, cData)) {
            m_clickHandlers.erase(iter);
        }
    }
}


/// <summary>
/// True if ClickData a is "in" ClickData b.
/// </summary>
bool ClickHandler::Matches(ClickData a, ClickData b) {
    return  a.type == b.type &&
            a.mods == b.mods &&
            a.area.left >= b.area.left &&
            a.area.right <= b.area.right &&
            a.area.top >= b.area.top &&
            a.area.bottom <= b.area.bottom;
}


/// <summary>
/// Gets the clicktype from a user input string.
/// </summary>
ClickHandler::ClickType ClickHandler::TypeFromString(LPCSTR str) {
    if (_stricmp(str, "WheelUp") == 0) return WHEELUP;
    if (_stricmp(str, "WheelDown") == 0) return WHEELDOWN;
    if (_stricmp(str, "WheelRight") == 0) return WHEELRIGHT;
    if (_stricmp(str, "WheelLeft") == 0) return WHEELLEFT;

    if (_stricmp(str, "LeftClickDown") == 0) return LEFTDOWN;
    if (_stricmp(str, "LeftClickUp") == 0) return LEFTUP;
    if (_stricmp(str, "LeftDoubleClick") == 0) return LEFTDOUBLE;

    if (_stricmp(str, "MiddleClickDown") == 0) return MIDDLEDOWN;
    if (_stricmp(str, "MiddleClickUp") == 0) return MIDDLEUP;
    if (_stricmp(str, "MiddleDoubleClick") == 0) return MIDDLEDOUBLE;

    if (_stricmp(str, "RightClickDown") == 0) return RIGHTDOWN;
    if (_stricmp(str, "RightClickUp") == 0) return RIGHTUP;
    if (_stricmp(str, "RightDoubleClick") == 0) return RIGHTDOUBLE;

    if (_stricmp(str, "X1ClickDown") == 0) return X1DOWN;
    if (_stricmp(str, "X1ClickUp") == 0) return X1UP;
    if (_stricmp(str, "X1DoubleClick") == 0) return X1DOUBLE;

    if (_stricmp(str, "X2ClickDown") == 0) return X2DOWN;
    if (_stricmp(str, "X2ClickUp") == 0) return X2UP;
    if (_stricmp(str, "X2DoubleClick") == 0) return X2DOUBLE;

    return UNKNOWN;
}


/// <summary>
/// Gets the mod value from a string.
/// </summary>
WORD ClickHandler::ModsFromString(LPSTR str) {
    WORD ret = 0x0000;

    char * context;
    LPSTR tok = strtok_s(str, "+", &context);
    while (tok != NULL) {
        if (_stricmp(tok, "ctrl") == 0) ret |= MK_CONTROL;
        else if (_stricmp(tok, "mouseleft") == 0) ret |= MK_LBUTTON;
        else if (_stricmp(tok, "mousemiddle") == 0) ret |= MK_MBUTTON;
        else if (_stricmp(tok, "mouseright") == 0) ret |= MK_RBUTTON;
        else if (_stricmp(tok, "shift") == 0) ret |= MK_SHIFT;
        else if (_stricmp(tok, "mousex1") == 0) ret |= MK_XBUTTON1;
        else if (_stricmp(tok, "mousex2") == 0) ret |= MK_XBUTTON2;
        tok = strtok_s(NULL, "+", &context);
    }

    return ret;
}
