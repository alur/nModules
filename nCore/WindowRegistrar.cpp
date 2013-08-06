/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowRegistrar.cpp
 *  The nModules Project
 *
 *  Maintains a list of drawables.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/DrawableWindow.hpp"
#include <string>
#include <map>
#include <functional>

using std::map;
using std::wstring;

struct STRICmp : std::binary_function<wstring, wstring, bool>
{
    bool operator()(const wstring &s1, const wstring &s2) const
    {
        return _wcsicmp(s1.c_str(), s2.c_str()) > 0;
    }
};


static map<wstring, DrawableWindow*, STRICmp> registeredWindows;
static map<wstring, list<DrawableWindow*>, STRICmp> registrationListeners;


EXPORT_CDECL(void) RegisterWindow(LPCWSTR prefix, DrawableWindow *window)
{
    registeredWindows[prefix] = window;
    for (auto listener : registrationListeners[prefix])
    {
        listener->SetParent(window);
    }
    registrationListeners[prefix].clear();
}


EXPORT_CDECL(void) UnRegisterWindow(LPCWSTR prefix)
{
    registeredWindows.erase(prefix);
}


EXPORT_CDECL(DrawableWindow*) FindRegisteredWindow(LPCWSTR prefix)
{
    auto iter = registeredWindows.find(prefix);
    if (iter != registeredWindows.end())
    {
        return iter->second;
    }
    return nullptr;
}


EXPORT_CDECL(void) AddWindowRegistrationListener(LPCWSTR prefix, DrawableWindow *listener)
{
    registrationListeners[prefix].push_back(listener);
}


EXPORT_CDECL(void) RemoveWindowRegistrationListener(LPCWSTR prefix, DrawableWindow *listener)
{
    registrationListeners[prefix].remove(listener);
}
