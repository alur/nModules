/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  EventHandler.hpp
 *  The nModules Project
 *
 *  Function declarations for the EventHandler class.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "LiteStep.h"
#include "Settings.hpp"

#include <vector>

using std::vector;

class EventHandler {
public:
  explicit EventHandler(Settings* settings);
  virtual ~EventHandler();

  void HandleMessage(HWND, UINT, WPARAM, LPARAM);
  void Refresh();

  void AddHandler(LPCTSTR);
  void RemoveHandlers(LPCTSTR);

private:
  enum EventType {
    UNKNOWN = 0,
    WHEELUP, WHEELDOWN, WHEELRIGHT, WHEELLEFT,
    LEFTDOWN, LEFTUP, LEFTDOUBLE,
    MIDDLEDOWN, MIDDLEUP, MIDDLEDOUBLE,
    RIGHTDOWN, RIGHTUP, RIGHTDOUBLE,
    X1DOWN, X1UP, X1DOUBLE,
    X2DOWN, X2UP, X2DOUBLE,
    ENTER, LEAVE
  };

  typedef struct ClickData {
    EventType type;
    WORD mods; // Modifier keys
    RECT area; // Region of the virtual screen where this event is valid
    TCHAR action[MAX_LINE_LENGTH]; // Action to fire when this event occurs
  } ClickData;

  vector<ClickData> m_clickHandlers;

  Settings* settings;

  void LoadSettings(bool = false);
  ClickData ParseLine(LPCTSTR);
  EventType TypeFromString(LPCTSTR str);
  WORD ModsFromString(LPTSTR str);
  bool Matches(ClickData a, ClickData b);

  bool mouseOver;
};
