/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowRegistrar.cpp
 *  The nModules Project
 *
 *  Maintains a list of drawables.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/Macros.h"
#include "WindowRegistrar.h"
#include <string>
#include <map>

using std::map;
using std::string;

map<string, LPVOID> registeredWindows;


EXPORT_CDECL(void) RegisterWindow(LPCSTR prefix, LPVOID window) {
    registeredWindows.insert(std::pair<string, LPVOID>(string(prefix), window));
}


EXPORT_CDECL(void) UnRegisterWindow(LPCSTR prefix) {
    registeredWindows.erase(string(prefix));
}


EXPORT_CDECL(LPVOID) FindRegisteredWindow(LPCSTR prefix) {
    map<string, LPVOID>::const_iterator iter = registeredWindows.find(string(prefix));
    if (iter != registeredWindows.end()) {
        return iter->second;
    }
    return NULL;
}