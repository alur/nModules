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
#include "Distance.hpp"


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
  case WM_MOUSEMOVE:
    {
      if (mouseOver) return;
      mouseOver = true;
      cData.type = ENTER;
    }
    break;
  case WM_MOUSELEAVE: cData.type = LEAVE; mouseOver = false; break;
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
  //cData.area.left = cData.area.right = GET_X_LPARAM(lParam) + g_pMonitorInfo->m_virtualDesktop.rect.left;
  //cData.area.top = cData.area.bottom = GET_Y_LPARAM(lParam) + g_pMonitorInfo->m_virtualDesktop.rect.top;

  for (vector<ClickData>::const_iterator iter = m_clickHandlers.begin(); iter != m_clickHandlers.end(); iter++) {
    if (EventHandler::Matches(cData, *iter)) {
      LiteStep::LSExecute(nullptr, iter->action, SW_SHOW);
    }
  }
}


/// <summary>
/// Loads click settings.
/// </summary>
void EventHandler::LoadSettings(bool /* bIsRefresh */) {
  this->settings->IterateOverCommandLines(L"On", std::bind(&EventHandler::AddHandler, this, std::placeholders::_1));
}


/// <summary>
/// Parses a click line.
/// </summary>
EventHandler::ClickData EventHandler::ParseLine(LPCTSTR szLine) {
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

  if (pszNext == nullptr) {
    cData.type = UNKNOWN;
    return cData;
  }

  // Guess that the rest is an action for now
  StringCchCopy(cData.action, sizeof(cData.action), pszNext);
  cData.area.left = LONG_MIN; cData.area.right = LONG_MAX;
  cData.area.top = LONG_MIN; cData.area.bottom = LONG_MAX;

  // Check if we have 4 valid coordinates followed by some action
  Distance left, top, width, height;
  if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
  if (pszNext == NULL) return cData;
  if (!Distance::Parse(szToken, left)) return cData;

  if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
  if (pszNext == NULL) return cData;
  if (!Distance::Parse(szToken, top)) return cData;

  if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
  if (pszNext == NULL) return cData;
  if (!Distance::Parse(szToken, width)) return cData;

  if (LiteStep::GetToken(pszNext, szToken, &pszNext, false) == FALSE) return cData;
  if (pszNext == NULL) return cData;
  if (!Distance::Parse(szToken, height)) return cData;

  // If these are all valid coordinates
  // TODO::Fix evaluations, or rather don't eval here, for resize purposes
  cData.area.left = (LONG)left.Evaluate(0);
  cData.area.right = cData.area.left + (LONG)width.Evaluate(0);
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
  for (vector<ClickData>::const_iterator iter = m_clickHandlers.begin(); iter != m_clickHandlers.end(); iter++) {
    if (Matches(*iter, cData)) {
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
  if (_wcsicmp(str, L"WheelUp") == 0) return WHEELUP;
  if (_wcsicmp(str, L"WheelDown") == 0) return WHEELDOWN;
  if (_wcsicmp(str, L"WheelRight") == 0) return WHEELRIGHT;
  if (_wcsicmp(str, L"WheelLeft") == 0) return WHEELLEFT;

  if (_wcsicmp(str, L"LeftClickDown") == 0) return LEFTDOWN;
  if (_wcsicmp(str, L"LeftClickUp") == 0) return LEFTUP;
  if (_wcsicmp(str, L"LeftDoubleClick") == 0) return LEFTDOUBLE;

  if (_wcsicmp(str, L"MiddleClickDown") == 0) return MIDDLEDOWN;
  if (_wcsicmp(str, L"MiddleClickUp") == 0) return MIDDLEUP;
  if (_wcsicmp(str, L"MiddleDoubleClick") == 0) return MIDDLEDOUBLE;

  if (_wcsicmp(str, L"RightClickDown") == 0) return RIGHTDOWN;
  if (_wcsicmp(str, L"RightClickUp") == 0) return RIGHTUP;
  if (_wcsicmp(str, L"RightDoubleClick") == 0) return RIGHTDOUBLE;

  if (_wcsicmp(str, L"X1ClickDown") == 0) return X1DOWN;
  if (_wcsicmp(str, L"X1ClickUp") == 0) return X1UP;
  if (_wcsicmp(str, L"X1DoubleClick") == 0) return X1DOUBLE;

  if (_wcsicmp(str, L"X2ClickDown") == 0) return X2DOWN;
  if (_wcsicmp(str, L"X2ClickUp") == 0) return X2UP;
  if (_wcsicmp(str, L"X2DoubleClick") == 0) return X2DOUBLE;

  if (_wcsicmp(str, L"Leave") == 0) return LEAVE;
  if (_wcsicmp(str, L"Enter") == 0) return ENTER;

  return UNKNOWN;
}


/// <summary>
/// Gets the mod value from a string.
/// </summary>
WORD EventHandler::ModsFromString(LPTSTR str) {
  WORD ret = 0x0000;

  LPTSTR context;
  LPTSTR tok = wcstok_s(str, L"+", &context);
  while (tok != NULL) {
    if (_wcsicmp(tok, L"ctrl") == 0) ret |= MK_CONTROL;
    else if (_wcsicmp(tok, L"mouseleft") == 0) ret |= MK_LBUTTON;
    else if (_wcsicmp(tok, L"mousemiddle") == 0) ret |= MK_MBUTTON;
    else if (_wcsicmp(tok, L"mouseright") == 0) ret |= MK_RBUTTON;
    else if (_wcsicmp(tok, L"shift") == 0) ret |= MK_SHIFT;
    else if (_wcsicmp(tok, L"mousex1") == 0) ret |= MK_XBUTTON1;
    else if (_wcsicmp(tok, L"mousex2") == 0) ret |= MK_XBUTTON2;
    else if (_wcsicmp(tok, L"alt") == 0) ret |= MK_ALT;
    tok = wcstok_s(NULL, L"+", &context);
  }

  return ret;
}
