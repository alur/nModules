#include "Api.h"
#include "EventHandler.hpp"
#include "Logger.hpp"

#include "../nShared/AlgorithmExt.h"
#include "../nShared/String.h"

#include "../nCoreApi/Messages.h"

#include "../Headers/lsapi.h"
#include "../Headers/Macros.h"

extern Logger *gLogger;

static StringKeyedMaps<LPCWSTR, EventHandler::Type>::ConstUnorderedMap stringToType({
  { L"WheelUp", EventHandler::Type::WHEELUP },
  { L"WheelDown", EventHandler::Type::WHEELDOWN },
  { L"WheelRight", EventHandler::Type::WHEELRIGHT },
  { L"WheelLeft", EventHandler::Type::WHEELLEFT },
  { L"LeftClickDown", EventHandler::Type::LEFTDOWN },
  { L"LeftClickUp", EventHandler::Type::LEFTUP },
  { L"LeftDoubleClick", EventHandler::Type::LEFTDOUBLE },
  { L"MiddleClickDown", EventHandler::Type::MIDDLEDOWN },
  { L"MiddleClickUp", EventHandler::Type::MIDDLEUP },
  { L"MiddleDoubleClick", EventHandler::Type::MIDDLEDOUBLE },
  { L"RightClickDown", EventHandler::Type::RIGHTDOWN },
  { L"RightClickUp", EventHandler::Type::RIGHTUP },
  { L"RightDoubleClick", EventHandler::Type::RIGHTDOUBLE },
  { L"X1ClickDown", EventHandler::Type::X1DOWN },
  { L"X1ClickUp", EventHandler::Type::X1UP },
  { L"X1DoubleClick", EventHandler::Type::X1DOUBLE },
  { L"X2ClickDown", EventHandler::Type::X2DOWN },
  { L"X2ClickUp", EventHandler::Type::X2UP },
  { L"X2DoubleClick", EventHandler::Type::X2DOUBLE },
  { L"Leave", EventHandler::Type::LEAVE },
  { L"Enter", EventHandler::Type::ENTER }
});


EXPORT_CDECL(IEventHandler*) CreateEventHandler(const ISettingsReader *settingsReader) {
  return new EventHandler(settingsReader);
}


EventHandler::EventHandler(const ISettingsReader *settingsReader) {
  settingsReader->EnumCommandLines(L"On", [] (LPCWSTR line, LPARAM lParam) -> void {
    ((EventHandler*)lParam)->AddEventAction(line);
  }, (LPARAM)this);
}


void EventHandler::AddEventAction(LPCWSTR line) {
  wchar_t action[MAX_LINE_LENGTH];
  DWORD id = ParseEventLine(line, action, _countof(action));
  if (id) {
    mEventActions.emplace(id, action);
  } else {
    gLogger->Warning(L"Invalid event line encountered: %ls", line);
  }
}


DWORD EventHandler::ParseEventLine(LPCWSTR line, LPWSTR action, size_t cchAction) {
  wchar_t token[MAX_LINE_LENGTH];

  Type type;
  if (GetToken(line, token, &line, FALSE) == FALSE) {
    return 0;
  }
  type = nstd::get(stringToType, (LPCWSTR)token, Type::UNKNOWN);

  WORD mods;
  if (GetToken(line, token, &line, FALSE) == FALSE) {
    return 0;
  }

  if (line == nullptr) {
    return 0;
  }

  mods = ModsFromString(token);

  StringCchCopy(action, cchAction, line);
  return MAKELONG(type, mods);
}


void EventHandler::Discard() {
  delete this;
}


LRESULT EventHandler::HandleMessage(HWND window, UINT msg, WPARAM wParam, LPARAM lParam,
    IEventProcessor*) {
  // Find the type of this click event
  Type type;
  switch (msg) {
  case WM_MOUSEWHEEL: type = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? Type::WHEELUP : Type::WHEELDOWN; break;
  case WM_MOUSEHWHEEL: type = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? Type::WHEELRIGHT : Type::WHEELLEFT; break;
  case WM_LBUTTONDOWN: type = Type::LEFTDOWN; break;
  case WM_LBUTTONUP: type = Type::LEFTUP; break;
  case WM_LBUTTONDBLCLK: type = Type::LEFTDOUBLE; break;
  case WM_MBUTTONDOWN: type = Type::MIDDLEDOWN; break;
  case WM_MBUTTONUP: type = Type::MIDDLEUP; break;
  case WM_MBUTTONDBLCLK: type = Type::MIDDLEDOUBLE; break;
  case WM_RBUTTONDOWN: type = Type::RIGHTDOWN; break;
  case WM_RBUTTONUP: type = Type::RIGHTUP; break;
  case WM_RBUTTONDBLCLK: type = Type::RIGHTDOUBLE; break;
  case NCORE_WM_MOUSEENTER: type = Type::ENTER; break;
  case WM_MOUSELEAVE: type = Type::LEAVE; break;
  case WM_XBUTTONDOWN:
    switch (GET_XBUTTON_WPARAM(wParam)) {
    case XBUTTON1: type = Type::X1DOWN; break;
    case XBUTTON2: type = Type::X2DOWN; break;
    default: type = Type::UNKNOWN;
    }
    break;
  case WM_XBUTTONUP:
    switch (GET_XBUTTON_WPARAM(wParam)) {
    case XBUTTON1: type = Type::X1UP; break;
    case XBUTTON2: type = Type::X2UP; break;
    default: type = Type::UNKNOWN;
    }
    break;
  case WM_XBUTTONDBLCLK:
    switch (GET_XBUTTON_WPARAM(wParam)) {
    case XBUTTON1: type = Type::X1DOUBLE; break;
    case XBUTTON2: type = Type::X2DOUBLE; break;
    default: type = Type::UNKNOWN;
    }
    break;
  default: type = Type::UNKNOWN;
  }
  WORD mods = GET_KEYSTATE_WPARAM(wParam) & (4 | 8);
  if (GetAsyncKeyState(VK_MENU) != 0) {
    mods |= MK_XBUTTON2 << 1;
  }

  auto range = mEventActions.equal_range(MAKELONG(type, mods));
  for (auto action = range.first; action != range.second; ++action) {
    LSExecute(nullptr, action->second.c_str(), SW_SHOW);
  }

  return DefWindowProc(window, msg, wParam, lParam);
};


/// <summary>
/// Gets the mod value from a string.
/// </summary>
WORD EventHandler::ModsFromString(LPWSTR str) {
  static StringKeyedMaps<LPCWSTR, WORD>::ConstUnorderedMap stringToMod({
    { L"ctrl", WORD(MK_CONTROL) },
    { L"mouseleft", WORD(MK_LBUTTON) },
    { L"mousemiddle", WORD(MK_MBUTTON) },
    { L"mouseright", WORD(MK_RBUTTON) },
    { L"shift", WORD(MK_SHIFT) },
    { L"mousex1", WORD(MK_XBUTTON1) },
    { L"mousex2", WORD(MK_XBUTTON2) },
    { L"alt", WORD(MK_XBUTTON2 << 1) }
  });

  WORD mods = 0x0000;
  LPWSTR context, token = wcstok_s(str, L"+", &context);
  while (token != nullptr) {
    mods |= nstd::get(stringToMod, (LPCWSTR)token, (WORD)0);
    token = wcstok_s(NULL, L"+", &context);
  }
  return mods;
}
