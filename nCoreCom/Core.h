/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *  Core.h
 *  The nModules Project
 *
 *  Functions declarations for the CoreCom library. All functions exported by
 *  nCore and some Init functions.
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#pragma once

#include "../nShared/MonitorInfo.hpp"
#include "../nShared/Versioning.h"
#include "../nCore/IParsedText.hpp"

typedef DWORD ARGB;
class DrawableWindow;

#define INIT_FUNC(var,type,name) var = (type)GetProcAddress(hCoreInstance,name); if (var == NULL) return E_NOTIMPL;

namespace nCore {
    HRESULT Connect(VERSION minVersion);
    void Disconnect();
    bool Initalized();

    VERSION GetVersion();

    namespace InputParsing {
        HRESULT _Init(HMODULE);

        bool ParseColor(LPCSTR pszColor, ARGB *target);
        bool ParseCoordinate(LPCSTR, int *, bool canBeRelative = true, bool canBeNegative = true);
        bool ParseLength(LPCSTR, int *, bool canBeRelative = true, bool canBeNegative = false);
        bool ParseMonitor(LPCSTR pszMonitor, UINT * target);
        bool ParseBool(LPCSTR pszBool);

        int GetPrefixedRCInt(LPCSTR prefix, LPCSTR option, int defValue);
        float GetPrefixedRCFloat(LPCSTR prefix, LPCSTR option, float defValue);
        double GetPrefixedRCDouble(LPCSTR prefix, LPCSTR option, double defValue);
        ARGB GetPrefixedRCColor(LPCSTR prefix, LPCSTR option, int defValue);
        bool GetPrefixedRCBool(LPCSTR prefix, LPCSTR option, bool defValue);
        bool GetPrefixedRCString(LPCSTR prefix, LPCSTR option, LPSTR pszBuffer, LPCSTR pszDefault, UINT cbBuffer);
        bool GetPrefixedRCWString(LPCSTR prefix, LPCSTR option, LPWSTR pszwBuffer, LPCSTR pszDefault, UINT cbBuffer);
        UINT GetPrefixedRCMonitor(LPCSTR prefix, LPCSTR option, UINT uDefault);
    }
    
    namespace System {
        HRESULT _Init(HMODULE);

        //
        MonitorInfo *FetchMonitorInfo();

        //
        IParsedText *ParseText(LPCWSTR text);
        BOOL RegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs, FORMATTINGPROC formatter, bool dynamic);
        BOOL UnRegisterDynamicTextFunction(LPCWSTR name, UCHAR numArgs);
        BOOL DynamicTextChangeNotification(LPCWSTR name, UCHAR numArgs);

        //
        void RegisterWindow(LPCSTR prefix, LPVOID window);
        void UnRegisterWindow(LPCSTR prefix);
        DrawableWindow *FindRegisteredWindow(LPCSTR prefix);
        void AddWindowRegistrationListener(LPCSTR prefix, DrawableWindow *listener);
        void RemoveWindowRegistrationListener(LPCSTR prefix, DrawableWindow *listener);
    }
}
