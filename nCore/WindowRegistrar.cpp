/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowRegistrar.cpp
 *  The nModules Project
 *
 *  Maintains a list of drawables.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../Utilities/StringUtils.h"

#include "../nShared/LiteStep.h"
#include "../nShared/Window.hpp"

#include <algorithm>
#include <ctype.h>
#include <functional>


static StringKeyedMaps<std::wstring, Window *>::UnorderedMap sRegisteredWindows;
static StringKeyedMaps<std::wstring, std::list<Window *>>::UnorderedMap sRegistrationListeners;


EXPORT_CDECL(void) RegisterWindow(LPCWSTR prefix, Window * window) {
  sRegisteredWindows[prefix] = window;
  for (auto listener : sRegistrationListeners[prefix]) {
    listener->SetParent(window);
  }
  sRegistrationListeners[prefix].clear();
}


EXPORT_CDECL(void) UnRegisterWindow(LPCWSTR prefix) {
  sRegisteredWindows.erase(prefix);
}


EXPORT_CDECL(Window*) FindRegisteredWindow(LPCWSTR prefix) {
  auto iter = sRegisteredWindows.find(prefix);
  if (iter != sRegisteredWindows.end()) {
    return iter->second;
  }
  return nullptr;
}


EXPORT_CDECL(void) AddWindowRegistrationListener(LPCWSTR prefix, Window * listener) {
  sRegistrationListeners[prefix].push_back(listener);
}


EXPORT_CDECL(void) RemoveWindowRegistrationListener(LPCWSTR prefix, Window * listener) {
  sRegistrationListeners[prefix].remove(listener);
}
