/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  EventHandler.cpp
 *  The nModules Project
 *
 *  Handles events...
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "LiteStep.h"
#include <strsafe.h>
#include <Windowsx.h>
#include "EventHandler.hpp"
#include "../nCoreCom/Core.h"


/// <summary>
/// Creates a new instance of the ClickHandler class.
/// </summary>
EventHandler::EventHandler(Settings* settings) {
    this->settings = settings;
    this->mouseOver = false;
    LoadSettings();
}


/// <summary>
/// Destroys this instance of the ClickHandler class.
/// </summary>
EventHandler::~EventHandler() {
    m_clickHandlers.clear();
}


/// <summary>
/// Call this when a click is triggered.
/// </summary>
void EventHandler::HandleMessage(HWND, UINT msg, WPARAM wParam, LPARAM /* lParam */) {
    ClickData cData;

    // Find the type of this click event
    switch (msg)
    {
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
        case WM_MOUSEMOVE:
            {
                if (mouseOver) return;
                mouseOver = true;
                cData.type = ENTER;
            }
            break;
        case WM_MOUSELEAVE: cData.type = LEAVE; mouseOver = false; break;
        case WM_XBUTTONDOWN:
            switch (GET_XBUTTON_WPARAM(wParam))
            {
                case XBUTTON1: cData.type = X1DOWN; break;
                case XBUTTON2: cData.type = X2DOWN; break;
                default: cData.type = UNKNOWN; break;
            }
            break;
        case WM_XBUTTONUP:
            switch (GET_XBUTTON_WPARAM(wParam))
            {
                case XBUTTON1: cData.type = X1UP; break;
                case XBUTTON2: cData.type = X2UP; break;
                default: cData.type = UNKNOWN; break;
            }
            break;
        case WM_XBUTTONDBLCLK:
            switch (GET_XBUTTON_WPARAM(wParam))
            {
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
    //cData.area.left = cData.area.right = GET_X_LPARAM(lParam) + g_pMonitorInfo->m_virtualDesktop.rect.left; 
    //cData.area.top = cData.area.bottom = GET_Y_LPARAM(lParam) + g_pMonitorInfo->m_virtualDesktop.rect.top;

    for (vector<ClickData>::const_iterator iter = m_clickHandlers.begin(); iter != m_clickHandlers.end(); iter++)
    {
        if (EventHandler::Matches(cData, *iter))
        {
            LiteStep::LSExecute(nullptr, iter->action, SW_SHOW);
        }
    }
}


/// <summary>
/// Loads click settings.
/// </summary>
void EventHandler::LoadSettings(bool /* bIsRefresh */)
{
    this->settings->IterateOverStars(_T("On"), std::bind(&EventHandler::AddHandler, this, std::placeholders::_1));
}


/// <summary>
/// Parses a click line.
/// </summary>
EventHandler::ClickData EventHandler::ParseLine(LPCTSTR szLine)
{
    // !nDeskOn <type> <mods> <action>
    // !nDeskOn <type> <mods> <left> <top> <right> <bottom> <action>
    TCHAR szToken[MAX_LINE_LENGTH];
    LPCTSTR pszNext = szLine;
    ClickData cData;

    using namespace LiteStep;

    // Type
    LiteStep::GetToken(pszNext, szToken, &pszNext, false);
    cData.type = TypeFromString(szToken);

    // ModKeys
    LiteStep::GetToken(pszNext, szToken, &pszNext, false);
    cData.mods = ModsFromString(szToken);

    // Guess that the rest is an action for now
    StringCchCopy(cData.action, sizeof(cData.action), pszNext);
    cData.area.left = LONG_MIN; cData.area.right = LONG_MAX;
    cData.area.top = LONG_MIN; cData.area.bottom = LONG_MAX;

    // Check if we have 4 valid coordinates followed by some action
    RelatedNumber left, top, width, height;
    if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseRelated(szToken, &left)) return cData;

    if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseRelated(szToken, &top)) return cData;

    if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseRelated(szToken, &width)) return cData;

    if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
    if (pszNext == NULL) return cData;
    if (!ParseRelated(szToken, &height)) return cData;

    // If these are all valid coordinates
    // TODO::Fix evaluations, or rather don't eval here, for resize purposes
    cData.area.left = (LONG)left.Evaluate(0);
    cData.area.right =  cData.area.left + (LONG)width.Evaluate(0);
    cData.area.top = (LONG)top.Evaluate(0);
    cData.area.bottom = cData.area.top + (LONG)height.Evaluate(0);

    // Then the rest is the action
    StringCchCopy(cData.action, sizeof(cData.action), pszNext);

    return cData;
}


/// <summary>
/// Parses a click line.
/// </summary>
void EventHandler::AddHandler(LPCTSTR szLine) {
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
void EventHandler::RemoveHandlers(LPCTSTR szLine) {
    ClickData cData = ParseLine(szLine);
    for (vector<ClickData>::const_iterator iter = m_clickHandlers.begin(); iter != m_clickHandlers.end(); iter++)
    {
        if (Matches(*iter, cData))
        {
            m_clickHandlers.erase(iter);
        }
    }
}


/// <summary>
/// True if ClickData a is "in" ClickData b.
/// </summary>
bool EventHandler::Matches(ClickData a, ClickData b) {
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
EventHandler::EventType EventHandler::TypeFromString(LPCTSTR str) {
    if (_tcsicmp(str, _T("WheelUp")) == 0) return WHEELUP;
    if (_tcsicmp(str, _T("WheelDown")) == 0) return WHEELDOWN;
    if (_tcsicmp(str, _T("WheelRight")) == 0) return WHEELRIGHT;
    if (_tcsicmp(str, _T("WheelLeft")) == 0) return WHEELLEFT;

    if (_tcsicmp(str, _T("LeftClickDown")) == 0) return LEFTDOWN;
    if (_tcsicmp(str, _T("LeftClickUp")) == 0) return LEFTUP;
    if (_tcsicmp(str, _T("LeftDoubleClick")) == 0) return LEFTDOUBLE;

    if (_tcsicmp(str, _T("MiddleClickDown")) == 0) return MIDDLEDOWN;
    if (_tcsicmp(str, _T("MiddleClickUp")) == 0) return MIDDLEUP;
    if (_tcsicmp(str, _T("MiddleDoubleClick")) == 0) return MIDDLEDOUBLE;

    if (_tcsicmp(str, _T("RightClickDown")) == 0) return RIGHTDOWN;
    if (_tcsicmp(str, _T("RightClickUp")) == 0) return RIGHTUP;
    if (_tcsicmp(str, _T("RightDoubleClick")) == 0) return RIGHTDOUBLE;

    if (_tcsicmp(str, _T("X1ClickDown")) == 0) return X1DOWN;
    if (_tcsicmp(str, _T("X1ClickUp")) == 0) return X1UP;
    if (_tcsicmp(str, _T("X1DoubleClick")) == 0) return X1DOUBLE;

    if (_tcsicmp(str, _T("X2ClickDown")) == 0) return X2DOWN;
    if (_tcsicmp(str, _T("X2ClickUp")) == 0) return X2UP;
    if (_tcsicmp(str, _T("X2DoubleClick")) == 0) return X2DOUBLE;

    if (_tcsicmp(str, _T("Leave")) == 0) return LEAVE;
    if (_tcsicmp(str, _T("Enter")) == 0) return ENTER;

    return UNKNOWN;
}


/// <summary>
/// Gets the mod value from a string.
/// </summary>
WORD EventHandler::ModsFromString(LPTSTR str) {
    WORD ret = 0x0000;

    LPTSTR context;
    LPTSTR tok = _tcstok_s(str, _T("+"), &context);
    while (tok != NULL) {
        if (_tcsicmp(tok, _T("ctrl")) == 0) ret |= MK_CONTROL;
        else if (_tcsicmp(tok, _T("mouseleft")) == 0) ret |= MK_LBUTTON;
        else if (_tcsicmp(tok, _T("mousemiddle")) == 0) ret |= MK_MBUTTON;
        else if (_tcsicmp(tok, _T("mouseright")) == 0) ret |= MK_RBUTTON;
        else if (_tcsicmp(tok, _T("shift")) == 0) ret |= MK_SHIFT;
        else if (_tcsicmp(tok, _T("mousex1")) == 0) ret |= MK_XBUTTON1;
        else if (_tcsicmp(tok, _T("mousex2")) == 0) ret |= MK_XBUTTON2;
        else if (_tcsicmp(tok, _T("alt"))  == 0) ret |= MK_ALT;
        tok = _tcstok_s(NULL, _T("+"), &context);
    }

    return ret;
}
