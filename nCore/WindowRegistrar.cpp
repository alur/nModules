/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowRegistrar.cpp
 *  The nModules Project
 *
 *  Maintains a list of drawables.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/DrawableWindow.hpp"
#include "../nShared/Macros.h"
#include <string>
#include <map>
#include <functional>

using std::map;
using std::string;

struct STRICmp : std::binary_function<std::string, std::string, bool> {
    bool operator()(const string &s1, const string &s2) const {
        return _stricmp(s1.c_str(), s2.c_str()) > 0;
    }
};


static map<string, DrawableWindow*, STRICmp> registeredWindows;
static map<string, list<DrawableWindow*>, STRICmp> registrationListeners;


EXPORT_CDECL(void) RegisterWindow(LPCSTR prefix, DrawableWindow *window) {
    registeredWindows[prefix] = window;
    for (auto listener : registrationListeners[prefix]) {
        listener->SetParent(window);
    }
    registrationListeners[prefix].clear();
}


EXPORT_CDECL(void) UnRegisterWindow(LPCSTR prefix) {
    registeredWindows.erase(string(prefix));
}


EXPORT_CDECL(DrawableWindow*) FindRegisteredWindow(LPCSTR prefix) {
    auto iter = registeredWindows.find(prefix);
    if (iter != registeredWindows.end()) {
        return iter->second;
    }
    return nullptr;
}


EXPORT_CDECL(void) AddWindowRegistrationListener(LPCSTR prefix, DrawableWindow *listener) {
    registrationListeners[prefix].push_back(listener);
}


EXPORT_CDECL(void) RemoveWindowRegistrationListener(LPCSTR prefix, DrawableWindow *listener) {
    registrationListeners[prefix].remove(listener);
}
