/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  WindowRegistrar.cpp
 *  The nModules Project
 *
 *  Maintains a list of drawables.
 *  
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include "../nShared/LiteStep.h"
#include "../nShared/Window.hpp"
#include <string>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <ctype.h>


using std::unordered_map;
using std::wstring;


struct wstring_nocase
{
    wstring_nocase(LPCWSTR str)
        : str(str)
    {
        std::transform(this->str.begin(), this->str.end(), this->str.begin(), ::towlower);
    }
    std::wstring str;
};

namespace std
{
    template<>
    struct hash<wstring_nocase>
    {
        size_t operator()(wstring_nocase const & s) const
        {
            return hash<wstring>()(s.str);
        }
    };

    template<>
    struct equal_to<wstring_nocase>
    {
        bool operator()(wstring_nocase const & s1, wstring_nocase const & s2) const
        {
            return std::equal_to<wstring>()(s1.str, s2.str);
        }
    };
}


static unordered_map<wstring_nocase, Window*> registeredWindows;
static unordered_map<wstring_nocase, list<Window*> > registrationListeners;


EXPORT_CDECL(void) RegisterWindow(LPCWSTR prefix, Window *window)
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


EXPORT_CDECL(Window*) FindRegisteredWindow(LPCWSTR prefix)
{
    auto iter = registeredWindows.find(prefix);
    if (iter != registeredWindows.end())
    {
        return iter->second;
    }
    return nullptr;
}


EXPORT_CDECL(void) AddWindowRegistrationListener(LPCWSTR prefix, Window *listener)
{
    registrationListeners[prefix].push_back(listener);
}


EXPORT_CDECL(void) RemoveWindowRegistrationListener(LPCWSTR prefix, Window *listener)
{
    registrationListeners[prefix].remove(listener);
}
