#pragma once

#include "../nCoreApi/IEventHandler.hpp"
#include "../nCoreApi/IEventProcessor.hpp"
#include "../nCoreApi/ISettingsReader.hpp"

#include <unordered_map>

class EventHandler : public IEventHandler {
public:
  enum class Type : WORD {
    UNKNOWN = 0,
    WHEELUP, WHEELDOWN, WHEELRIGHT, WHEELLEFT,
    LEFTDOWN, LEFTUP, LEFTDOUBLE,
    MIDDLEDOWN, MIDDLEUP, MIDDLEDOUBLE,
    RIGHTDOWN, RIGHTUP, RIGHTDOUBLE,
    X1DOWN, X1UP, X1DOUBLE,
    X2DOWN, X2UP, X2DOUBLE,
    ENTER, LEAVE
  };

public:
  EventHandler(const ISettingsReader *settingsReader);

public:
  EventHandler(const EventHandler&) = delete;
  EventHandler &operator=(const EventHandler&) = delete;

  // IDiscardable
public:
  void APICALL Discard() override;

  // IEventHandler
public:
  LRESULT APICALL HandleMessage(HWND, UINT message, WPARAM, LPARAM, IEventProcessor*) override;

private:
  void AddEventAction(LPCWSTR line);
  /// <summary>
  /// Parses the line.
  /// </summary>
  /// <returns>The modifier & type, 0 on error.</returns>
  DWORD ParseEventLine(LPCWSTR line, LPWSTR action, size_t cchAction);
  static WORD ModsFromString(LPWSTR str);

private:
  std::unordered_multimap<DWORD, std::wstring> mEventActions;
};
