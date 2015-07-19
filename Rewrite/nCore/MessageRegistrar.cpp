#include "../Headers/Macros.h"
#include "../Headers/Windows.h"

#include <unordered_map>
#include <unordered_set>

static std::unordered_map<UINT, std::unordered_set<HWND>> sMessageMap;


EXPORT_CDECL(void) RegisterForMessages(HWND hwnd, const UINT messages[]) {
  for (const UINT *message = messages; *message != 0; ++message) {
    sMessageMap[*message].insert(hwnd);
  }
}


EXPORT_CDECL(void) UnregisterForMessages(HWND hwnd, const UINT messages[]) {
  for (const UINT *message = messages; *message != 0; ++message) {
    sMessageMap[*message].erase(hwnd);
  }
}


void SendCoreMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  for (HWND window : sMessageMap[message]) {
    SendMessage(window, message, wParam, lParam);
  }
}


void PostCoreMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  for (HWND window : sMessageMap[message]) {
    PostMessage(window, message, wParam, lParam);
  }
}
