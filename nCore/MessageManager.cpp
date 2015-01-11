//-------------------------------------------------------------------------------------------------
// /nCore/MessageManager.cpp
// The nModules Project
//
// Main .cpp file for the nCore module.
//-------------------------------------------------------------------------------------------------
#include "../Utilities/Common.h"
#include "../Utilities/Macros.h"

#include <unordered_map>
#include <unordered_set>

static std::unordered_map<UINT, std::unordered_set<HWND>> sMessageMap;


EXPORT_CDECL(void) RegisterForCoreMessages(HWND hwnd, const UINT messages[]) {
  for (const UINT *message = messages; *message != 0; ++message) {
    sMessageMap[*message].insert(hwnd);
  }
}


EXPORT_CDECL(void) UnregisterForCoreMessages(HWND hwnd, const UINT messages[]) {
  for (const UINT *message = messages; *message != 0; ++message) {
    sMessageMap[*message].erase(hwnd);
  }
}


void SendCoreMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  for (HWND window : sMessageMap[message]) {
    SendMessage(window, message, wParam, lParam);
  }
}
